/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#include "cut-pipeline.h"
#include "cut-test-result.h"
#include "cut-runner.h"
#include "cut-experimental.h"
#include "cut-utils.h"

#ifdef G_OS_WIN32
#  include <io.h>
#  define pipe(phandles) _pipe(phandles, 4096, _O_BINARY)
#else
#  include <unistd.h>
#endif

#define CUT_PIPELINE_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_PIPELINE, CutPipelinePrivate))

typedef struct _CutPipelinePrivate	CutPipelinePrivate;
struct _CutPipelinePrivate
{
    GPid pid;
    guint process_source_id;
    guint child_out_source_id;
    GIOChannel *child_out;
    gint child_pipe[2];
};

static CutRunnerIface *parent_runner_iface;

static void runner_init (CutRunnerIface *iface);

G_DEFINE_TYPE_WITH_CODE(CutPipeline, cut_pipeline, CUT_TYPE_STREAM_READER,
                        G_IMPLEMENT_INTERFACE(CUT_TYPE_RUNNER, runner_init))

static void     dispose      (GObject         *object);

static void     runner_run_async (CutRunner *runner);

static void
cut_pipeline_class_init (CutPipelineClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutPipelinePrivate));
}

static void
cut_pipeline_init (CutPipeline *pipeline)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    priv->process_source_id = 0;
    priv->child_out_source_id = 0;
    priv->pid               = 0;

    priv->child_out         = NULL;
    priv->child_pipe[0] = -1;
    priv->child_pipe[1] = -1;
}

static void
runner_init (CutRunnerIface *iface)
{
    parent_runner_iface = g_type_interface_peek_parent(iface);
    iface->run_async = runner_run_async;
    iface->run       = NULL;
}

static void
remove_child_watch_func (CutPipelinePrivate *priv)
{
    g_source_remove(priv->process_source_id);
    priv->process_source_id = 0;
}

static void
remove_child_out_watch_func (CutPipelinePrivate *priv)
{
    g_source_remove(priv->child_out_source_id);
    priv->child_out_source_id = 0;
}

static void
close_child (CutPipelinePrivate *priv)
{
    g_spawn_close_pid(priv->pid);
    priv->pid = 0;
}

static void
unref_child_out_channel (CutPipelinePrivate *priv)
{
    g_io_channel_unref(priv->child_out);
    priv->child_out = NULL;
}

static void
dispose (GObject *object)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(object);

    if (priv->process_source_id)
        remove_child_watch_func(priv);

    if (priv->child_out_source_id)
        remove_child_out_watch_func(priv);

    if (priv->pid)
        close_child(priv);

    if (priv->child_out)
        unref_child_out_channel(priv);

    G_OBJECT_CLASS(cut_pipeline_parent_class)->dispose(object);
}

CutRunContext *
cut_pipeline_new (void)
{
    return g_object_new(CUT_TYPE_PIPELINE, NULL);
}

CutRunContext *
cut_pipeline_new_from_run_context (CutRunContext *run_context)
{
    return g_object_new(CUT_TYPE_PIPELINE,
                        "test-directory",
                        cut_run_context_get_test_directory(run_context),
                        "use-multi-thread",
                        cut_run_context_get_multi_thread(run_context),
                        "max-threads",
                        cut_run_context_get_max_threads(run_context),
                        "handle-signals",
                        cut_run_context_get_handle_signals(run_context),
                        "exclude-files",
                        cut_run_context_get_exclude_files(run_context),
                        "exclude-directories",
                        cut_run_context_get_exclude_directories(run_context),
                        "target-test-case-names",
                        cut_run_context_get_target_test_case_names(run_context),
                        "target-test-names",
                        cut_run_context_get_target_test_names(run_context),
                        "test-case-order",
                        cut_run_context_get_test_case_order(run_context),
                        "source-directory",
                        cut_run_context_get_source_directory(run_context),
                        "command-line-args",
                        cut_run_context_get_command_line_args(run_context),
                        "fatal-failures",
                        cut_run_context_get_fatal_failures(run_context),
                        "keep-opening-modules",
                        cut_run_context_get_keep_opening_modules(run_context),
                        "enable-convenience-attribute-definition",
                        cut_run_context_get_enable_convenience_attribute_definition(run_context),
                        NULL);
}

GQuark
cut_pipeline_error_quark (void)
{
    return g_quark_from_static_string("cut-pipeline-error-quark");
}

#define emit_error(pipeline, code, error, ...) do                       \
{                                                                       \
    CutPipeline *_pipeline;                                             \
    CutPipelinePrivate *_priv;                                          \
    CutRunContext *_run_context;                                        \
                                                                        \
    _pipeline = (pipeline);                                             \
    _priv = CUT_PIPELINE_GET_PRIVATE(_pipeline);                        \
    _run_context = CUT_RUN_CONTEXT(_pipeline);                          \
    cut_run_context_emit_error(_run_context, CUT_PIPELINE_ERROR,        \
                               code, error,                             \
                               __VA_ARGS__);                            \
    cut_run_context_emit_complete_run(_run_context, FALSE);             \
} while (0)

static void
reap_child (CutPipeline *pipeline, GPid pid)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    if (priv->pid != pid)
        return;

    remove_child_watch_func(priv);
    remove_child_out_watch_func(priv);
    unref_child_out_channel(priv);
    close_child(priv);
}

static gboolean
read_stream (CutPipeline *pipeline, GIOChannel *channel)
{
    return cut_stream_reader_read_from_io_channel(CUT_STREAM_READER(pipeline),
                                                  channel);
}

static void
child_watch_func (GPid pid, gint status, gpointer data)
{
    CutPipeline *pipeline = data;
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);
    read_stream(pipeline, priv->child_out);
    reap_child(pipeline, pid);
}

static GIOChannel *
create_child_out_channel (CutPipeline *pipeline)
{
    GIOChannel *channel;
    CutStreamReader *reader;
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);
    if (priv->child_pipe[CUT_READ] == -1)
        return NULL;

#ifndef G_OS_WIN32
    channel = g_io_channel_unix_new(priv->child_pipe[CUT_READ]);
#else
    channel = g_io_channel_win32_new_fd(priv->child_pipe[CUT_READ]);
#endif
    if (!channel)
        return NULL;

    g_io_channel_set_close_on_unref(channel, TRUE);

    reader = CUT_STREAM_READER(pipeline);
    /* FIXME: should connect "error" or "complete-run"
     * signal and cleanup resource. */
    priv->child_out_source_id = cut_stream_reader_watch_io_channel(reader,
                                                                   channel);

    return channel;
}

static gchar **
create_command_line_args_from_argv (CutPipeline *pipeline, const gchar **argv)
{
    CutPipelinePrivate *priv;
    CutRunContext *run_context;
    gchar **new_argv;
    gchar **copy;
    const gchar *test_directory;
    gchar *stream_fd;
    guint i;
    guint length;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);
    run_context = CUT_RUN_CONTEXT(pipeline);

    length = g_strv_length((gchar **)argv);
    /* remove the last argument in which test directory is stored */
    copy = g_new(gchar *, length);
    for (i = 0; i < length - 1; i++) {
        copy[i] = g_strdup(argv[i]);
    }
    copy[i] = NULL;

    stream_fd = g_strdup_printf("--stream-fd=%d",
                                priv->child_pipe[CUT_WRITE]);
    test_directory = cut_run_context_get_test_directory(run_context);
    new_argv = cut_utils_strv_concat((const gchar **)copy,
                                     "--ui=console",
                                     "-v", "s",
                                     "--stream=xml",
                                     stream_fd,
                                     test_directory,
                                     NULL);
    g_free(stream_fd);
    g_strfreev(copy);

    return new_argv;
}

static void
append_arg (GArray *argv, const gchar *arg)
{
    gchar *dupped_arg;

    dupped_arg = g_strdup(arg);
    g_array_append_val(argv, dupped_arg);
}

static void
append_arg_printf (GArray *argv, const gchar *format, ...)
{
    gchar *arg;
    va_list args;

    va_start(args, format);
    arg = g_strdup_vprintf(format, args);
    va_end(args);

    g_array_append_val(argv, arg);
}

static gchar **
create_command_line_args_from_parameters (CutPipeline *pipeline)
{
    CutPipelinePrivate *priv;
    CutRunContext *run_context;
    GArray *argv;
    const gchar *directory;
    const gchar **strings;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);
    run_context = CUT_RUN_CONTEXT(pipeline);

    argv = g_array_new(TRUE, TRUE, sizeof(gchar *));

    append_arg(argv, cut_utils_get_cutter_command_path());
    append_arg(argv, "--verbose=silent");
    append_arg(argv, "--stream=xml");
    append_arg_printf(argv, "--stream-fd=%d", priv->child_pipe[CUT_WRITE]);

    directory = cut_run_context_get_source_directory(run_context);
    if (directory)
        append_arg_printf(argv, "--source-directory=%s", directory);

    if (cut_run_context_get_multi_thread(run_context))
        append_arg(argv, "--multi-thread");

    append_arg_printf(argv,
                      "--max-threads=%d",
                      cut_run_context_get_max_threads(run_context));

    strings = cut_run_context_get_exclude_files(run_context);
    while (strings && *strings) {
        append_arg_printf(argv, "--exclude-file=%s", *strings);
        strings++;
    }

    strings = cut_run_context_get_exclude_directories(run_context);
    while (strings && *strings) {
        append_arg_printf(argv, "--exclude-directory=%s", *strings);
        strings++;
    }

    strings = cut_run_context_get_target_test_case_names(run_context);
    while (strings && *strings) {
        append_arg_printf(argv, "--test-case=%s", *strings);
        strings++;
    }

    strings = cut_run_context_get_target_test_names(run_context);
    while (strings && *strings) {
        append_arg_printf(argv, "--name=%s", *strings);
        strings++;
    }

    if (cut_run_context_get_fatal_failures(run_context))
        append_arg(argv, "--fatal-failures");

    append_arg(argv, cut_run_context_get_test_directory(run_context));

    return (gchar **)(g_array_free(argv, FALSE));
}

static gchar **
create_command_line_args (CutPipeline *pipeline)
{
    CutRunContext *run_context;
    const gchar **original_argv;
    gchar **new_args = NULL;

    run_context = CUT_RUN_CONTEXT(pipeline);
    g_return_val_if_fail(cut_run_context_get_test_directory(run_context) != NULL,
                         NULL);

    original_argv = cut_run_context_get_command_line_args(run_context);

    if (original_argv) {
        new_args = create_command_line_args_from_argv(pipeline, original_argv);
    } else {
        new_args = create_command_line_args_from_parameters(pipeline);
    }

    return new_args;
}

#ifndef G_OS_WIN32
static void
setup_child (gpointer user_data)
{
    CutPipeline *pipeline = user_data;
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    cut_utils_close_pipe(priv->child_pipe, CUT_READ);
}
#endif

static void
run_async (CutPipeline *pipeline)
{
    gchar **command_line_args;
    gboolean result;
    GError *error = NULL;
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    if (pipe(priv->child_pipe) < 0) {
        emit_error(pipeline, CUT_PIPELINE_ERROR_PIPE, NULL,
                   "failed to create pipe: %s", g_strerror(errno));
        return;
    }

    command_line_args = create_command_line_args(pipeline);
    if (!command_line_args) {
        emit_error(pipeline, CUT_PIPELINE_ERROR_COMMAND_LINE, NULL,
                   "failed to generate command line");
        return;
    }

    result = g_spawn_async_with_pipes(NULL,
                                      command_line_args,
                                      NULL,
                                      G_SPAWN_SEARCH_PATH |
                                      G_SPAWN_LEAVE_DESCRIPTORS_OPEN |
                                      G_SPAWN_DO_NOT_REAP_CHILD,
#ifndef G_OS_WIN32
                                      setup_child,
#else
                                      NULL,
#endif
                                      pipeline,
                                      &priv->pid,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &error);
    g_strfreev(command_line_args);
    cut_utils_close_pipe(priv->child_pipe, CUT_WRITE);

    if (!result) {
        emit_error(pipeline, CUT_PIPELINE_ERROR_SPAWN, error,
                   "failed to spawn child process");
        return;
    }

    if (priv->pid == (GPid)0) {
        emit_error(pipeline, CUT_PIPELINE_ERROR_CHILD_PID,
                   error, "failed to get child PID");
        return;
    }

    priv->child_out = create_child_out_channel(pipeline);
    if (!priv->child_out) {
        emit_error(pipeline, CUT_PIPELINE_ERROR_PIPE,
                   error, "failed to connect to child pipe");
        return;
    }

    priv->process_source_id = g_child_watch_add(priv->pid,
                                                child_watch_func,
                                                pipeline);
}

static void
runner_run_async (CutRunner *runner)
{
    CutPipeline *pipeline;

    pipeline = CUT_PIPELINE(runner);
    run_async(pipeline);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
