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
#include <wait.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "cut-pipeline.h"
#include "cut-test-result.h"
#include "cut-runner.h"
#include "cut-experimental.h"

#define CUT_PIPELINE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_PIPELINE, CutPipelinePrivate))

typedef struct _CutPipelinePrivate	CutPipelinePrivate;
struct _CutPipelinePrivate
{
    GPid pid;
    guint process_source_id;
    guint io_source_id;
    gchar *target_directory;
    GIOChannel *stdout_io;
    GString *cutter_string;
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
static void     set_property (GObject         *object,
                              guint            prop_id,
                              const GValue    *value,
                              GParamSpec      *pspec);
static void     get_property (GObject         *object,
                              guint            prop_id,
                              GValue          *value,
                              GParamSpec      *pspec);

static gboolean runner_run       (CutRunner *runner);
static void     runner_run_async (CutRunner *runner);

static void
cut_pipeline_class_init (CutPipelineClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("target-directory",
                               "Test target directory name",
                               "Test target directory name",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TARGET_DIRECTORY, spec);

    g_type_class_add_private(gobject_class, sizeof(CutPipelinePrivate));
}

static void
cut_pipeline_init (CutPipeline *pipeline)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    priv->process_source_id = 0;
    priv->io_source_id      = 0;
    priv->pid               = 0;

    priv->target_directory = NULL;
    priv->stdout_io        = NULL;

    priv->cutter_string = g_string_new(NULL);
}

static void
runner_init (CutRunnerIface *iface)
{
    parent_runner_iface = g_type_interface_peek_parent(iface);
    iface->run = runner_run;
    iface->run_async = runner_run_async;
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_TARGET_DIRECTORY:
        priv->target_directory = g_value_dup_string(value);
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
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_TARGET_DIRECTORY:
        g_value_set_string(value, priv->target_directory);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
remove_child_watch_func (CutPipelinePrivate *priv)
{
    g_source_remove(priv->process_source_id);
    priv->process_source_id = 0;
}

static void
remove_io_watch_func (CutPipelinePrivate *priv)
{
    g_source_remove(priv->io_source_id);
    priv->io_source_id = 0;
}

static void
close_child (CutPipelinePrivate *priv)
{
    g_spawn_close_pid(priv->pid);
    priv->pid = 0;
}

static void
shutdown_io_channel (CutPipelinePrivate *priv)
{
    g_io_channel_shutdown(priv->stdout_io, TRUE, NULL);
    priv->stdout_io = NULL;
}

static void
dispose (GObject *object)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(object);

    if (priv->process_source_id)
        remove_child_watch_func(priv);

    if (priv->io_source_id)
        remove_io_watch_func(priv);

    if (priv->pid)
        close_child(priv);

    if (priv->stdout_io)
        shutdown_io_channel(priv);

    if (priv->target_directory) {
        g_free(priv->target_directory);
        priv->target_directory = NULL;
    }

    if (priv->cutter_string) {
        g_string_free(priv->cutter_string, TRUE);
        priv->cutter_string = NULL;
    }

    G_OBJECT_CLASS(cut_pipeline_parent_class)->dispose(object);
}

CutRunContext *
cut_pipeline_new (const gchar *target_directory)
{
    return g_object_new(CUT_TYPE_PIPELINE,
                        "target-directory", target_directory,
                        NULL);
}

static void
reap_child (CutPipeline *pipeline, GPid pid)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    if (priv->pid != pid)
        return;

    remove_child_watch_func(priv);
    shutdown_io_channel(priv);
    close_child(priv);
}

static void
emit_complete_signal (CutPipeline *pipeline, gboolean success)
{
    g_signal_emit_by_name(pipeline, "complete-run", success);
}

static void
child_watch_func (GPid pid, gint status, gpointer data)
{
    if (WIFEXITED(status)) {
        emit_complete_signal(CUT_PIPELINE(data), WEXITSTATUS(status) ? FALSE : TRUE);
        reap_child(CUT_PIPELINE(data), pid);
    }
}

static GIOStatus
read_line (CutPipeline *pipeline, GIOChannel *channel)
{
    GIOStatus status;
    gchar *line_string = NULL;
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    status = g_io_channel_read_line(channel,
                                    &line_string,
                                    NULL,
                                    NULL,
                                    NULL);
    if (status == G_IO_STATUS_NORMAL ||
        status == G_IO_STATUS_EOF) {
        g_string_append(priv->cutter_string, line_string);
        if (g_regex_match_simple("</result>", line_string, 0, 0)) {
            CutTestResult *result;

            result = cut_test_result_new_from_xml(priv->cutter_string->str,
                                                  priv->cutter_string->len);
            if (result) {
                /* cut_test_context_emit_signal(context, result); */
                g_object_unref(result);
            }
            g_string_truncate(priv->cutter_string, 0);
        }
    }

    if (line_string)
        g_free(line_string);

    return status;
}

static gboolean
io_watch_func (GIOChannel *channel, GIOCondition condition, gpointer data)
{
    CutPipeline *pipeline;

    if (!CUT_IS_PIPELINE(data))
        return FALSE;

    pipeline = CUT_PIPELINE(data);

    if (condition & G_IO_IN ||
        condition & G_IO_PRI) {
        read_line(pipeline, channel);
    }

    if (condition & G_IO_ERR ||
        condition & G_IO_HUP) {
        emit_complete_signal(CUT_PIPELINE(data), FALSE);
        return FALSE;
    }

    return TRUE;
}

static GIOChannel *
create_io_channel (CutPipeline *pipeline, gint pipe)
{
    GIOChannel *channel;
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    channel = g_io_channel_unix_new(pipe);
    g_io_channel_set_close_on_unref(channel, TRUE);
    priv->io_source_id = g_io_add_watch(channel,
                                        G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP,
                                        io_watch_func, pipeline);

    return channel;
}

static void
run_async (CutPipeline *pipeline)
{
    gchar *command_line;
    const gchar *cutter_command;
    gchar **argv = NULL;
    gint argc;
    gint std_out;
    gboolean ret;
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    cutter_command = g_getenv("CUTTER");
    if (!cutter_command)
        cutter_command = g_get_prgname();

    command_line = g_strdup_printf("%s -v v --streamer=xml %s",
                                   cutter_command,
                                   priv->target_directory);

    ret = g_shell_parse_argv(command_line, &argc, &argv, NULL);
    g_free(command_line);
    if (!ret) {
        emit_complete_signal(pipeline, FALSE);
        return;
    }

    ret = g_spawn_async_with_pipes(NULL,
                                   argv,
                                   NULL,
                                   G_SPAWN_DO_NOT_REAP_CHILD,
                                   NULL,
                                   NULL,
                                   &priv->pid,
                                   NULL,
                                   &std_out,
                                   NULL,
                                   NULL);
    g_strfreev(argv);
    if (!ret) {
        emit_complete_signal(pipeline, FALSE);
        return;
    }

    priv->process_source_id = g_child_watch_add(priv->pid, child_watch_func, pipeline);
    priv->stdout_io = create_io_channel(pipeline, std_out);
}

static gboolean
runner_run (CutRunner *runner)
{
    return FALSE;
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
