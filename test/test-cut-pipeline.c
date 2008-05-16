#include "cutter.h"
#include <cutter/cut-pipeline.h>
#include <cutter/cut-runner.h>

#include <unistd.h>

void test_error (void);
void test_success (void);

static CutRunContext *pipeline;
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
cb_complete_run_signal (CutPipeline *pipeline, gboolean success, gpointer user_data)
{
    gboolean *is_success;
    received_complete_signal = TRUE;

    is_success = (gboolean*)user_data;
    *is_success = success;
}

void
test_error (void)
{
    gboolean success = TRUE;

    pipeline = cut_pipeline_new("./pipeline_test_dir/error_test/");
    cut_assert(pipeline);

    g_signal_connect(pipeline, "complete-run",
                     G_CALLBACK(cb_complete_run_signal), &success);

    cut_runner_run_async(CUT_RUNNER(pipeline));
    while (!received_complete_signal) {
        g_main_context_iteration(NULL, FALSE);
    }

    cut_assert_false(success);
    g_signal_handlers_disconnect_by_func(pipeline,
                                         G_CALLBACK(cb_complete_run_signal),
                                         &success);
}

void
test_success (void)
{
    gboolean success = FALSE;

    pipeline = cut_pipeline_new("./pipeline_test_dir/success_test/");
    cut_assert(pipeline);

    g_signal_connect(pipeline, "complete-run",
                     G_CALLBACK(cb_complete_run_signal), &success);

    cut_runner_run_async(CUT_RUNNER(pipeline));
    while (!received_complete_signal) {
        g_main_context_iteration(NULL, FALSE);
    }

    cut_assert_true(success);
    g_signal_handlers_disconnect_by_func(pipeline,
                                         G_CALLBACK(cb_complete_run_signal),
                                         &success);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
