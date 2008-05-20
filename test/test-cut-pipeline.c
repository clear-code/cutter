#include "cutter.h"
#include <cutter/cut-pipeline.h>
#include <cutter/cut-runner.h>

#include "cuttest-utils.h"

#include <unistd.h>

void test_exit_status_error (void);
void test_exit_status_success (void);
void test_failure_signal (void);
void test_error_signal (void);
void test_pending_signal (void);
void test_omission_signal (void);
void test_failure_count (void);
void test_error_count (void);
void test_pending_count (void);
void test_omission_count (void);

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

static gboolean
run (const gchar *target_dir)
{
    cut_run_context_set_test_directory(pipeline, target_dir);

    return cut_runner_run(CUT_RUNNER(pipeline));
}

void
test_exit_status_error (void)
{
    cut_assert_false(run(error_test_dir));
}

void
test_exit_status_success (void)
{
    cut_assert_true(run(success_test_dir));
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
    gint n_signal = 0;                                                  \
                                                                        \
    g_signal_connect(pipeline, #signal_name "-test",                    \
                     G_CALLBACK(cb_ ## signal_name ## _signal),         \
                     &n_signal);                                        \
                                                                        \
    cut_assert_false(run(error_test_dir));                              \
    cut_assert_equal_int(1, n_signal);                                  \
                                                                        \
    g_signal_handlers_disconnect_by_func(                               \
        pipeline, G_CALLBACK(cb_ ## signal_name ## _signal),            \
        &n_signal);                                                     \
}

DEFINE_ERROR_SIGNAL_TEST(failure)
DEFINE_ERROR_SIGNAL_TEST(error)
DEFINE_ERROR_SIGNAL_TEST(pending)
DEFINE_ERROR_SIGNAL_TEST(omission)

#define DEFINE_ERROR_COUNT_TEST(status_name)                            \
void                                                                    \
test_ ## status_name ## _count (void)                                   \
{                                                                       \
    cut_assert_false(run(error_test_dir));                              \
    cut_assert_equal_int(1,                                             \
        cut_run_context_get_n_ ## status_name ## s(pipeline));          \
}

DEFINE_ERROR_COUNT_TEST(failure)
DEFINE_ERROR_COUNT_TEST(error)
DEFINE_ERROR_COUNT_TEST(pending)
DEFINE_ERROR_COUNT_TEST(omission)

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
