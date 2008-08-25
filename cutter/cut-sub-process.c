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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "cut-sub-process.h"
#include "cut-pipeline.h"
#include "cut-test-context.h"

#define CUT_SUB_PROCESS_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_SUB_PROCESS, CutSubProcessPrivate))

typedef struct _CutSubProcessPrivate         CutSubProcessPrivate;
struct _CutSubProcessPrivate
{
    CutRunContext *pipeline;
    CutTestContext *test_context;
    gboolean running;
    gboolean is_success;
    gboolean is_ran;
};

enum {
    PROP_0,
    PROP_PIPELINE,
    PROP_TEST_CONTEXT
};

G_DEFINE_TYPE(CutSubProcess, cut_sub_process, G_TYPE_OBJECT)

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
cut_sub_process_class_init (CutSubProcessClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("pipeline",
                               "Pipeline",
                               "The pipeline",
                               CUT_TYPE_PIPELINE,
                               G_PARAM_READABLE | G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_PIPELINE, spec);

    spec = g_param_spec_object("test-context",
                               "Test context",
                               "The test context",
                               CUT_TYPE_TEST_CONTEXT,
                               G_PARAM_READABLE | G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_TEST_CONTEXT, spec);

    g_type_class_add_private(gobject_class, sizeof(CutSubProcessPrivate));
}

static void
cut_sub_process_init (CutSubProcess *sub_process)
{
    CutSubProcessPrivate *priv;

    priv = CUT_SUB_PROCESS_GET_PRIVATE(sub_process);
    priv->pipeline = NULL;
    priv->test_context = NULL;
    priv->running = FALSE;
    priv->is_success = TRUE;
    priv->is_ran = FALSE;
}

static void
dispose (GObject *object)
{
    CutSubProcessPrivate *priv;

    priv = CUT_SUB_PROCESS_GET_PRIVATE(object);

    if (priv->running) {
        if (priv->pipeline)
            cut_run_context_emit_complete_run(priv->pipeline, FALSE);
        priv->running = FALSE;
    }

    if (priv->pipeline) {
        g_object_unref(priv->pipeline);
        priv->pipeline = NULL;
    }

    if (priv->test_context) {
        g_object_unref(priv->test_context);
        priv->test_context = NULL;
    }

    G_OBJECT_CLASS(cut_sub_process_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutSubProcess *sub_process;

    sub_process = CUT_SUB_PROCESS(object);
    switch (prop_id) {
      case PROP_PIPELINE:
        cut_sub_process_set_pipeline(sub_process, g_value_get_object(value));
        break;
      case PROP_TEST_CONTEXT:
        cut_sub_process_set_test_context(sub_process, g_value_get_object(value));
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
    CutSubProcessPrivate *priv = CUT_SUB_PROCESS_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_PIPELINE:
        g_value_set_object(value, priv->pipeline);
        break;
      case PROP_TEST_CONTEXT:
        g_value_set_object(value, priv->test_context);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutSubProcess *
cut_sub_process_new (const char *test_directory, CutTestContext *test_context)
{
    CutSubProcess *sub_process;
    CutRunContext *parent_run_context = NULL;
    CutRunContext *pipeline;

    if (test_context)
        parent_run_context = cut_test_context_get_run_context(test_context);

    pipeline = cut_pipeline_new();
    cut_run_context_set_source_directory(pipeline, test_directory);
    cut_run_context_set_test_directory(pipeline, test_directory);

    if (parent_run_context) {
        if (cut_run_context_get_multi_thread(parent_run_context))
            cut_run_context_set_multi_thread(pipeline, TRUE);
        cut_run_context_delegate_signals(pipeline, parent_run_context);
    }

    sub_process = g_object_new(CUT_TYPE_SUB_PROCESS,
                               "test-context", test_context,
                               "pipeline", pipeline,
                               NULL);
    g_object_unref(pipeline);

    return sub_process;
}

CutRunContext *
cut_sub_process_get_pipeline (CutSubProcess *sub_process)
{
    return CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
}

void
cut_sub_process_set_pipeline (CutSubProcess *sub_process,
                              CutRunContext *pipeline)
{
    CutSubProcessPrivate *priv;

    priv = CUT_SUB_PROCESS_GET_PRIVATE(sub_process);
    if (priv->pipeline)
        g_object_unref(priv->pipeline);
    if (pipeline)
        g_object_ref(pipeline);
    priv->pipeline = pipeline;
}

CutTestContext *
cut_sub_process_get_test_context (CutSubProcess *sub_process)
{
    return CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->test_context;
}

void
cut_sub_process_set_test_context (CutSubProcess  *sub_process,
                                  CutTestContext *test_context)
{
    CutSubProcessPrivate *priv;

    priv = CUT_SUB_PROCESS_GET_PRIVATE(sub_process);
    if (priv->test_context)
        g_object_unref(priv->test_context);
    if (test_context)
        g_object_ref(test_context);
    priv->test_context = test_context;
}

gboolean
cut_sub_process_run (CutSubProcess *sub_process)
{
    CutSubProcessPrivate *priv;

    priv = CUT_SUB_PROCESS_GET_PRIVATE(sub_process);
    if (!priv->is_ran) {
        priv->running = TRUE;
        priv->is_success = cut_run_context_start(priv->pipeline);
        priv->running = FALSE;
        priv->is_ran = TRUE;
    }
    return priv->is_success;
}

static void
cb_complete_run (CutRunContext *pipeline, gboolean success, gpointer user_data)
{
    CutSubProcess *sub_process = user_data;
    CutSubProcessPrivate *priv;

    g_signal_handlers_disconnect_by_func(pipeline,
                                         G_CALLBACK(cb_complete_run),
                                         user_data);

    priv = CUT_SUB_PROCESS_GET_PRIVATE(sub_process);
    priv->is_success = success;
    priv->running = FALSE;
    priv->is_ran = TRUE;
}

void
cut_sub_process_run_async (CutSubProcess *sub_process)
{
    CutSubProcessPrivate *priv;

    priv = CUT_SUB_PROCESS_GET_PRIVATE(sub_process);
    if (!priv->is_ran && !priv->running) {
        priv->running = TRUE;
        priv->is_success = TRUE;
        g_signal_connect(priv->pipeline, "complete-run",
                         G_CALLBACK(cb_complete_run), sub_process);
        cut_run_context_start_async(priv->pipeline);
    }
}

gboolean
cut_sub_process_wait (CutSubProcess *sub_process)
{
    CutSubProcessPrivate *priv;

    priv = CUT_SUB_PROCESS_GET_PRIVATE(sub_process);
    if (!priv->is_ran) {
        while (priv->running)
            g_main_context_iteration(NULL, FALSE);
    }
    return priv->is_success;
}

gboolean
cut_sub_process_is_success (CutSubProcess *sub_process)
{
    return CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->is_success;
}

gboolean
cut_sub_process_is_running (CutSubProcess *sub_process)
{
    return CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->running;
}


const char *
cut_sub_process_get_test_directory (CutSubProcess *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_test_directory(pipeline);
}

void cut_sub_process_set_test_directory (CutSubProcess  *sub_process,
                                         const char     *test_directory)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    cut_run_context_set_test_directory(pipeline, test_directory);
}

const char *
cut_sub_process_get_source_directory (CutSubProcess *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_source_directory(pipeline);
}

void
cut_sub_process_set_source_directory (CutSubProcess  *sub_process,
                                      const char     *source_directory)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    cut_run_context_set_source_directory(pipeline, source_directory);
}

gboolean
cut_sub_process_is_multi_thread (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_is_multi_thread(pipeline);
}

gboolean
cut_sub_process_get_multi_thread (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_multi_thread(pipeline);
}

void
cut_sub_process_set_multi_thread (CutSubProcess  *sub_process,
                                  gboolean        multi_thread)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    cut_run_context_set_multi_thread(pipeline, multi_thread);
}

const char **
cut_sub_process_get_exclude_files (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_exclude_files(pipeline);
}

void
cut_sub_process_set_exclude_files (CutSubProcess  *sub_process,
                                   const char    **files)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    cut_run_context_set_exclude_files(pipeline, files);
}


const char **
cut_sub_process_get_exclude_directories (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_exclude_directories(pipeline);
}

void
cut_sub_process_set_exclude_directories (CutSubProcess  *sub_process,
                                         const char    **directories)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    cut_run_context_set_exclude_directories(pipeline, directories);
}


const char **
cut_sub_process_get_target_test_case_names (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_target_test_case_names(pipeline);
}

void
cut_sub_process_set_target_test_case_names (CutSubProcess  *sub_process,
                                            const char    **names)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    cut_run_context_set_target_test_case_names(pipeline, names);
}

const char **
cut_sub_process_get_target_test_names (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_target_test_names(pipeline);
}

void
cut_sub_process_set_target_test_names (CutSubProcess  *sub_process,
                                       const char    **names)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    cut_run_context_set_target_test_names(pipeline, names);
}

double
cut_sub_process_get_elapsed (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_elapsed(pipeline);
}

double
cut_sub_process_get_total_elapsed (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_total_elapsed(pipeline);
}

gboolean
cut_sub_process_is_crashed (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_is_crashed(pipeline);
}

const char *
cut_sub_process_get_backtrace (CutSubProcess  *sub_process)
{
    CutRunContext *pipeline;

    pipeline = CUT_SUB_PROCESS_GET_PRIVATE(sub_process)->pipeline;
    return cut_run_context_get_backtrace(pipeline);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
