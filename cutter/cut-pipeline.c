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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "cut-pipeline.h"
#include "cut-test-result.h"
#include "cut-runner.h"
#include "cut-stream-parser.h"
#include "cut-experimental.h"
#include "cut-utils.h"

#define CUT_PIPELINE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_PIPELINE, CutPipelinePrivate))

typedef struct _CutPipelinePrivate	CutPipelinePrivate;
struct _CutPipelinePrivate
{
    GPid pid;
    guint process_source_id;
    guint child_out_source_id;
    GIOChannel *child_out;
    gint child_pipe[2];
    CutStreamParser *parser;
    gboolean error_emitted;
    gboolean eof;
};

enum
{
    PROP_0,
    PROP_TARGET_DIRECTORY
};

static CutRunnerIface *parent_runner_iface;

static void runner_init (CutRunnerIface *iface);

G_DEFINE_TYPE_WITH_CODE(CutPipeline, cut_pipeline, CUT_TYPE_RUN_CONTEXT,
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

    priv->parser = NULL;
    priv->error_emitted = FALSE;
    priv->eof = FALSE;
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
shutdown_child_out_channel (CutPipelinePrivate *priv)
{
    g_io_channel_shutdown(priv->child_out, TRUE, NULL);
    priv->child_out = NULL;
}

static void
dispose_parser (CutPipelinePrivate *priv)
{
    if (priv->parser) {
        CutStreamParser *parser;

        parser = priv->parser;
        priv->parser = NULL;
        g_object_unref(parser);
    }
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
        shutdown_child_out_channel(priv);

    dispose_parser(priv);

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
                        "test-directory", cut_run_context_get_test_directory(run_context),
                        "use-multi-thread", cut_run_context_get_multi_thread(run_context),
                        "exclude-files", cut_run_context_get_exclude_files(run_context),
                        "exclude-directories", cut_run_context_get_exclude_directories(run_context),
                        "target-test-case-names", cut_run_context_get_target_test_case_names(run_context),
                        "target-test-names", cut_run_context_get_target_test_names(run_context),
                        "test-case-order", cut_run_context_get_test_case_order(run_context),
                        "source-directory", cut_run_context_get_source_directory(run_context),
                        "command-line-args", cut_run_context_get_command_line_args(run_context),
                        NULL);
}

#define emit_error(pipeline, error, format, ...) do                     \
{                                                                       \
    CutPipelinePrivate *_priv;                                          \
    CutRunContext *_run_context;                                        \
                                                                        \
    _priv = CUT_PIPELINE_GET_PRIVATE(pipeline);                         \
    _run_context = CUT_RUN_CONTEXT(pipeline);                           \
    cut_run_context_emit_error(_run_context, "StreamError", error,      \
                               format, ## __VA_ARGS__);                 \
    _priv->error_emitted = TRUE;                                        \
    emit_complete_signal(pipeline, FALSE);                              \
} while (0)

static void
reap_child (CutPipeline *pipeline, GPid pid)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    if (priv->pid != pid)
        return;

    remove_child_watch_func(priv);
    remove_child_out_watch_func(priv);
    shutdown_child_out_channel(priv);
    close_child(priv);
    dispose_parser(priv);
}

static void
emit_complete_signal (CutPipeline *pipeline, gboolean success)
{
    g_signal_emit_by_name(pipeline, "complete-run", success);
}

#define BUFFER_SIZE 512
static void
read_stream (CutPipeline *pipeline, GIOChannel *channel)
{
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    while (!priv->error_emitted && !priv->eof) {
        GIOStatus status;
        gchar stream[BUFFER_SIZE + 1];
        gsize length = 0;
        GError *error = NULL;

        status = g_io_channel_read_chars(channel, stream, BUFFER_SIZE, &length,
                                         &error);
        if (status == G_IO_STATUS_EOF)
            priv->eof = TRUE;

        if (error) {
            emit_error(pipeline, error, "failed to read stream");
            break;
        }

        if (length <= 0)
            break;

        cut_stream_parser_parse(priv->parser, stream, length, &error);
        if (error) {
            emit_error(pipeline, error, "failed to parse stream");
            break;
        }

        if (priv->eof) {
            cut_stream_parser_end_parse(priv->parser, &error);
            if (error) {
                emit_error(pipeline, error, "failed to end parse stream");
                break;
            }
        } else {
            g_main_context_iteration(NULL, FALSE);
        }
    }
}

static gboolean
child_out_watch_func (GIOChannel *channel, GIOCondition condition, gpointer data)
{
    CutPipeline *pipeline;

    if (!CUT_IS_PIPELINE(data))
        return FALSE;

    pipeline = CUT_PIPELINE(data);

    if (condition & G_IO_IN ||
        condition & G_IO_PRI) {
        read_stream(pipeline, channel);
    }

    if (condition & G_IO_ERR ||
        condition & G_IO_HUP) {
        return FALSE;
    }

    return TRUE;
}

static void
child_watch_func (GPid pid, gint status, gpointer data)
{
    if (WIFEXITED(status)) {
        CutPipeline *pipeline = data;
        CutPipelinePrivate *priv;

        priv = CUT_PIPELINE_GET_PRIVATE(pipeline);
        read_stream(pipeline, priv->child_out);
        if (!priv->error_emitted)
            emit_complete_signal(pipeline, WEXITSTATUS(status) == EXIT_SUCCESS);
        reap_child(pipeline, pid);
    }
}

static GIOChannel *
create_child_out_channel (CutPipeline *pipeline)
{
    GIOChannel *channel;
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);
    channel = g_io_channel_unix_new(priv->child_pipe[CUT_READ]);
    g_io_channel_set_close_on_unref(channel, TRUE);
    priv->child_out_source_id = g_io_add_watch(channel,
                                               G_IO_IN | G_IO_PRI |
                                               G_IO_ERR | G_IO_HUP,
                                               child_out_watch_func, pipeline);

    return channel;
}

static gchar **
create_command_line_args (CutPipeline *pipeline)
{
    const gchar **original_argv;
    gchar **new_args = NULL;
    const gchar *test_directory;
    CutRunContext *run_context = CUT_RUN_CONTEXT(pipeline);
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);
    test_directory = cut_run_context_get_test_directory(run_context);
    original_argv = cut_run_context_get_command_line_args(run_context);

    if (original_argv) {
        gchar **copy;
        gchar *stream_fd;
        guint i;
        guint length = g_strv_length((gchar **)original_argv);

        /* remove the last argumen in which test directory is stored */
        copy = g_new(gchar *, length);
        for (i = 0; i < length - 1; i++) {
            copy[i] = g_strdup(original_argv[i]);
        }
        copy[i] = NULL;

        stream_fd = g_strdup_printf("--stream-fd=%d",
                                    priv->child_pipe[CUT_WRITE]);
        new_args = cut_utils_strv_concat((const gchar **)copy,
                                         "--ui=console",
                                         "-v", "s",
                                         "--streamer=xml",
                                         stream_fd,
                                         test_directory,
                                         NULL);
        g_free(stream_fd);
        g_strfreev(copy);
    } else {
        gboolean result;
        gint argc;
        gchar *command_line;
        const gchar *cutter_command = NULL;

        cutter_command = g_getenv("CUTTER");

        if (!cutter_command)
            cutter_command = g_get_prgname();

        command_line = g_strdup_printf("%s "
                                       "-v s "
                                       "--streamer=xml "
                                       "--stream-fd=%d "
                                       "%s",
                                       cutter_command,
                                       priv->child_pipe[CUT_WRITE],
                                       test_directory);
        result = g_shell_parse_argv(command_line, &argc, &new_args, NULL);
        g_free(command_line);
    }

    return new_args;
}

static void
setup_child (gpointer user_data)
{
    CutPipeline *pipeline = user_data;
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    cut_utils_close_pipe(priv->child_pipe, CUT_READ);
}

static void
run_async (CutPipeline *pipeline)
{
    gchar **command_line_args;
    gboolean result;
    GError *error = NULL;
    CutPipelinePrivate *priv;

    priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    if (pipe(priv->child_pipe) < 0) {
        emit_error(pipeline, NULL, "can't create pipe: %s", strerror(errno));
        return;
    }

    command_line_args = create_command_line_args(pipeline);
    if (!command_line_args) {
        emit_error(pipeline, NULL, "failed to generate command line");
        return;
    }

    result = g_spawn_async_with_pipes(NULL,
                                      command_line_args,
                                      NULL,
                                      G_SPAWN_LEAVE_DESCRIPTORS_OPEN |
                                      G_SPAWN_DO_NOT_REAP_CHILD,
                                      setup_child,
                                      pipeline,
                                      &priv->pid,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &error);
    g_strfreev(command_line_args);
    cut_utils_close_pipe(priv->child_pipe, CUT_WRITE);

    if (!result) {
        emit_error(pipeline, error, "failed to spawn child process");
        return;
    }

    priv->error_emitted = FALSE;
    priv->eof = FALSE;
    priv->parser = cut_stream_parser_new(CUT_RUN_CONTEXT(pipeline));
    priv->child_out = create_child_out_channel(pipeline);
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
