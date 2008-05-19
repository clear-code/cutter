#include "cutter.h"
#include <cutter/cut-pipeline.h>
#include <cutter/cut-runner.h>

#include "cuttest-utils.h"

#include <unistd.h>

void test_exit_status_error (void);
void test_exit_status_success (void);
void test_failure_signal(void);
void test_error_signal(void);
void test_pending_signal(void);

static CutRunContext *pipeline;
static gboolean received_complete_signal = FALSE;
static gchar *success_test_dir, *error_test_dir;

void
startup (void)
{
    success_test_dir = g_build_filename(cuttest_get_base_dir(),
                                        "pipeline_test_dir",
                                        "success_test",
                                        NULL);
    error_test_dir = g_build_filename(cuttest_get_base_dir(),
                                      "pipeline_test_dir",
                                      "error_test",
                                      NULL);
}

void
shutdown (void)
{
    g_free(success_test_dir);
    g_free(error_test_dir);
}

void
setup (void)
{
    pipeline = cut_pipeline_new();
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

    is_success = (gboolean *)user_data;
    *is_success = success;
}

void
test_exit_status_error (void)
{
    gboolean success = TRUE;

    cut_assert(pipeline);

    cut_run_context_set_test_directory(CUT_RUN_CONTEXT(pipeline), error_test_dir);
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
test_exit_status_success (void)
{
    gboolean success = FALSE;

    cut_run_context_set_test_directory(CUT_RUN_CONTEXT(pipeline), success_test_dir);

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

#define DEFINE_ERROR_SIGNAL_TEST(signal_name)                           \
static void                                                             \
cb_ ## signal_name ## _signal (CutRunContext *run_context,              \
                               CutTest *test,                           \
                               CutTestContext *test_context,            \
                               CutTestResult *result,                   \
                               gpointer data)                           \
{                                                                       \
    gint *n_signal;                                                     \
                                                                        \
    n_signal = (gint *)data;                                            \
    (*n_signal)++;                                                      \
}                                                                       \
                                                                        \
void                                                                    \
test_ ## signal_name ## _signal (void)                                  \
{                                                                       \
    gboolean success = FALSE;                                           \
    gint n_signal = 0;                                                  \
                                                                        \
    cut_assert(pipeline);                                               \
    cut_run_context_set_test_directory(CUT_RUN_CONTEXT(pipeline),       \
                                       error_test_dir);                 \
                                                                        \
    g_signal_connect(pipeline, "complete-run",                          \
                     G_CALLBACK(cb_complete_run_signal), &success);     \
    g_signal_connect(pipeline, #signal_name "-test",                    \
                     G_CALLBACK(cb_ ## signal_name ## _signal),         \
                     &n_signal);                                        \
                                                                        \
    cut_runner_run_async(CUT_RUNNER(pipeline));                         \
    while (!received_complete_signal) {                                 \
        g_main_context_iteration(NULL, FALSE);                          \
    }                                                                   \
                                                                        \
    cut_assert_false(success);                                          \
    cut_assert_equal_int(1, n_signal);                                  \
                                                                        \
    g_signal_handlers_disconnect_by_func(                               \
        pipeline, G_CALLBACK(cb_complete_run_signal), &success);        \
    g_signal_handlers_disconnect_by_func(                               \
        pipeline, G_CALLBACK(cb_ ## signal_name ## _signal),            \
        &n_signal);                                                     \
}

DEFINE_ERROR_SIGNAL_TEST(failure)
DEFINE_ERROR_SIGNAL_TEST(error)
DEFINE_ERROR_SIGNAL_TEST(pending)

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
