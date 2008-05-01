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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <setjmp.h>
#define __USE_GNU
#include <signal.h>
#undef __USE_GNU

#include "cut-test-suite.h"

#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-runner.h"
#include "cut-utils.h"
#include "cut-marshalers.h"
#include "cut-test-result.h"

enum
{
    READY_SIGNAL,
    START_TEST_CASE_SIGNAL,
    COMPLETE_TEST_CASE_SIGNAL,
    LAST_SIGNAL
};

static jmp_buf jump_buffer;
static gchar *stack_trace = NULL;

static gint cut_test_suite_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (CutTestSuite, cut_test_suite, CUT_TYPE_TEST_CONTAINER)

static void
cut_test_suite_class_init (CutTestSuiteClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    cut_test_suite_signals[READY_SIGNAL]
        = g_signal_new("ready",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestSuiteClass, ready),
                       NULL, NULL,
                       _cut_marshal_VOID__UINT_UINT,
                       G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);

    cut_test_suite_signals[START_TEST_CASE_SIGNAL]
        = g_signal_new("start-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestSuiteClass, start_test_case),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__OBJECT,
                       G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

    cut_test_suite_signals[COMPLETE_TEST_CASE_SIGNAL]
        = g_signal_new("complete-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestSuiteClass, complete_test_case),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__OBJECT,
                       G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);
}

static void
cut_test_suite_init (CutTestSuite *test_suite)
{
}

CutTestSuite *
cut_test_suite_new (void)
{
    return g_object_new(CUT_TYPE_TEST_SUITE, NULL);
}

typedef struct _RunTestInfo
{
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutRunner *runner;
    gchar **test_names;
} RunTestInfo;

static gpointer
run (gpointer data)
{
    RunTestInfo *info = data;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutRunner *runner;
    gchar **test_names;
    gboolean success;

    test_suite = info->test_suite;
    test_case = info->test_case;
    runner = info->runner;
    test_names = info->test_names;

    g_signal_emit_by_name(test_suite, "start-test-case", test_case);
    success = cut_test_case_run_with_filter(test_case, runner,
                                            (const gchar **)test_names);
    g_signal_emit_by_name(test_suite, "complete-test-case", test_case);

    g_object_unref(test_suite);
    g_object_unref(test_case);
    g_object_unref(runner);
    g_strfreev(test_names);
    g_free(info);

    return GINT_TO_POINTER(success);
}

static void
run_with_thread (CutTestSuite *test_suite, CutTestCase *test_case,
                 CutRunner *runner, const gchar **test_names,
                 gboolean try_thread, GList **threads, gboolean *success)
{
    RunTestInfo *info;
    gboolean need_no_thread_run = TRUE;

    if (cut_runner_is_canceled(runner))
        return;

    info = g_new0(RunTestInfo, 1);
    info->test_suite = g_object_ref(test_suite);
    info->test_case = g_object_ref(test_case);
    info->runner = g_object_ref(runner);
    info->test_names = g_strdupv((gchar **)test_names);

    if (try_thread) {
        GThread *thread;
        GError *error = NULL;

        thread = g_thread_create(run, info, TRUE, &error);
        if (error) {
            g_warning("%s(%d)", error->message, error->code);
            g_error_free(error);
        } else {
            need_no_thread_run = FALSE;
            *threads = g_list_append(*threads, thread);
        }
    }

    if (need_no_thread_run) {
        if (!run(info))
            *success = FALSE;
    }
}

static void
read_stack_trace (int in_fd)
{
    GRegex *regex;
    GString *buffer;
    gchar window[4096];
    gssize size;

    buffer = g_string_new("");
    while ((size = read(in_fd, window, sizeof(window))) > 0) {
        g_string_append_len(buffer, window, size);
    }

    regex = g_regex_new("^.+?<signal handler called>\n",
                        G_REGEX_MULTILINE | G_REGEX_DOTALL,
                        G_REGEX_MATCH_NEWLINE_ANY, NULL);
    if (regex) {
        stack_trace = g_regex_replace(regex, buffer->str, buffer->len, 0,
                                      "", 0, NULL);
        g_regex_unref(regex);
        g_string_free(buffer, TRUE);
    } else {
        stack_trace = g_string_free(buffer, FALSE);
    }
}

static void
collect_stack_trace (void)
{
    int fds[2];
    FILE *original_stdout, *pseudo_stdout;

    if (pipe(fds) == -1) {
        perror("unable to open pipe for collecting stack trace");
        return;
    }

    original_stdout = stdout;
    pseudo_stdout = fdopen(fds[1], "w");
    if (!pseudo_stdout) {
        perror("unable to open FILE for pipe");
        close(fds[0]);
        close(fds[1]);
        return;
    }
    stdout = pseudo_stdout;
    g_on_error_stack_trace(g_get_prgname());
    stdout = original_stdout;
    fclose(pseudo_stdout);

    read_stack_trace(fds[0]);

    close(fds[0]);
    close(fds[1]);
}

static void
i_will_be_back_handler (int signum)
{
    g_free(stack_trace);
    stack_trace = NULL;
    collect_stack_trace();
    longjmp(jump_buffer, signum);
}

static void
emit_ready_signal (CutTestSuite *test_suite, GList *test_cases,
                   const gchar **test_names)
{
    GList *node;
    guint n_test_cases, n_tests;

    n_test_cases = 0;
    n_tests = 0;

    for (node = test_cases; node; node = g_list_next(node)) {
        CutTestCase *test_case = node->data;

        n_test_cases++;
        n_tests += cut_test_case_get_n_tests(test_case, test_names);
    }

    g_signal_emit_by_name(test_suite, "ready", n_test_cases, n_tests);
}

static gboolean
cut_test_suite_run_test_cases (CutTestSuite *test_suite, CutRunner *runner,
                               GList *test_cases, const gchar **test_names)
{
    GList *node, *threads = NULL;
    GList *sorted_test_cases;
    gboolean try_thread;
    gboolean all_success = TRUE;
    gint signum;
    sighandler_t previous_segv_handler, previous_int_handler;

    sorted_test_cases = g_list_copy(test_cases);
    sorted_test_cases = cut_runner_sort_test_cases(runner, sorted_test_cases);
    previous_segv_handler = signal(SIGSEGV, i_will_be_back_handler);
    previous_int_handler = signal(SIGINT, i_will_be_back_handler);
    signum = setjmp(jump_buffer);
    switch (signum) {
      case 0:
        cut_runner_prepare_test_suite(runner, test_suite);
        emit_ready_signal(test_suite, sorted_test_cases, test_names);
        g_signal_emit_by_name(CUT_TEST(test_suite), "start");

        try_thread = cut_runner_get_multi_thread(runner);
        for (node = sorted_test_cases; node; node = g_list_next(node)) {
            CutTestCase *test_case = node->data;

            if (!test_case)
                continue;
            if (CUT_IS_TEST_CASE(test_case)) {
                run_with_thread(test_suite, test_case, runner, test_names,
                                try_thread, &threads, &all_success);
            } else {
                g_warning("This object is not test case!");
            }
        }

        for (node = threads; node; node = g_list_next(node)) {
            GThread *thread = node->data;

            if (!GPOINTER_TO_INT(g_thread_join(thread)))
                all_success = FALSE;
        }

        if (all_success) {
            CutTestResult *result;
            result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                         NULL,
                                         NULL,
                                         test_suite,
                                         NULL, NULL,
                                         NULL, NULL, 0);
            g_signal_emit_by_name(CUT_TEST(test_suite), "success", NULL, result);
            g_object_unref(result);
        }
        break;
      case SIGSEGV:
        all_success = FALSE;
        g_signal_emit_by_name(CUT_TEST(test_suite), "crashed", stack_trace);
        g_free(stack_trace);
        break;
      default:
        break;
    }
    signal(SIGINT, previous_int_handler);
    signal(SIGSEGV, previous_segv_handler);

    g_signal_emit_by_name(CUT_TEST(test_suite), "complete");

    g_list_free(sorted_test_cases);

    return all_success;
}

gboolean
cut_test_suite_run (CutTestSuite *suite, CutRunner *runner)
{
    const gchar **test_case_names;
    const gchar **test_names;

    test_case_names = cut_runner_get_target_test_case_names(runner);
    test_names = cut_runner_get_target_test_names(runner);
    return cut_test_suite_run_with_filter(suite, runner,
                                          test_case_names, test_names);
}

gboolean
cut_test_suite_run_test_case (CutTestSuite *suite, CutRunner *runner,
                              const gchar *test_case_name)
{
    const gchar *test_case_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_case_names[0] = test_case_name;
    return cut_test_suite_run_with_filter(suite, runner,
                                          test_case_names, NULL);
}

gboolean
cut_test_suite_run_test (CutTestSuite *suite, CutRunner *runner,
                         const gchar *test_name)
{
    GList *test_cases;
    const gchar *test_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_names[0] = test_name;
    test_cases = cut_test_container_get_children(CUT_TEST_CONTAINER(suite));
    return cut_test_suite_run_test_cases(suite, runner,
                                         test_cases, test_names);
}

gboolean
cut_test_suite_run_test_in_test_case (CutTestSuite *suite,
                                      CutRunner   *runner,
                                      const gchar  *test_name,
                                      const gchar  *test_case_name)
{
    const gchar *test_names[] = {NULL, NULL};
    const gchar *test_case_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_names[0] = test_name;
    test_case_names[0] = test_case_name;

    return cut_test_suite_run_with_filter(suite, runner,
                                          test_case_names, test_names);
}

gboolean
cut_test_suite_run_with_filter (CutTestSuite *test_suite,
                                CutRunner   *runner,
                                const gchar **test_case_names,
                                const gchar **test_names)
{
    CutTestContainer *container;
    GList *filtered_test_cases = NULL;
    gboolean success = TRUE;

    container = CUT_TEST_CONTAINER(test_suite);
    if (test_case_names && *test_case_names) {
        filtered_test_cases =
            cut_test_container_filter_children(container, test_case_names);
    } else {
        CutTestContainer *container;

        container = CUT_TEST_CONTAINER(test_suite);
        filtered_test_cases =
            g_list_copy(cut_test_container_get_children(container));
    }

    if (!filtered_test_cases)
        return success;

    success = cut_test_suite_run_test_cases(test_suite, runner,
                                            filtered_test_cases, test_names);
    g_list_free(filtered_test_cases);

    return success;
}


void
cut_test_suite_add_test_case (CutTestSuite *suite, CutTestCase *test_case)
{
    g_return_if_fail(CUT_IS_TEST_CASE(test_case));
    g_return_if_fail(CUT_IS_TEST_SUITE(suite));

    cut_test_container_add_test(CUT_TEST_CONTAINER(suite), CUT_TEST(test_case));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
