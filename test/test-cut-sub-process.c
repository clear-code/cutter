/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cutter.h>
#include <gcutter.h>
#include <cutter/cut-pipeline.h>
#include <cutter/cut-runner.h>

#include "lib/cuttest-assertions.h"

void test_run (void);

static CutRunContext *pipeline;
static const gchar *env_sub_process_test_dir;

#define build_test_dir(dir, ...)                \
    g_build_filename(cuttest_get_base_dir(),    \
                     "fixtures",                \
                     "sub-process",             \
                     "fixtures",                \
                     dir,                       \
                     ## __VA_ARGS__,            \
                     NULL)

void
setup (void)
{
    pipeline = cut_pipeline_new();
    env_sub_process_test_dir = g_getenv(CUTTEST_TEST_DIR_KEY);
}

void
teardown (void)
{
    if (pipeline)
        g_object_unref(pipeline);

    if (env_sub_process_test_dir)
        g_setenv(CUTTEST_TEST_DIR_KEY, env_sub_process_test_dir, TRUE);
    else
        g_unsetenv(CUTTEST_TEST_DIR_KEY);
}

static void
report_error (CutRunContext *context, GError *error, gpointer user_data)
{
    gcut_assert_error(error, "Pipeline Error");
}

static gboolean
run (const gchar *test_dir)
{
    const gchar *exclude_directories[] = {"fixtures", NULL};
    gchar *test_invoker_dir;

    cut_run_context_set_exclude_directories(pipeline, exclude_directories);

    test_invoker_dir = g_build_filename(cuttest_get_base_dir(),
                                        "fixtures",
                                        "sub-process",
                                        NULL);
    cut_run_context_set_test_directory(pipeline, test_invoker_dir);
    cut_run_context_set_source_directory(pipeline, test_invoker_dir);
    g_free(test_invoker_dir);

    g_setenv(CUTTEST_TEST_DIR_KEY, test_dir, TRUE);

    g_signal_connect(pipeline, "error", G_CALLBACK(report_error), NULL);
    return cut_run_context_start(pipeline);
}

void
test_run (void)
{
    const gchar *test_dir;
    gint n_normal_tests, n_iterated_tests, n_spike_tests;
    gint n_spike_test_assertions;
    gint n_results, n_non_critical_results;

    test_dir = cut_take_string(build_test_dir("normal"));
    cut_assert_false(run(test_dir));

    n_normal_tests = 1;
    n_iterated_tests = 2;
    n_spike_tests = 1;
    n_spike_test_assertions = 1;
    n_results = 6;
    n_non_critical_results = 3;
    cut_assert_test_result_summary(pipeline,
                                   (n_normal_tests + n_iterated_tests) *
                                     n_results + n_spike_tests,
                                   (n_normal_tests + n_iterated_tests) +
                                     (n_spike_test_assertions * n_spike_tests),
                                   (n_normal_tests + n_iterated_tests) *
                                     n_non_critical_results + n_spike_tests,
                                   (n_normal_tests + n_iterated_tests),
                                   (n_normal_tests + n_iterated_tests),
                                   (n_normal_tests + n_iterated_tests),
                                   (n_normal_tests + n_iterated_tests),
                                   (n_normal_tests + n_iterated_tests));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
