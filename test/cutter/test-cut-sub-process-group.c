/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cutter.h>
#include <gcutter.h>
#include <cutter/cut-pipeline.h>
#include <cutter/cut-runner.h>

#include "../lib/cuttest-assertions.h"

void test_run (void);
void test_run_async (void);

static CutRunContext *pipeline;
static gchar *env_test_dir;

#define build_test_dir(dir, ...)                \
    g_build_filename(cuttest_get_base_dir(),    \
                     "fixtures",                \
                     "sub-process-group",       \
                     "fixtures",                \
                     dir,                       \
                     ## __VA_ARGS__,            \
                     NULL)

void
cut_setup (void)
{
    pipeline = cut_pipeline_new();
    env_test_dir = g_strdup(g_getenv(CUTTEST_TEST_DIR_KEY));
}

void
cut_teardown (void)
{
    if (pipeline)
        g_object_unref(pipeline);

    if (env_test_dir) {
        g_setenv(CUTTEST_TEST_DIR_KEY, env_test_dir, TRUE);
        g_free(env_test_dir);
    } else {
        g_unsetenv(CUTTEST_TEST_DIR_KEY);
    }
}

static void
report_error (CutRunContext *context, GError *error, gpointer user_data)
{
    gcut_assert_error(error, "Pipeline Error");
}

static gboolean
run (const gchar *test_dir, const gchar *test_name)
{
    const gchar *exclude_directories[] = {"fixtures", NULL};
    const gchar *test_names[] = {NULL, NULL};
    gchar *test_invoker_dir;

    cut_run_context_set_exclude_directories(pipeline, exclude_directories);
    test_names[0] = test_name;
    cut_run_context_set_target_test_names(pipeline, test_names);

    test_invoker_dir = g_build_filename(cuttest_get_base_dir(),
                                        "fixtures",
                                        "sub-process-group",
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
    gint n_processes, n_normal_tests, n_iterated_tests, n_spike_tests;
    gint n_results, n_non_critical_results;
    gint n_tests, n_assertions, n_successes, n_failures, n_errors;
    gint n_pendings, n_notifications, n_omissions;

    test_dir = cut_take_string(build_test_dir("normal"));
    cut_assert_false(run(test_dir, "test_run"));

    n_processes = 3;
    n_normal_tests = 1;
    n_spike_tests = 1;
    n_iterated_tests = 2;
    n_results = 6;
    n_non_critical_results = 3;

    n_tests = ((n_normal_tests + n_iterated_tests) * n_results);
    n_assertions = (n_normal_tests + n_iterated_tests);
    n_successes = (n_normal_tests + n_iterated_tests) * n_non_critical_results;
    n_failures = (n_normal_tests + n_iterated_tests);
    n_errors = (n_normal_tests + n_iterated_tests);
    n_pendings = (n_normal_tests + n_iterated_tests);
    n_notifications = (n_normal_tests + n_iterated_tests);
    n_omissions = (n_normal_tests + n_iterated_tests);

    cut_assert_test_result_summary(pipeline,
                                   n_tests * n_processes + n_spike_tests,
                                   n_assertions * n_processes,
                                   n_successes * n_processes + n_spike_tests,
                                   n_failures * n_processes,
                                   n_errors * n_processes,
                                   n_pendings * n_processes,
                                   n_notifications * n_processes,
                                   n_omissions * n_processes);
}

void
test_run_async (void)
{
    const gchar *test_dir;
    gint n_processes, n_normal_tests, n_iterated_tests, n_spike_tests;
    gint n_results, n_non_critical_results;
    gint n_tests, n_assertions, n_successes, n_failures, n_errors;
    gint n_pendings, n_notifications, n_omissions;

    test_dir = cut_take_string(build_test_dir("normal"));
    cut_assert_false(run(test_dir, "test_run_async"));

    n_processes = 3;
    n_normal_tests = 1;
    n_spike_tests = 1;
    n_iterated_tests = 2;
    n_results = 6;
    n_non_critical_results = 3;

    n_tests = ((n_normal_tests + n_iterated_tests) * n_results);
    n_assertions = (n_normal_tests + n_iterated_tests);
    n_successes = (n_normal_tests + n_iterated_tests) * n_non_critical_results;
    n_failures = (n_normal_tests + n_iterated_tests);
    n_errors = (n_normal_tests + n_iterated_tests);
    n_pendings = (n_normal_tests + n_iterated_tests);
    n_notifications = (n_normal_tests + n_iterated_tests);
    n_omissions = (n_normal_tests + n_iterated_tests);

    cut_assert_test_result_summary(pipeline,
                                   n_tests * n_processes + n_spike_tests,
                                   n_assertions * n_processes,
                                   n_successes * n_processes + n_spike_tests,
                                   n_failures * n_processes,
                                   n_errors * n_processes,
                                   n_pendings * n_processes,
                                   n_notifications * n_processes,
                                   n_omissions * n_processes);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
