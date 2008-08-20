/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <gcutter.h>

#include <cutter/cut-pipeline.h>

void test_delegate(void);

static CutRunContext *pipeline;

void
setup (void)
{
    pipeline = cut_pipeline_new();
}

void
teardown (void)
{
    if (pipeline)
        g_object_unref(pipeline);
}

void
test_delegate (void)
{
    CutRunContext *current_run_context;

    cut_run_context_set_test_directory(pipeline,
                                       g_getenv("CUTTEST_PIPELINE_TEST_DIR"));
    cut_run_context_set_source_directory(pipeline,
                                         g_getenv("CUTTEST_PIPELINE_TEST_DIR"));

    current_run_context =
        cut_test_context_get_run_context(get_current_test_context());
    cut_run_context_delegate_signals(pipeline, current_run_context);
    cut_run_context_start(pipeline);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
