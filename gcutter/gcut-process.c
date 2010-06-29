/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
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
#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif
#include <signal.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <errno.h>

#include "gcut-io.h"
#include "gcut-process.h"
#include "gcut-marshalers.h"
#include "cut-utils.h"

#define GCUT_PROCESS_GET_PRIVATE(obj)                                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), GCUT_TYPE_PROCESS, GCutProcessPrivate))

typedef struct _WatchOutputData
{
    GCutProcess *process;
    guint signal;
} WatchOutputData;

typedef struct _GCutProcessPrivate	GCutProcessPrivate;
struct _GCutProcessPrivate
{
    gchar **command;
    GArray *env;
    GSpawnFlags flags;
    GSpawnFlags must_flags;

    GPid pid;
    gint status;
    guint process_watch_id;

    GIOChannel *input;
    GIOChannel *output;
    GIOChannel *error;

#ifdef CUT_SUPPORT_GIO
    GOutputStream *input_stream;
    GInputStream *output_stream;
    GInputStream *error_stream;
#endif

    guint output_watch_id;
    guint error_watch_id;

    WatchOutputData *watch_output_data;
    WatchOutputData *watch_error_data;

    guint forced_termination_wait_time;
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

G_DEFINE_TYPE(GCutProcess, gcut_process, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void output_received (GCutProcess *process,
                             const gchar *chunk,
                             gsize        size);
static void error_received  (GCutProcess *process,
                             const gchar *chunk,
                             gsize        size);

static void
gcut_process_class_init (GCutProcessClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->output_received = output_received;
    klass->error_received = error_received;

    spec = g_param_spec_pointer("command",
                                "Command",
                                "The command to be ran by the process",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COMMAND, spec);

    /**
     * GCutProcess::output-received:
     * @process: the object which received the signal.
     * @chunk: the chunk read from an external process's
     *         standard output.
     * @size: the size of @chunk. (%gsize)
     *
     * It is emitted each time an external process outputs
     * something to its standard output and it is read.
     *
     * Note that you need to run GLib's main loop by
     * g_main_loop_run(), g_main_context_iteration() and so
     * on for detecting an external process's output is
     * readable.
     *
     * Since: 1.1.5
     */
    signals[OUTPUT_RECEIVED]
        = g_signal_new("output-received",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutProcessClass, output_received),
                       NULL, NULL,
#if GLIB_SIZEOF_SIZE_T == 8
                       _gcut_marshal_VOID__STRING_UINT64,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT64
#else
                       _gcut_marshal_VOID__STRING_UINT,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT
#endif
                       );

    /**
     * GCutProcess::error-received:
     * @process: the object which received the signal.
     * @chunk: the chunk read from an external process's
     *         standard error output.
     * @size: the size of @chunk. (%gsize)
     *
     * It is emitted each time an external process outputs
     * something to its standard error output and it is
     * read.
     *
     * Note that you need to run GLib's main loop by
     * g_main_loop_run(), g_main_context_iteration() and so
     * on for detecting an external process's output is
     * readable.
     *
     * Since: 1.0.5
     */
    signals[ERROR_RECEIVED]
        = g_signal_new("error-received",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutProcessClass, error_received),
                       NULL, NULL,
#if GLIB_SIZEOF_SIZE_T == 8
                       _gcut_marshal_VOID__STRING_UINT64,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT64
#else
                       _gcut_marshal_VOID__STRING_UINT,
                       G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT
#endif
                       );

    /**
     * GCutProcess::reaped:
     * @process: the object which received the signal.
     * @status: the exit status of an external process.
     *
     * It is emitted when an external process is exited.
     *
     * Note that you need to run GLib's main loop by
     * g_main_loop_run(), g_main_context_iteration() and so
     * on for detecting an external process is exited.
     *
     * Since: 1.0.5
     */
    signals[REAPED]
        = g_signal_new("reaped",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutProcessClass, reaped),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__INT,
                       G_TYPE_NONE, 1, G_TYPE_INT);

    /**
     * GCutProcess::error:
     * @process: the object which received the signal.
     * @error: the error of an external process. (%GError)
     *
     * It is emitted each time an external process causes an
     * error. (e.g. IO error)
     *
     * Since: 1.0.5
     */
    signals[ERROR]
        = g_signal_new("error",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GCutProcessClass, error),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__POINTER,
                       G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_type_class_add_private(gobject_class, sizeof(GCutProcessPrivate));
}

static void
gcut_process_init (GCutProcess *process)
{
    GCutProcessPrivate *priv = GCUT_PROCESS_GET_PRIVATE(process);

    priv->command = NULL;
    priv->env = NULL;
    priv->flags = G_SPAWN_SEARCH_PATH;
    priv->must_flags = G_SPAWN_DO_NOT_REAP_CHILD;

    priv->pid = 0;
    priv->status = 0;
    priv->process_watch_id = 0;

    priv->input = NULL;
    priv->output = NULL;
    priv->error = NULL;

#ifdef CUT_SUPPORT_GIO
    priv->input_stream = g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    priv->output_stream = g_memory_input_stream_new();
    priv->error_stream = g_memory_input_stream_new();
#endif

    priv->output_watch_id = 0;
    priv->error_watch_id = 0;

    priv->watch_output_data = NULL;
    priv->watch_error_data = NULL;

    priv->forced_termination_wait_time = 10;
}

static void
remove_child_watch_func (GCutProcessPrivate *priv)
{
    g_source_remove(priv->process_watch_id);
    priv->process_watch_id = 0;
}

static void
remove_output_watch_func (GCutProcessPrivate *priv)
{
    g_source_remove(priv->output_watch_id);
    priv->output_watch_id = 0;
}

static void
remove_error_watch_func (GCutProcessPrivate *priv)
{
    g_source_remove(priv->error_watch_id);
    priv->error_watch_id = 0;
}

static gboolean
cb_timeout_wait (gpointer user_data)
{
    gboolean *is_timeout = user_data;
    *is_timeout = TRUE;
    return FALSE;
}

static void
dispose_io_channel (GCutProcessPrivate *priv)
{
    if (priv->input) {
        g_io_channel_unref(priv->input);
        priv->input = NULL;
    }

    if (priv->output_watch_id > 0)
        remove_output_watch_func(priv);

    if (priv->error_watch_id > 0)
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
}

#ifdef CUT_SUPPORT_GIO
static void
dispose_streams (GCutProcessPrivate *priv)
{
    if (priv->input_stream) {
        g_object_unref(priv->input_stream);
        priv->input_stream = NULL;
    }
    if (priv->output_stream) {
        g_object_unref(priv->output_stream);
        priv->output_stream = NULL;
    }
    if (priv->error_stream) {
        g_object_unref(priv->error_stream);
        priv->error_stream = NULL;
    }
}
#endif

static void
gcut_process_close (GCutProcess *process)
{
    GCutProcessPrivate *priv;

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    if (priv->pid) {
        g_spawn_close_pid(priv->pid);
        priv->pid = 0;
    }
}

static void
dispose (GObject *object)
{
    GCutProcessPrivate *priv;

    priv = GCUT_PROCESS_GET_PRIVATE(object);
    if (priv->command) {
        g_strfreev(priv->command);
        priv->command = NULL;
    }

    if (priv->process_watch_id && priv->pid) {
        GError *error = NULL;
        gcut_process_kill(GCUT_PROCESS(object), SIGTERM, &error);
        if (priv->forced_termination_wait_time > 0) {
            gboolean is_timeout = FALSE;
            guint timeout_wait_id;

            timeout_wait_id = g_timeout_add(priv->forced_termination_wait_time,
                                            cb_timeout_wait,
                                            &is_timeout);
            while (!is_timeout && priv->process_watch_id > 0 && priv->pid > 0)
                g_main_context_iteration(NULL, TRUE);
            g_source_remove(timeout_wait_id);
        }
    }

    if (priv->process_watch_id)
        remove_child_watch_func(priv);

    if (priv->pid) {
        gcut_process_close(GCUT_PROCESS(object));
    }

#ifdef CUT_SUPPORT_GIO
    dispose_streams(priv);
#endif
    dispose_io_channel(priv);

    G_OBJECT_CLASS(gcut_process_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GCutProcessPrivate *priv = GCUT_PROCESS_GET_PRIVATE(object);

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
    GCutProcessPrivate *priv = GCUT_PROCESS_GET_PRIVATE(object);

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
gcut_process_error_quark (void)
{
    return g_quark_from_static_string("gcut-process-error-quark");
}

GCutProcess *
gcut_process_new (const gchar *command, ...)
{
    GCutProcess *process;
    va_list args;

    va_start(args, command);
    process = gcut_process_new_va_list(command, args);
    va_end(args);

    return process;
}

GCutProcess *
gcut_process_new_va_list (const gchar *command, va_list args)
{
    GCutProcess *process;
    GArray *command_line;

    command_line = g_array_new(TRUE, TRUE, sizeof(gchar *));
    while (command) {
        g_array_append_val(command_line, command);
        command = va_arg(args, const gchar *);
    }
    process = gcut_process_new_array(command_line);
    g_array_free(command_line, TRUE);

    return process;
}

GCutProcess *
gcut_process_new_argv (gint args, gchar **argv)
{
    GCutProcess *process;
    gint i;
    GArray *command_line;

    command_line = g_array_new(TRUE, TRUE, sizeof(gchar *));
    for (i = 0; i < args && argv[i]; i++) {
        const gchar *command = argv[i];
        g_array_append_val(command_line, command);
    }
    process = gcut_process_new_array(command_line);
    g_array_free(command_line, TRUE);

    return process;
}

GCutProcess *
gcut_process_new_strings (const gchar **commands)
{
    return g_object_new(GCUT_TYPE_PROCESS,
                        "command", commands,
                        NULL);
}

GCutProcess *
gcut_process_new_array (GArray *command)
{
    return gcut_process_new_strings((const gchar **)(command->data));
}

GSpawnFlags
gcut_process_get_flags (GCutProcess *process)
{
    return GCUT_PROCESS_GET_PRIVATE(process)->flags;
}

void
gcut_process_set_flags (GCutProcess *process, GSpawnFlags flags)
{
    GCUT_PROCESS_GET_PRIVATE(process)->flags = flags;
}

gchar **
gcut_process_get_env (GCutProcess *process)
{
    GCutProcessPrivate *priv;
    GArray *env;
    guint i;

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    if (!priv->env)
        return NULL;

    env = g_array_new(TRUE, TRUE, sizeof(gchar *));
    for (i = 0; i < priv->env->len; i += 2) {
        gchar *pair;

        pair = g_strconcat(g_array_index(priv->env, gchar *, i),
                           "=",
                           g_array_index(priv->env, gchar *, i + 1),
                           NULL);
        g_array_append_val(env, pair);
    }

    return (gchar **)g_array_free(env, FALSE);
}

void
gcut_process_set_env (GCutProcess *process, const gchar *name, ...)
{
    GCutProcessPrivate *priv;
    va_list args;

    if (!name)
        return;

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    if (!priv->env)
        priv->env = g_array_new(TRUE, TRUE, sizeof(gchar *));

    va_start(args, name);
    while (name) {
        const gchar *value;
        guint i;
        gboolean found = FALSE;

        value = va_arg(args, gchar *);
        for (i = 0; i < priv->env->len; i += 2) {
            const gchar *_name;

            _name = g_array_index(priv->env, const gchar *, i);
            if (g_str_equal(name, _name)) {
                if (value) {
                    g_array_index(priv->env, gchar *, i + 1) = g_strdup(value);
                } else {
                    g_array_remove_index(priv->env, i + 1);
                    g_array_remove_index(priv->env, i);
                }
                found = TRUE;
                break;
            }
        }

        if (!found && value) {
            const gchar *duped_string;

            duped_string = g_strdup(name);
            g_array_append_val(priv->env, duped_string);
            duped_string = g_strdup(value);
            g_array_append_val(priv->env, duped_string);
        }
        name = va_arg(args, gchar *);
    }
    va_end(args);
}

static void
reap_child (GCutProcess *process, GPid pid, gint status)
{
    GCutProcessPrivate *priv = GCUT_PROCESS_GET_PRIVATE(process);

    if (priv->pid != pid)
        return;

    remove_child_watch_func(priv);
    priv->status = status;
    g_signal_emit(process, signals[REAPED], 0, status);
    gcut_process_close(process);
}

static void
child_watch_func (GPid pid, gint status, gpointer data)
{
    GCutProcess *process = data;
    GCutProcessPrivate *priv;

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    reap_child(process, pid, status);
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

static void
output_received (GCutProcess *process,
                 const gchar *chunk,
                 gsize        size)
{
    GCutProcessPrivate *priv;

    priv = GCUT_PROCESS_GET_PRIVATE(process);

    g_memory_input_stream_add_data(G_MEMORY_INPUT_STREAM(priv->output_stream),
                                   g_strndup(chunk, size),
                                   size,
                                   g_free);
}

static void
error_received (GCutProcess *process,
                const gchar *chunk,
                gsize        size)
{
    GCutProcessPrivate *priv;

    priv = GCUT_PROCESS_GET_PRIVATE(process);

    g_memory_input_stream_add_data(G_MEMORY_INPUT_STREAM(priv->error_stream),
                                   g_strndup(chunk, size),
                                   size,
                                   g_free);
}

#define BUFFER_SIZE 4096
static gboolean
read_from_io_channel (GIOChannel *channel, GCutProcess *process, guint signal)
{
    GCutProcessPrivate *priv;
    gboolean need_more_data = TRUE;

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    while (need_more_data) {
        GIOStatus status;
        gchar stream[BUFFER_SIZE];
        gsize length = 0;
        GError *error = NULL;

        status = g_io_channel_read_chars(channel, stream, BUFFER_SIZE,
                                         &length, &error);
        if (error) {
            g_signal_emit(process, signals[ERROR], 0, error);
            g_error_free(error);
            need_more_data = FALSE;
            break;
        }

        if (status == G_IO_STATUS_EOF)
            need_more_data = FALSE;

        if (status == G_IO_STATUS_AGAIN ||
            length == 0)
            break;

        g_signal_emit(process, signal, 0, stream, length);
    }

    return need_more_data;
}

static gboolean
watch_output (GIOChannel *source, GIOCondition condition, gpointer user_data)
{
    WatchOutputData *data = user_data;
    gboolean keep_callback = TRUE;

    if (condition & (G_IO_IN | G_IO_PRI)) {
        keep_callback = read_from_io_channel(source, data->process, data->signal);
    }

    if (condition & (G_IO_ERR | G_IO_NVAL)) {
        gchar *message;
        GError *error;

        message = gcut_io_inspect_condition(condition);
        error = g_error_new(GCUT_PROCESS_ERROR, GCUT_PROCESS_ERROR_IO_ERROR,
                            "%s", message);
        g_signal_emit(data->process, signals[ERROR], 0, error);
        g_free(message);
        g_error_free(error);
        keep_callback = FALSE;
    }

    if (condition & G_IO_HUP)
        keep_callback = FALSE;

    return keep_callback;
}

gboolean
gcut_process_run (GCutProcess *process, GError **error)
{
    GCutProcessPrivate *priv;
    gboolean success;
    gint input_fd = 0, output_fd = 0, error_fd = 0;
    gchar **env;

    priv = GCUT_PROCESS_GET_PRIVATE(process);

    if (!priv->command) {
        g_set_error(error, GCUT_PROCESS_ERROR, GCUT_PROCESS_ERROR_COMMAND_LINE,
                    "command line isn't set");
        return FALSE;
    }

    if (priv->pid > 0) {
        gchar *command;

        command = g_strjoinv(" ", priv->command);
        g_set_error(error, GCUT_PROCESS_ERROR, GCUT_PROCESS_ERROR_ALREADY_RUNNING,
                    "already running: %s", command);
        g_free(command);

        return FALSE;
    }

    dispose_io_channel(priv);

    env = gcut_process_get_env(process);
    success = g_spawn_async_with_pipes(NULL,
                                       priv->command,
                                       env,
                                       priv->flags | priv->must_flags,
                                       NULL,
                                       NULL,
                                       &priv->pid,
                                       &input_fd,
                                       &output_fd,
                                       &error_fd,
                                       error);
    if (env)
        g_strfreev(env);

    if (!success)
        return FALSE;

    priv->process_watch_id = g_child_watch_add(priv->pid,
                                               child_watch_func,
                                               process);

    if (input_fd > 0)
        priv->input = create_output_channel(input_fd);

    if (output_fd > 0) {
        priv->watch_output_data = g_new(WatchOutputData, 1);
        priv->watch_output_data->process = process;
        priv->watch_output_data->signal = signals[OUTPUT_RECEIVED];
        priv->output = create_input_channel(output_fd,
                                            &(priv->output_watch_id),
                                            watch_output,
                                            priv->watch_output_data);
    }
    if (error_fd > 0) {
        priv->watch_error_data = g_new(WatchOutputData, 1);
        priv->watch_error_data->process = process;
        priv->watch_error_data->signal = signals[ERROR_RECEIVED];
        priv->error = create_input_channel(error_fd,
                                           &(priv->error_watch_id),
                                           watch_output,
                                           priv->watch_error_data);
    }

    return TRUE;
}

gboolean
gcut_process_write (GCutProcess *process, const gchar *chunk, gsize size,
                    GError **error)
{
    GCutProcessPrivate *priv;
    gsize rest_size = size;

    priv = GCUT_PROCESS_GET_PRIVATE(process);
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
gcut_process_get_pid (GCutProcess *process)
{
    return GCUT_PROCESS_GET_PRIVATE(process)->pid;
}

gint
gcut_process_wait (GCutProcess *process, guint timeout, GError **error)
{
    GCutProcessPrivate *priv;
    gboolean is_timeout = FALSE;
    guint timeout_id;

    priv = GCUT_PROCESS_GET_PRIVATE(process);

    if (!priv) {
        g_set_error(error,
                    GCUT_PROCESS_ERROR,
                    GCUT_PROCESS_ERROR_INVALID_OBJECT,
                    "passed GCutProcess is invalid");
        return -1;
    }

    if (priv->pid == 0) {
        gchar *command = NULL;

        if (priv->command)
            command = g_strjoinv(" ", priv->command);
        if (command) {
            g_set_error(error,
                        GCUT_PROCESS_ERROR,
                        GCUT_PROCESS_ERROR_NOT_RUNNING,
                        "not running: <%s>", command);
            g_free(command);
        } else {
            g_set_error(error,
                        GCUT_PROCESS_ERROR,
                        GCUT_PROCESS_ERROR_NOT_RUNNING,
                        "not running");
        }
        return -1;
    }

    timeout_id = g_timeout_add(timeout, cb_timeout_wait, &is_timeout);
    while (!is_timeout && priv->pid > 0)
        g_main_context_iteration(NULL, TRUE);
    g_source_remove(timeout_id);

    if (is_timeout) {
        gchar *command;

        command = g_strjoinv(" ", priv->command);
        g_set_error(error,
                    GCUT_PROCESS_ERROR,
                    GCUT_PROCESS_ERROR_TIMEOUT,
                    "timeout while waiting reaped: <%s>", command);
        g_free(command);
        return -1;
    }

    return priv->status;
}

static GCutProcessError
gcut_process_error_from_errno (gint err_no)
{
    switch (err_no) {
    case EINVAL:
        return GCUT_PROCESS_ERROR_INVALID_SIGNAL;
        break;
    case EPERM:
        return GCUT_PROCESS_ERROR_PERMISSION_DENIED;
        break;
    case ESRCH:
        return GCUT_PROCESS_ERROR_INVALID_OBJECT;
        break;
    default:
        return GCUT_PROCESS_ERROR_INVALID_OBJECT;
        break;
    }
}

gboolean
gcut_process_kill (GCutProcess *process, int signal_number, GError **error)
{
    GCutProcessPrivate *priv;

    priv = GCUT_PROCESS_GET_PRIVATE(process);

    if (!priv) {
        g_set_error(error,
                    GCUT_PROCESS_ERROR,
                    GCUT_PROCESS_ERROR_INVALID_OBJECT,
                    "Invalid process object.");
        return FALSE;
    }

    if (priv->pid == 0) {
        g_set_error(error,
                    GCUT_PROCESS_ERROR,
                    GCUT_PROCESS_ERROR_NOT_RUNNING,
                    "The process is not running.");
        return FALSE;
    }

#ifndef G_OS_WIN32
    if (kill(priv->pid, signal_number) == -1) {
        g_set_error(error,
                    GCUT_PROCESS_ERROR,
                    gcut_process_error_from_errno(errno),
                    "Error killing process: ");
        return FALSE;
    };
    return TRUE;
#else
    return cut_win32_kill_process(priv->pid, 0);
#endif
}

GIOChannel *
gcut_process_get_input_channel (GCutProcess *process)
{
    return GCUT_PROCESS_GET_PRIVATE(process)->input;
}

GIOChannel *
gcut_process_get_output_channel (GCutProcess *process)
{
    return GCUT_PROCESS_GET_PRIVATE(process)->output;
}

GIOChannel *
gcut_process_get_error_channel (GCutProcess *process)
{
    return GCUT_PROCESS_GET_PRIVATE(process)->error;
}

guint
gcut_process_get_forced_termination_wait_time (GCutProcess *process)
{
    return GCUT_PROCESS_GET_PRIVATE(process)->forced_termination_wait_time;
}

void
gcut_process_set_forced_termination_wait_time (GCutProcess *process, guint timeout)
{
    GCUT_PROCESS_GET_PRIVATE(process)->forced_termination_wait_time = timeout;
}

#ifdef CUT_SUPPORT_GIO
GOutputStream *
gcut_process_get_input_stream (GCutProcess *process)
{
    GCutProcessPrivate *priv;

    g_return_val_if_fail(GCUT_IS_PROCESS(process), NULL);

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    g_return_val_if_fail(priv, NULL);

    return priv->input_stream;
}

GInputStream *
gcut_process_get_output_stream (GCutProcess *process)
{
    GCutProcessPrivate *priv;

    g_return_val_if_fail(GCUT_IS_PROCESS(process), NULL);

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    g_return_val_if_fail(priv, NULL);

    return priv->output_stream;
}

GInputStream *
gcut_process_get_error_stream (GCutProcess *process)
{
    GCutProcessPrivate *priv;

    g_return_val_if_fail(GCUT_IS_PROCESS(process), NULL);

    priv = GCUT_PROCESS_GET_PRIVATE(process);
    g_return_val_if_fail(priv, NULL);

    return priv->error_stream;
}
#endif

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
