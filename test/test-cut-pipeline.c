#include "cutter.h"
#include <cutter/cut-pipeline.h>

#include <unistd.h>

void test_fork (void);

static CutPipeline *pipeline;
static gboolean received_complete_signal = FALSE;

void
setup (void)
{
    pipeline = NULL;
    received_complete_signal = FALSE;
}

void
teardown (void)
{
    if (pipeline)
        g_object_unref(pipeline);
}

static void
cb_complete_signal (CutPipeline *pipeline, gboolean success)
{
    received_complete_signal = TRUE;
}

void
test_fork (void)
{
    pipeline = cut_pipeline_new("./pipeline_test_dir");
    cut_assert(pipeline);

    g_signal_connect(pipeline, "complete", G_CALLBACK(cb_complete_signal), NULL);

    cut_pipeline_run(pipeline);
    while (!received_complete_signal) {
        g_main_context_iteration(NULL, FALSE);
    }

    g_signal_handlers_disconnect_by_func(pipeline,
                                         G_CALLBACK(cb_complete_signal),
                                         NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
