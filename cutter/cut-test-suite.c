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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <glib.h>
#include <setjmp.h>
#include <signal.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include "cut-test-suite.h"

#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-run-context.h"
#include "cut-main.h"
#include "cut-utils.h"
#include "cut-marshalers.h"
#include "cut-test-result.h"

#include "../gcutter/gcut-error.h"

#define CUT_TEST_SUITE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_SUITE, CutTestSuitePrivate))

typedef struct _CutTestSuitePrivate	CutTestSuitePrivate;
struct _CutTestSuitePrivate
{
    CutWarmupFunction warmup;
    CutCooldownFunction cooldown;
};

enum
{
    PROP_0,
    PROP_WARMUP_FUNCTION,
    PROP_COOLDOWN_FUNCTION
};

enum
{
    READY_SIGNAL,
    START_TEST_CASE_SIGNAL,
    COMPLETE_TEST_CASE_SIGNAL,
    LAST_SIGNAL
};

static jmp_buf jump_buffer;
#ifndef G_OS_WIN32
static gchar *backtrace = NULL;
#endif

static gint cut_test_suite_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (CutTestSuite, cut_test_suite, CUT_TYPE_TEST_CONTAINER)

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
cut_test_suite_class_init (CutTestSuiteClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_pointer("warmup-function",
                                "Warmup Function",
                                "The function for warmup",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_WARMUP_FUNCTION, spec);

    spec = g_param_spec_pointer("cooldown-function",
                                "Cooldown Function",
                                "The function for cooldown",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_COOLDOWN_FUNCTION, spec);

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

    g_type_class_add_private(gobject_class, sizeof(CutTestSuitePrivate));
}

static void
cut_test_suite_init (CutTestSuite *test_suite)
{
    CutTestSuitePrivate *priv = CUT_TEST_SUITE_GET_PRIVATE(test_suite);

    priv->warmup = NULL;
    priv->cooldown = NULL;
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_test_suite_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestSuitePrivate *priv = CUT_TEST_SUITE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_WARMUP_FUNCTION:
        priv->warmup = g_value_get_pointer(value);
        break;
      case PROP_COOLDOWN_FUNCTION:
        priv->cooldown = g_value_get_pointer(value);
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
    CutTestSuitePrivate *priv = CUT_TEST_SUITE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_WARMUP_FUNCTION:
        g_value_set_pointer(value, priv->warmup);
        break;
      case PROP_COOLDOWN_FUNCTION:
        g_value_set_pointer(value, priv->cooldown);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestSuite *
cut_test_suite_new (const gchar *name,
                    CutWarmupFunction warmup, CutCooldownFunction cooldown)
{
    return g_object_new(CUT_TYPE_TEST_SUITE,
                        "element-name", "test-suite",
                        "name", name,
                        "warmup-function", warmup,
                        "cooldown-function", cooldown,
                        NULL);
}

CutTestSuite *
cut_test_suite_new_empty (void)
{
    return cut_test_suite_new(NULL, NULL, NULL);
}

typedef struct _RunTestInfo
{
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutRunContext *run_context;
    gchar **test_names;
} RunTestInfo;

static void
run (gpointer data, gpointer user_data)
{
    RunTestInfo *info = data;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutRunContext *run_context;
    gchar **test_names;
    gboolean *success = user_data;

    test_suite = info->test_suite;
    test_case = info->test_case;
    run_context = info->run_context;
    test_names = info->test_names;

    g_signal_emit_by_name(test_suite, "start-test-case", test_case);
    if (!cut_test_case_run_with_filter(test_case, run_context,
                                       (const gchar**)test_names))
        *success =  FALSE;
    g_signal_emit_by_name(test_suite, "complete-test-case", test_case);

    g_object_unref(test_suite);
    g_object_unref(test_case);
    g_object_unref(run_context);
    g_strfreev(test_names);
    g_free(info);
}

static void
run_with_thread_support (CutTestSuite *test_suite, CutTestCase *test_case,
                         CutRunContext *run_context, const gchar **test_names,
                         GThreadPool *thread_pool, gboolean *success)
{
    RunTestInfo *info;
    gboolean need_no_thread_run = TRUE;

    if (cut_run_context_is_canceled(run_context))
        return;

    info = g_new0(RunTestInfo, 1);
    info->test_suite = g_object_ref(test_suite);
    info->test_case = g_object_ref(test_case);
    info->run_context = g_object_ref(run_context);
    info->test_names = g_strdupv((gchar **)test_names);

    if (thread_pool) {
        GError *error = NULL;

        g_thread_pool_push(thread_pool, info, &error);
        if (error) {
            gchar *inspected;

            inspected = gcut_error_inspect(error);
            g_warning("%s", inspected);
            g_free(inspected);
            g_error_free(error);
        } else {
            need_no_thread_run = FALSE;
        }
    }

    if (need_no_thread_run)
        run(info, success);
}

#ifndef G_OS_WIN32
static void
read_backtrace (int in_fd)
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
        backtrace = g_regex_replace(regex, buffer->str, buffer->len, 0,
                                    "", 0, NULL);
        g_regex_unref(regex);
        g_string_free(buffer, TRUE);
    } else {
        backtrace = g_string_free(buffer, FALSE);
    }
}

static void
collect_backtrace (void)
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
    g_on_error_stack_trace(cut_get_cutter_command_path());
    stdout = original_stdout;
    fclose(pseudo_stdout);

    read_backtrace(fds[0]);

    close(fds[0]);
    close(fds[1]);
}

static void
i_will_be_back_handler (int signum)
{
    g_free(backtrace);
    backtrace = NULL;
    collect_backtrace();
    longjmp(jump_buffer, signum);
}
#endif

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
cut_test_suite_run_test_cases (CutTestSuite *test_suite,
                               CutRunContext *run_context,
                               GList *test_cases, const gchar **test_names)
{
    CutTestSuitePrivate *priv;
    GList *node;
    GThreadPool *thread_pool = NULL;
    GList *sorted_test_cases;
    gboolean try_thread;
    gboolean all_success = TRUE;
    gint signum;
#ifndef G_OS_WIN32
    struct sigaction i_will_be_back_action;
    struct sigaction previous_segv_action, previous_int_action;
    gboolean set_segv_action = TRUE;
    gboolean set_int_action = TRUE;
#endif

    priv = CUT_TEST_SUITE_GET_PRIVATE(test_suite);

    sorted_test_cases = g_list_copy(test_cases);
    sorted_test_cases = cut_run_context_sort_test_cases(run_context,
                                                        sorted_test_cases);

#ifndef G_OS_WIN32
    i_will_be_back_action.sa_handler = i_will_be_back_handler;
    sigemptyset(&i_will_be_back_action.sa_mask);
    i_will_be_back_action.sa_flags = 0;
    if (sigaction(SIGSEGV, &i_will_be_back_action, &previous_segv_action) == -1)
        set_segv_action = FALSE;
    if (sigaction(SIGINT, &i_will_be_back_action, &previous_int_action) == -1)
        set_int_action = FALSE;
#endif

    try_thread = cut_run_context_get_multi_thread(run_context);
    if (try_thread) {
        GError *error = NULL;
        gint max_threads;

        max_threads = cut_run_context_get_max_threads(run_context);
        thread_pool = g_thread_pool_new(run, &all_success,
                                        max_threads, FALSE, &error);
        if (error) {
            gchar *inspected;

            inspected = gcut_error_inspect(error);
            g_warning("%s", inspected);
            g_free(inspected);
            g_error_free(error);
        }
    }

    signum = setjmp(jump_buffer);
    switch (signum) {
      case 0:
        cut_run_context_prepare_test_suite(run_context, test_suite);
        emit_ready_signal(test_suite, sorted_test_cases, test_names);
        g_signal_emit_by_name(CUT_TEST(test_suite), "start");

        if (priv->warmup)
            priv->warmup();

        for (node = sorted_test_cases; node; node = g_list_next(node)) {
            CutTestCase *test_case = node->data;

            if (!test_case)
                continue;
            if (CUT_IS_TEST_CASE(test_case)) {
                run_with_thread_support(test_suite, test_case, run_context,
                                        test_names, thread_pool, &all_success);
            } else {
                g_warning("This object is not test case!");
            }
        }

        if (thread_pool)
            g_thread_pool_free(thread_pool, FALSE, TRUE);

        if (all_success) {
            CutTestResult *result;
            result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                         NULL, NULL, NULL, test_suite, NULL,
                                         NULL, NULL,
                                         NULL, NULL, 0);
            cut_test_emit_result_signal(CUT_TEST(test_suite), NULL, result);
            g_object_unref(result);
        }
        break;
#ifndef G_OS_WIN32
      case SIGSEGV:
        all_success = FALSE;
        g_signal_emit_by_name(CUT_TEST(test_suite), "crashed", backtrace);
        g_free(backtrace);
        break;
#endif
      default:
        break;
    }
#ifndef G_OS_WIN32
    if (set_int_action)
        sigaction(SIGINT, &previous_int_action, NULL);
    if (set_segv_action)
        sigaction(SIGSEGV, &previous_segv_action, NULL);
#endif

    if (priv->cooldown)
        priv->cooldown();

    g_signal_emit_by_name(CUT_TEST(test_suite), "complete");

    g_list_free(sorted_test_cases);

    return all_success;
}

gboolean
cut_test_suite_run (CutTestSuite *suite, CutRunContext *run_context)
{
    const gchar **test_case_names;
    const gchar **test_names;

    test_case_names = cut_run_context_get_target_test_case_names(run_context);
    test_names = cut_run_context_get_target_test_names(run_context);
    return cut_test_suite_run_with_filter(suite, run_context,
                                          test_case_names,
                                          test_names);
}

gboolean
cut_test_suite_run_test_case (CutTestSuite *suite, CutRunContext *run_context,
                              gchar *test_case_name)
{
    gchar *test_case_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_case_names[0] = test_case_name;
    return cut_test_suite_run_with_filter(suite, run_context,
                                          (const gchar **)test_case_names, NULL);
}

gboolean
cut_test_suite_run_test (CutTestSuite *suite, CutRunContext *run_context,
                         gchar *test_name)
{
    GList *test_cases;
    const gchar *test_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_names[0] = test_name;
    test_cases = cut_test_container_get_children(CUT_TEST_CONTAINER(suite));
    return cut_test_suite_run_test_cases(suite, run_context,
                                         test_cases, test_names);
}

gboolean
cut_test_suite_run_test_in_test_case (CutTestSuite *suite,
                                      CutRunContext    *run_context,
                                      gchar        *test_name,
                                      gchar        *test_case_name)
{
    gchar *test_names[] = {NULL, NULL};
    gchar *test_case_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_names[0] = test_name;
    test_case_names[0] = test_case_name;

    return cut_test_suite_run_with_filter(suite, run_context,
                                          (const gchar **)test_case_names,
                                          (const gchar **)test_names);
}

gboolean
cut_test_suite_run_with_filter (CutTestSuite *test_suite,
                                CutRunContext   *run_context,
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

    success = cut_test_suite_run_test_cases(test_suite, run_context,
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
