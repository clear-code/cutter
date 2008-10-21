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
#  include "../cutter/config.h"
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/wait.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "gcut-io.h"
#include "gcut-spawn.h"
#include <cutter/cut-marshalers.h>

#define GCUT_SPAWN_GET_PRIVATE(obj)                                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), GCUT_TYPE_SPAWN, GCutSpawnPrivate))

typedef struct _WatchOutputData
{
    GCutSpawn *spawn;
    guint signal;
} WatchOutputData;

typedef struct _GCutSpawnPrivate	GCutSpawnPrivate;
struct _GCutSpawnPrivate
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

G_DEFINE_TYPE(GCutSpawn, gcut_spawn, G_TYPE_OBJECT)

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
gcut_spawn_class_init (GCutSpawnClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_pointer("command",
                                "Command",
                                "The command to be ran by the spawn",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COMMAND, spec);

    signals[OUTPUT_RECEIVED]
        = g_signal_new("output-received",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutSpawnClass, output_received),
                       NULL, NULL,
#if GLIB_SIZEOF_SIZE_T == 8
                       _cut_marshal_VOID__STRING_UINT64,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT64
#else
                       _cut_marshal_VOID__STRING_UINT,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT
#endif
                       );

    signals[ERROR_RECEIVED]
        = g_signal_new("error-received",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutSpawnClass, error_received),
                       NULL, NULL,
#if GLIB_SIZEOF_SIZE_T == 8
                       _cut_marshal_VOID__STRING_UINT64,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT64
#else
                       _cut_marshal_VOID__STRING_UINT,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT
#endif
                       );

    signals[REAPED]
        = g_signal_new("reaped",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutSpawnClass, reaped),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__INT,
                       G_TYPE_NONE, 1, G_TYPE_INT);

    signals[ERROR]
        = g_signal_new("error",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutSpawnClass, error),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__POINTER,
                       G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_type_class_add_private(gobject_class, sizeof(GCutSpawnPrivate));
}

static void
gcut_spawn_init (GCutSpawn *spawn)
{
    GCutSpawnPrivate *priv = GCUT_SPAWN_GET_PRIVATE(spawn);

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
}

static void
remove_child_watch_func (GCutSpawnPrivate *priv)
{
    g_source_remove(priv->process_watch_id);
    priv->process_watch_id = 0;
}

static void
close_child (GCutSpawnPrivate *priv)
{
    g_spawn_close_pid(priv->pid);
    priv->pid = 0;
}

static void
remove_output_watch_func (GCutSpawnPrivate *priv)
{
    g_source_remove(priv->output_watch_id);
    priv->output_watch_id = 0;
}

static void
remove_error_watch_func (GCutSpawnPrivate *priv)
{
    g_source_remove(priv->error_watch_id);
    priv->error_watch_id = 0;
}

static void
dispose (GObject *object)
{
    GCutSpawnPrivate *priv = GCUT_SPAWN_GET_PRIVATE(object);

    if (priv->command) {
        g_strfreev(priv->command);
        priv->command = NULL;
    }

    if (priv->process_watch_id)
        remove_child_watch_func(priv);

    if (priv->pid) {
        gcut_spawn_close(GCUT_SPAWN(object));
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

    G_OBJECT_CLASS(gcut_spawn_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GCutSpawnPrivate *priv = GCUT_SPAWN_GET_PRIVATE(object);

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
    GCutSpawnPrivate *priv = GCUT_SPAWN_GET_PRIVATE(object);

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
gcut_spawn_error_quark (void)
{
    return g_quark_from_static_string("gcut-spawn-error-quark");
}

GCutSpawn *
gcut_spawn_new (const gchar *command, ...)
{
    GCutSpawn *spawn;
    va_list args;

    va_start(args, command);
    spawn = gcut_spawn_new_va_list(command, args);
    va_end(args);

    return spawn;
}

GCutSpawn *
gcut_spawn_new_va_list (const gchar *command, va_list args)
{
    GCutSpawn *spawn;
    GArray *command_line;

    command_line = g_array_new(TRUE, TRUE, sizeof(gchar *));
    while (command) {
        g_array_append_val(command_line, command);
        command = va_arg(args, const gchar *);
    }
    spawn = gcut_spawn_new_strings((const gchar **)(command_line->data));
    g_array_free(command_line, TRUE);

    return spawn;
}

GCutSpawn *
gcut_spawn_new_argv (gint args, gchar **argv)
{
    GCutSpawn *spawn;
    gint i;
    GArray *command_line;

    command_line = g_array_new(TRUE, TRUE, sizeof(gchar *));
    for (i = 0; i < args && argv[i]; i++) {
        const gchar *command = argv[i];
        g_array_append_val(command_line, command);
    }
    spawn = gcut_spawn_new_strings((const gchar **)(command_line->data));
    g_array_free(command_line, TRUE);

    return spawn;
}

GCutSpawn *
gcut_spawn_new_strings (const gchar **commands)
{
    return g_object_new(GCUT_TYPE_SPAWN,
                        "command", commands,
                        NULL);
}

GSpawnFlags
gcut_spawn_get_flags (GCutSpawn *spawn)
{
    return GCUT_SPAWN_GET_PRIVATE(spawn)->flags;
}

void
gcut_spawn_set_flags (GCutSpawn *spawn, GSpawnFlags flags)
{
    GCUT_SPAWN_GET_PRIVATE(spawn)->flags = flags;
}

static void
reap_child (GCutSpawn *spawn, GPid pid)
{
    GCutSpawnPrivate *priv = GCUT_SPAWN_GET_PRIVATE(spawn);
    gint status;

    if (priv->pid != pid)
        return;

    remove_child_watch_func(priv);
    waitpid(pid, &status, 0);
    g_signal_emit(spawn, signals[REAPED], 0, status);
    close_child(priv);
}

static void
child_watch_func (GPid pid, gint status, gpointer data)
{
    GCutSpawn *spawn = data;
    GCutSpawnPrivate *priv;

    priv = GCUT_SPAWN_GET_PRIVATE(spawn);
    reap_child(spawn, pid);
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

    *watch_id = g_io_add_watch(channel,
                               G_IO_IN | G_IO_PRI |
                               G_IO_ERR | G_IO_HUP | G_IO_NVAL,
                               watch_func, user_data);

    return channel;
}

#define BUFFER_SIZE 4096
static gboolean
read_from_io_channel (GIOChannel *channel, GCutSpawn *spawn, guint signal)
{
    GCutSpawnPrivate *priv;
    gboolean need_more_data = TRUE;

    priv = GCUT_SPAWN_GET_PRIVATE(spawn);
    while (need_more_data) {
        GIOStatus status;
        gchar stream[BUFFER_SIZE + 1];
        gsize length = 0;
        GError *error = NULL;

        status = g_io_channel_read_chars(channel, stream, BUFFER_SIZE,
                                         &length, &error);
        if (status == G_IO_STATUS_EOF)
            need_more_data = FALSE;

        if (error) {
            g_signal_emit(spawn, signals[ERROR], 0, error);
            g_error_free(error);
            need_more_data = TRUE;
            break;
        }

        if (length <= 0)
            break;

        g_signal_emit(spawn, signal, 0, stream, length);
    }

    return need_more_data;
}

static gboolean
watch_output (GIOChannel *source, GIOCondition condition, gpointer user_data)
{
    WatchOutputData *data = user_data;
    gboolean keep_callback = TRUE;

    if (condition & (G_IO_IN | G_IO_PRI)) {
        keep_callback = read_from_io_channel(source, data->spawn, data->signal);
    }

    if (condition & (G_IO_ERR | G_IO_NVAL)) {
        gchar *message;
        GError *error;

        message = gcut_io_inspect_condition(condition);
        error = g_error_new(GCUT_SPAWN_ERROR, GCUT_SPAWN_ERROR_IO_ERROR,
                            "%s", message);
        g_signal_emit(data->spawn, signals[ERROR], 0, error);
        g_free(message);
        g_error_free(error);
        keep_callback = FALSE;
    }

    if (condition & G_IO_HUP)
        keep_callback = FALSE;

    return keep_callback;
}

gboolean
gcut_spawn_run (GCutSpawn *spawn, GError **error)
{
    GCutSpawnPrivate *priv;
    gboolean success;
    gint input_fd = 0, output_fd = 0, error_fd = 0;

    priv = GCUT_SPAWN_GET_PRIVATE(spawn);

    if (!priv->command) {
        g_set_error(error, GCUT_SPAWN_ERROR, GCUT_SPAWN_ERROR_COMMAND_LINE,
                    "command line isn't set");
        return FALSE;
    }

    if (priv->pid > 0) {
        gchar *command;

        command = g_strjoinv(" ", priv->command);
        g_set_error(error, GCUT_SPAWN_ERROR, GCUT_SPAWN_ERROR_ALREADY_RUNNING,
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
                                               spawn);

    if (input_fd > 0)
        priv->input = create_output_channel(input_fd);

    if (output_fd > 0) {
        priv->watch_output_data = g_new(WatchOutputData, 1);
        priv->watch_output_data->spawn = spawn;
        priv->watch_output_data->signal = signals[OUTPUT_RECEIVED];
        priv->output = create_input_channel(output_fd,
                                            &(priv->output_watch_id),
                                            watch_output,
                                            priv->watch_output_data);
    }
    if (error_fd > 0) {
        priv->watch_error_data = g_new(WatchOutputData, 1);
        priv->watch_error_data->spawn = spawn;
        priv->watch_error_data->signal = signals[ERROR_RECEIVED];
        priv->error = create_input_channel(error_fd,
                                           &(priv->error_watch_id),
                                           watch_output,
                                           priv->watch_error_data);
    }

    return TRUE;
}

void
gcut_spawn_close (GCutSpawn *spawn)
{
    GCutSpawnPrivate *priv;

    priv = GCUT_SPAWN_GET_PRIVATE(spawn);
    if (priv->pid) {
        g_spawn_close_pid(priv->pid);
        priv->pid = 0;
    }
}

gboolean
gcut_spawn_write (GCutSpawn *spawn, const gchar *chunk, gsize size,
                  GError **error)
{
    GCutSpawnPrivate *priv;
    gsize rest_size = size;

    priv = GCUT_SPAWN_GET_PRIVATE(spawn);
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
gcut_spawn_get_pid (GCutSpawn *spawn)
{
    return GCUT_SPAWN_GET_PRIVATE(spawn)->pid;
}

GIOChannel *
gcut_spawn_get_input (GCutSpawn *spawn)
{
    return GCUT_SPAWN_GET_PRIVATE(spawn)->input;
}

GIOChannel *
gcut_spawn_get_output (GCutSpawn *spawn)
{
    return GCUT_SPAWN_GET_PRIVATE(spawn)->output;
}

GIOChannel *
gcut_spawn_get_error (GCutSpawn *spawn)
{
    return GCUT_SPAWN_GET_PRIVATE(spawn)->error;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
