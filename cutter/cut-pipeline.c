/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "cut-pipeline.h"
#include "cut-experimental.h"

#define CUT_PIPELINE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_PIPELINE, CutPipelinePrivate))

typedef struct _CutPipelinePrivate	CutPipelinePrivate;
struct _CutPipelinePrivate
{
    GPid pid;
    guint source_id;
    gchar *target_directory;
    GIOChannel *stdout_io;
};

enum
{
    PROP_0,
    PROP_TARGET_DIRECTORY
};

G_DEFINE_TYPE (CutPipeline, cut_pipeline, G_TYPE_OBJECT)

static GObject *constructor  (GType                  type,
                              guint                  n_props,
                              GObjectConstructParam *props);
static void     dispose      (GObject         *object);
static void     set_property (GObject         *object,
                              guint            prop_id,
                              const GValue    *value,
                              GParamSpec      *pspec);
static void     get_property (GObject         *object,
                              guint            prop_id,
                              GValue          *value,
                              GParamSpec      *pspec);

static gboolean cut_pipeline_spawn (CutPipeline *pipeline);

static void
cut_pipeline_class_init (CutPipelineClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->constructor  = constructor;
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

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;
    GObjectClass *klass = G_OBJECT_CLASS(cut_pipeline_parent_class);

    object = klass->constructor(type, n_props, props);

    cut_pipeline_spawn(CUT_PIPELINE(object));

    return object;
}

static void
cut_pipeline_init (CutPipeline *pipeline)
{
    CutPipelinePrivate *priv = CUT_PIPELINE_GET_PRIVATE(pipeline);

    priv->source_id     = 0;
    priv->pid           = 0;

    priv->target_directory = NULL;
    priv->stdout_io        = NULL;
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
    g_source_remove(priv->source_id);
    priv->source_id = 0;
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

    if (priv->source_id)
        remove_child_watch_func(priv);

    if (priv->pid)
        close_child(priv);

    if (priv->stdout_io)
        shutdown_io_channel(priv);

    if (priv->target_directory) {
        g_free(priv->target_directory);
        priv->target_directory = NULL;
    }

    G_OBJECT_CLASS(cut_pipeline_parent_class)->dispose(object);
}

CutPipeline *
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
child_watch_func (GPid pid, gint status, gpointer data)
{
    switch (status) {
      case WEXITED:
        reap_child (CUT_PIPELINE(data), pid);
        break;
      default:
        break;
    }
}

static GIOChannel *
create_io_channel (gint pipe)
{
    GIOChannel *channel;

    channel = g_io_channel_unix_new(pipe);
    g_io_channel_set_encoding(channel, NULL, NULL);
    g_io_channel_set_flags(channel, G_IO_FLAG_IS_READABLE, NULL);
    g_io_channel_set_close_on_unref(channel, TRUE);

    return channel;
}

static gboolean
cut_pipeline_spawn (CutPipeline *pipeline)
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

    command_line = g_strdup_printf("%s -v s --streamer=xml %s",
                                   cutter_command,
                                   priv->target_directory);

    ret = g_shell_parse_argv(command_line, &argc, &argv, NULL);
    g_free(command_line);
    if (!ret)
        return FALSE;

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
    if (!ret)
        return FALSE;

    priv->source_id = g_child_watch_add(priv->pid, child_watch_func, pipeline);
    priv->stdout_io = create_io_channel(std_out);

    return TRUE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
