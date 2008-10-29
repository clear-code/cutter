/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "gcut-io.h"
#include "gcut-egg.h"
#include "gcut-marshalers.h"

#define GCUT_EGG_GET_PRIVATE(obj)                                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), GCUT_TYPE_EGG, GCutEggPrivate))

typedef struct _WatchOutputData
{
    GCutEgg *egg;
    guint signal;
} WatchOutputData;

typedef struct _GCutEggPrivate	GCutEggPrivate;
struct _GCutEggPrivate
{
    gchar **command;
    GSpawnFlags flags;
    GSpawnFlags must_flags;

    GPid pid;
    guint process_watch_id;

    GIOChannel *input;
    GIOChannel *output;
    GIOChannel *error;

    guint output_watch_id;
    guint error_watch_id;

    WatchOutputData *watch_output_data;
    WatchOutputData *watch_error_data;

    guint kill_wait_milliseconds;
};

enum
{
    PROP_0,
    PROP_COMMAND
};

enum
{
    START_SIGNAL,

    OUTPUT_RECEIVED,
    ERROR_RECEIVED,
    REAPED,
    ERROR,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE(GCutEgg, gcut_egg, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
gcut_egg_class_init (GCutEggClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_pointer("command",
                                "Command",
                                "The command to be ran by the egg",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COMMAND, spec);

    signals[OUTPUT_RECEIVED]
        = g_signal_new("output-received",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutEggClass, output_received),
                       NULL, NULL,
#if GLIB_SIZEOF_SIZE_T == 8
                       _gcut_marshal_VOID__STRING_UINT64,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT64
#else
                       _gcut_marshal_VOID__STRING_UINT,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT
#endif
                       );

    signals[ERROR_RECEIVED]
        = g_signal_new("error-received",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutEggClass, error_received),
                       NULL, NULL,
#if GLIB_SIZEOF_SIZE_T == 8
                       _gcut_marshal_VOID__STRING_UINT64,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT64
#else
                       _gcut_marshal_VOID__STRING_UINT,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT
#endif
                       );

    signals[REAPED]
        = g_signal_new("reaped",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutEggClass, reaped),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__INT,
                       G_TYPE_NONE, 1, G_TYPE_INT);

    signals[ERROR]
        = g_signal_new("error",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutEggClass, error),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__POINTER,
                       G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_type_class_add_private(gobject_class, sizeof(GCutEggPrivate));
}

static void
gcut_egg_init (GCutEgg *egg)
{
    GCutEggPrivate *priv = GCUT_EGG_GET_PRIVATE(egg);

    priv->command = NULL;
    priv->flags = G_SPAWN_SEARCH_PATH;
    priv->must_flags = G_SPAWN_DO_NOT_REAP_CHILD;

    priv->pid = 0;
    priv->process_watch_id = 0;

    priv->input = NULL;
    priv->output = NULL;
    priv->error = NULL;

    priv->output_watch_id = 0;
    priv->error_watch_id = 0;

    priv->watch_output_data = NULL;
    priv->watch_error_data = NULL;

    priv->kill_wait_milliseconds = 10;
}

static void
remove_child_watch_func (GCutEggPrivate *priv)
{
    g_source_remove(priv->process_watch_id);
    priv->process_watch_id = 0;
}

static void
remove_output_watch_func (GCutEggPrivate *priv)
{
    g_source_remove(priv->output_watch_id);
    priv->output_watch_id = 0;
}

static void
remove_error_watch_func (GCutEggPrivate *priv)
{
    g_source_remove(priv->error_watch_id);
    priv->error_watch_id = 0;
}

static gboolean
cb_timeout_kill_wait (gpointer user_data)
{
    gboolean *waiting = user_data;

    *waiting = FALSE;

    return FALSE;
}

static void
dispose (GObject *object)
{
    GCutEggPrivate *priv;

    priv = GCUT_EGG_GET_PRIVATE(object);
    if (priv->command) {
        g_strfreev(priv->command);
        priv->command = NULL;
    }

    if (priv->process_watch_id && priv->pid) {
        gcut_egg_kill(GCUT_EGG(object), SIGTERM);
        if (priv->kill_wait_milliseconds > 0) {
            gboolean waiting = TRUE;
            guint timeout_kill_wait_id;

            timeout_kill_wait_id = g_timeout_add(priv->kill_wait_milliseconds,
                                                 cb_timeout_kill_wait,
                                                 &waiting);
            while (waiting && priv->process_watch_id && priv->pid)
                g_main_context_iteration(NULL, TRUE);
            g_source_remove(timeout_kill_wait_id);
        }
    }

    if (priv->process_watch_id)
        remove_child_watch_func(priv);

    if (priv->pid) {
        gcut_egg_close(GCUT_EGG(object));
    }

    if (priv->input) {
        g_io_channel_unref(priv->input);
        priv->input = NULL;
    }

    if (priv->output_watch_id)
        remove_output_watch_func(priv);

    if (priv->error_watch_id)
        remove_error_watch_func(priv);

    if (priv->output) {
        g_io_channel_unref(priv->output);
        priv->output = NULL;
    }

    if (priv->error) {
        g_io_channel_unref(priv->error);
        priv->error = NULL;
    }

    if (priv->watch_output_data) {
        g_free(priv->watch_output_data);
        priv->watch_output_data = NULL;
    }

    if (priv->watch_error_data) {
        g_free(priv->watch_error_data);
        priv->watch_error_data = NULL;
    }

    G_OBJECT_CLASS(gcut_egg_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GCutEggPrivate *priv = GCUT_EGG_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_COMMAND:
        if (priv->command)
            g_strfreev(priv->command);
        priv->command = g_strdupv(g_value_get_pointer(value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    GCutEggPrivate *priv = GCUT_EGG_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_COMMAND:
        g_value_set_pointer(value, priv->command);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

GQuark
gcut_egg_error_quark (void)
{
    return g_quark_from_static_string("gcut-egg-error-quark");
}

GCutEgg *
gcut_egg_new (const gchar *command, ...)
{
    GCutEgg *egg;
    va_list args;

    va_start(args, command);
    egg = gcut_egg_new_va_list(command, args);
    va_end(args);

    return egg;
}

GCutEgg *
gcut_egg_new_va_list (const gchar *command, va_list args)
{
    GCutEgg *egg;
    GArray *command_line;

    command_line = g_array_new(TRUE, TRUE, sizeof(gchar *));
    while (command) {
        g_array_append_val(command_line, command);
        command = va_arg(args, const gchar *);
    }
    egg = gcut_egg_new_strings((const gchar **)(command_line->data));
    g_array_free(command_line, TRUE);

    return egg;
}

GCutEgg *
gcut_egg_new_argv (gint args, gchar **argv)
{
    GCutEgg *egg;
    gint i;
    GArray *command_line;

    command_line = g_array_new(TRUE, TRUE, sizeof(gchar *));
    for (i = 0; i < args && argv[i]; i++) {
        const gchar *command = argv[i];
        g_array_append_val(command_line, command);
    }
    egg = gcut_egg_new_strings((const gchar **)(command_line->data));
    g_array_free(command_line, TRUE);

    return egg;
}

GCutEgg *
gcut_egg_new_strings (const gchar **commands)
{
    return g_object_new(GCUT_TYPE_EGG,
                        "command", commands,
                        NULL);
}

GSpawnFlags
gcut_egg_get_flags (GCutEgg *egg)
{
    return GCUT_EGG_GET_PRIVATE(egg)->flags;
}

void
gcut_egg_set_flags (GCutEgg *egg, GSpawnFlags flags)
{
    GCUT_EGG_GET_PRIVATE(egg)->flags = flags;
}

static void
reap_child (GCutEgg *egg, GPid pid, gint status)
{
    GCutEggPrivate *priv = GCUT_EGG_GET_PRIVATE(egg);

    if (priv->pid != pid)
        return;

    remove_child_watch_func(priv);
    g_signal_emit(egg, signals[REAPED], 0, status);
    g_spawn_close_pid(priv->pid);
    priv->pid = 0;
}

static void
child_watch_func (GPid pid, gint status, gpointer data)
{
    GCutEgg *egg = data;
    GCutEggPrivate *priv;

    priv = GCUT_EGG_GET_PRIVATE(egg);
    reap_child(egg, pid, status);
}

static GIOChannel *
create_output_channel (gint fd)
{
    GIOChannel *channel;

#ifndef G_OS_WIN32
    channel = g_io_channel_unix_new(fd);
#else
    channel = g_io_channel_win32_new_fd(fd);
#endif
    if (!channel)
        return NULL;

    g_io_channel_set_close_on_unref(channel, TRUE);

    return channel;
}

static GIOChannel *
create_input_channel (gint fd, guint *watch_id,
                      GIOFunc watch_func, gpointer user_data)
{
    GIOChannel *channel;

#ifndef G_OS_WIN32
    channel = g_io_channel_unix_new(fd);
#else
    channel = g_io_channel_win32_new_fd(fd);
#endif
    if (!channel)
        return NULL;

    g_io_channel_set_close_on_unref(channel, TRUE);
    g_io_channel_set_flags(channel, G_IO_FLAG_NONBLOCK, NULL);

    *watch_id = g_io_add_watch(channel,
                               G_IO_IN | G_IO_PRI |
                               G_IO_ERR | G_IO_HUP | G_IO_NVAL,
                               watch_func, user_data);

    return channel;
}

#define BUFFER_SIZE 4096
static gboolean
read_from_io_channel (GIOChannel *channel, GCutEgg *egg, guint signal)
{
    GCutEggPrivate *priv;
    gboolean need_more_data = TRUE;

    priv = GCUT_EGG_GET_PRIVATE(egg);
    while (need_more_data) {
        GIOStatus status;
        gchar stream[BUFFER_SIZE];
        gsize length = 0;
        GError *error = NULL;

        status = g_io_channel_read_chars(channel, stream, BUFFER_SIZE,
                                         &length, &error);
        if (error) {
            g_signal_emit(egg, signals[ERROR], 0, error);
            g_error_free(error);
            need_more_data = FALSE;
            break;
        }

        if (status == G_IO_STATUS_EOF)
            need_more_data = FALSE;

        if (status == G_IO_STATUS_AGAIN ||
            length == 0)
            break;

        g_signal_emit(egg, signal, 0, stream, length);
    }

    return need_more_data;
}

static gboolean
watch_output (GIOChannel *source, GIOCondition condition, gpointer user_data)
{
    WatchOutputData *data = user_data;
    gboolean keep_callback = TRUE;

    if (condition & (G_IO_IN | G_IO_PRI)) {
        keep_callback = read_from_io_channel(source, data->egg, data->signal);
    }

    if (condition & (G_IO_ERR | G_IO_NVAL)) {
        gchar *message;
        GError *error;

        message = gcut_io_inspect_condition(condition);
        error = g_error_new(GCUT_EGG_ERROR, GCUT_EGG_ERROR_IO_ERROR,
                            "%s", message);
        g_signal_emit(data->egg, signals[ERROR], 0, error);
        g_free(message);
        g_error_free(error);
        keep_callback = FALSE;
    }

    if (condition & G_IO_HUP)
        keep_callback = FALSE;

    return keep_callback;
}

gboolean
gcut_egg_hatch (GCutEgg *egg, GError **error)
{
    GCutEggPrivate *priv;
    gboolean success;
    gint input_fd = 0, output_fd = 0, error_fd = 0;

    priv = GCUT_EGG_GET_PRIVATE(egg);

    if (!priv->command) {
        g_set_error(error, GCUT_EGG_ERROR, GCUT_EGG_ERROR_COMMAND_LINE,
                    "command line isn't set");
        return FALSE;
    }

    if (priv->pid > 0) {
        gchar *command;

        command = g_strjoinv(" ", priv->command);
        g_set_error(error, GCUT_EGG_ERROR, GCUT_EGG_ERROR_ALREADY_RUNNING,
                    "already running: %s", command);
        g_free(command);

        return FALSE;
    }

    success = g_spawn_async_with_pipes(NULL,
                                       priv->command,
                                       NULL,
                                       priv->flags | priv->must_flags,
                                       NULL,
                                       NULL,
                                       &priv->pid,
                                       &input_fd,
                                       &output_fd,
                                       &error_fd,
                                       error);

    if (!success)
        return FALSE;

    priv->process_watch_id = g_child_watch_add(priv->pid,
                                               child_watch_func,
                                               egg);

    if (input_fd > 0)
        priv->input = create_output_channel(input_fd);

    if (output_fd > 0) {
        priv->watch_output_data = g_new(WatchOutputData, 1);
        priv->watch_output_data->egg = egg;
        priv->watch_output_data->signal = signals[OUTPUT_RECEIVED];
        priv->output = create_input_channel(output_fd,
                                            &(priv->output_watch_id),
                                            watch_output,
                                            priv->watch_output_data);
    }
    if (error_fd > 0) {
        priv->watch_error_data = g_new(WatchOutputData, 1);
        priv->watch_error_data->egg = egg;
        priv->watch_error_data->signal = signals[ERROR_RECEIVED];
        priv->error = create_input_channel(error_fd,
                                           &(priv->error_watch_id),
                                           watch_output,
                                           priv->watch_error_data);
    }

    return TRUE;
}

void
gcut_egg_close (GCutEgg *egg)
{
    GCutEggPrivate *priv;

    priv = GCUT_EGG_GET_PRIVATE(egg);
    if (priv->pid) {
        g_spawn_close_pid(priv->pid);
        priv->pid = 0;
    }
}

gboolean
gcut_egg_write (GCutEgg *egg, const gchar *chunk, gsize size,
                  GError **error)
{
    GCutEggPrivate *priv;
    gsize rest_size = size;

    priv = GCUT_EGG_GET_PRIVATE(egg);
    while (rest_size > 0) {
        gsize written_size = 0;
        GIOStatus status;

        status = g_io_channel_write_chars(priv->input,
                                          chunk, rest_size,
                                          &written_size,
                                          error);
        if (status == G_IO_STATUS_ERROR)
            return FALSE;

        if (written_size == 0)
            return FALSE;

        rest_size -= written_size;
        chunk += written_size;
    }

    return TRUE;
}

GPid
gcut_egg_get_pid (GCutEgg *egg)
{
    return GCUT_EGG_GET_PRIVATE(egg)->pid;
}

void
gcut_egg_kill (GCutEgg *egg, int signal_number)
{
    GCutEggPrivate *priv;

    priv = GCUT_EGG_GET_PRIVATE(egg);

    g_return_if_fail(priv != NULL);
    g_return_if_fail(priv->pid > 0);

    kill(priv->pid, signal_number);
}

GIOChannel *
gcut_egg_get_input (GCutEgg *egg)
{
    return GCUT_EGG_GET_PRIVATE(egg)->input;
}

GIOChannel *
gcut_egg_get_output (GCutEgg *egg)
{
    return GCUT_EGG_GET_PRIVATE(egg)->output;
}

GIOChannel *
gcut_egg_get_error (GCutEgg *egg)
{
    return GCUT_EGG_GET_PRIVATE(egg)->error;
}

guint
gcut_egg_get_kill_wait_milliseconds (GCutEgg *egg)
{
    return GCUT_EGG_GET_PRIVATE(egg)->kill_wait_milliseconds;
}

void
gcut_egg_set_kill_wait_milliseconds (GCutEgg *egg, guint milliseconds)
{
    GCUT_EGG_GET_PRIVATE(egg)->kill_wait_milliseconds = milliseconds;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
