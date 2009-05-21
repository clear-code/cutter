/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#include <signal.h>

#include <cutter.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-crash-backtrace.h>

#include "../lib/cuttest-assertions.h"

void test_setup(void);
void test_teardown(void);
void test_test_case_count(void);
void test_run(void);
void test_run_with_setup_error(void);
void test_run_this_function(void);
void test_run_tests_with_regex(void);
void test_run_with_name_filter(void);
void test_run_with_regex_filter(void);
void test_run_with_name_and_regex_filter(void);
void test_get_name(void);
void test_start_signal(void);
void test_success_signal(void);
void test_failure_signal(void);
void test_error_signal(void);
void test_pending_signal(void);
void test_notification_signal(void);
void test_omission_signal(void);
void test_crash_signal(void);
void test_failure_in_signal(void);
void test_error_in_signal(void);
void test_pending_in_signal(void);
void test_notification_in_signal(void);
void test_omission_in_signal(void);
void test_crash_in_signal(void);
void test_complete_signal(void);

static CutTestCase *test_object;
static CutRunContext *run_context;

static gboolean set_failure_on_startup = FALSE;
static gboolean set_error_on_startup = FALSE;
static gboolean set_pending_on_startup = FALSE;
static gboolean set_notification_on_startup = FALSE;
static gboolean set_omission_on_startup = FALSE;
static gboolean set_crash_on_startup = FALSE;

static gboolean set_error_on_setup = FALSE;

static gint n_startup = 0;
static gint n_shutdown = 0;
static gint n_setup = 0;
static gint n_teardown = 0;
static gint n_run_stub_run_test_function = 0;
static gint n_run_stub_test_function1 = 0;
static gint n_run_stub_test_function2 = 0;

static void
stub_test_function1 (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    n_run_stub_test_function1++;
}

static void
stub_test_function2 (void)
{
    n_run_stub_test_function2++;
}

static void
stub_run_test_function (void)
{
    n_run_stub_run_test_function++;
}

static void
stub_failure_test (void)
{
    cut_fail("Failed!!!");
}

static void
stub_error_test (void)
{
    cut_error("Error!!!");
}

static void
stub_pending_test (void)
{
    cut_pend("Pend!!!");
}

static void
stub_notification_test (void)
{
    cut_notify("Notify!!!");
}

static void
stub_omission_test (void)
{
    cut_omit("Omit!!!");
}

#ifndef G_OS_WIN32
static void
stub_crash_test (void)
{
    raise(SIGABRT);
}
#endif

static void
stub_setup_function (void)
{
    n_setup++;

    if (set_error_on_setup)
        cut_error("Error in setup");
}

static void
stub_teardown_function (void)
{
    n_teardown++;
}

static void
stub_startup_function (void)
{
    n_startup++;

    if (set_failure_on_startup) {
        cut_fail("failure in startup");
    } else if (set_error_on_startup) {
        cut_error("error in startup");
    } else if (set_pending_on_startup) {
        cut_pend("pending in startup");
    } else if (set_notification_on_startup) {
        cut_notify("notification in startup");
    } else if (set_omission_on_startup) {
        cut_omit("omission in startup");
    } else if (set_crash_on_startup) {
#ifndef G_OS_WIN32
        raise(SIGTERM);
#endif
    }
}

static void
stub_shutdown_function (void)
{
    n_shutdown++;
}

void
cut_setup (void)
{
    const gchar *test_names[] = {"/.*/", NULL};

    set_failure_on_startup = FALSE;
    set_error_on_startup = FALSE;
    set_pending_on_startup = FALSE;
    set_notification_on_startup = FALSE;
    set_omission_on_startup = FALSE;
    set_crash_on_startup = FALSE;

    set_error_on_setup = FALSE;

    n_startup = 0;
    n_shutdown = 0;
    n_setup = 0;
    n_teardown = 0;
    n_run_stub_run_test_function = 0;
    n_run_stub_test_function1 = 0;
    n_run_stub_test_function2 = 0;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());
    cut_run_context_set_target_test_names(run_context, test_names);

    test_object = cut_test_case_new("stub test case",
                                    stub_setup_function,
                                    stub_teardown_function,
                                    stub_startup_function,
                                    stub_shutdown_function);
    cuttest_add_test(test_object, "stub_test_1", stub_test_function1);
    cuttest_add_test(test_object, "stub_test_2", stub_test_function2);
    cuttest_add_test(test_object, "run_test_function", stub_run_test_function);
}

void
cut_teardown (void)
{
    cut_crash_backtrace_set_show_on_the_moment(TRUE);

    g_object_unref(test_object);
    g_object_unref(run_context);
}

static void
cb_count_start_test (CutTestCase *test_case, CutTest *test,
                     CutTestContext *test_context, gpointer data)
{
    gint *count = data;
    *count += 1;
}

static void
cb_count_complete_test (CutTestCase *test_case, CutTest *test,
                        CutTestContext *test_context, gboolean success,
                        gpointer data)
{
    gint *count = data;
    *count += 1;
}

static void
cb_count_status (CutTest *test, CutTestContext *test_context,
                 CutTestResult *result, gpointer data)
{
    gint *count = data;
    *count += 1;
}

static void
cb_count_status_in (CutTestCase *test_case, CutTestContext *test_context,
                    CutTestResult *result, gpointer data)
{
    gint *count = data;
    *count += 1;
}

static gboolean
run_the_test (void)
{
    return cut_test_case_run(test_object, run_context);
}

void
test_setup (void)
{
    cut_assert(run_the_test());
    cut_assert(1 < n_setup);
}

void
test_teardown (void)
{
    cut_assert(run_the_test());
    cut_assert(1 < n_teardown);
}

void
test_test_case_count (void)
{
    CutTestContainer *container;
    const gchar *test_names[] = {"/.*/", NULL};

    container = CUT_TEST_CONTAINER(test_object);
    cut_assert_equal_int(3,
                         cut_test_container_get_n_tests(container, NULL));

    cut_run_context_set_target_test_names(run_context, test_names);
    cut_assert_equal_int(3,
                         cut_test_container_get_n_tests(container, run_context));
}

void
test_run (void)
{
    cut_assert(run_the_test());
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
}

void
test_run_with_setup_error (void)
{
    set_error_on_setup = TRUE;
    cut_assert(!run_the_test());
    cut_assert_equal_int(0, n_run_stub_test_function1);
    cut_assert_equal_int(0, n_run_stub_run_test_function);
}

void
test_run_this_function (void)
{
    cut_assert(cut_test_case_run_test(test_object, run_context,
                                      "run_test_function"));

    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stub_test_function1);
}

void
test_run_tests_with_regex (void)
{
    cut_assert(cut_test_case_run_test(test_object, run_context, "/stub/"));
    cut_assert_equal_int(0, n_run_stub_run_test_function);
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
}

void
test_run_with_name_filter (void)
{
    const gchar *names[] = {"stub_test_1", "run_test_function", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, run_context, names));
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(0, n_run_stub_test_function2);
}

void
test_run_with_regex_filter (void)
{
    const gchar *regex[] = {"/stub/", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, run_context, regex));
    cut_assert_equal_int(0, n_run_stub_run_test_function);
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
}

void
test_run_with_name_and_regex_filter (void)
{
    const gchar *name_and_regex[] = {"/stub/", "run_test_function", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, run_context,
                                             name_and_regex));
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
}

void
test_get_name (void)
{
    cut_assert_equal_string("stub test case",
                            cut_test_get_name(CUT_TEST(test_object)));
}

void
test_start_signal (void)
{
    gint n_start_tests = 0;
    g_signal_connect(test_object, "start-test",
                     G_CALLBACK(cb_count_start_test), &n_start_tests);
    cut_assert(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_start_test),
                                         &n_start_tests);
    cut_assert_equal_int(3, n_start_tests);
}

void
test_success_signal (void)
{
    gint n_successes = 0;
    g_signal_connect(test_object, "success",
                     G_CALLBACK(cb_count_status), &n_successes);
    cut_assert(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_successes);
    cut_assert_equal_int(1, n_successes);
}

void
test_failure_signal (void)
{
    gint n_failures = 0;
    g_signal_connect(test_object, "failure",
                     G_CALLBACK(cb_count_status), &n_failures);
    cuttest_add_test(test_object, "stub_failure_test", stub_failure_test);
    cut_assert(!cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_failures);
    cut_assert_equal_int(1, n_failures);
}

void
test_error_signal (void)
{
    gint n_errors = 0;
    g_signal_connect(test_object, "error",
                     G_CALLBACK(cb_count_status), &n_errors);
    cuttest_add_test(test_object, "stub_error_test", stub_error_test);
    cut_assert(!cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_errors);
    cut_assert_equal_int(1, n_errors);
}

void
test_pending_signal (void)
{
    gint n_pendings = 0;
    g_signal_connect(test_object, "pending",
                     G_CALLBACK(cb_count_status), &n_pendings);
    cuttest_add_test(test_object, "stub_pending_test", stub_pending_test);
    cut_assert(!cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_pendings);
    cut_assert_equal_int(1, n_pendings);
}

void
test_notification_signal (void)
{
    gint n_notifications = 0;
    g_signal_connect(test_object, "notification",
                     G_CALLBACK(cb_count_status), &n_notifications);
    cuttest_add_test(test_object, "stub_notification_test",
                     stub_notification_test);
    cut_assert(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_notifications);
    cut_assert_equal_int(1, n_notifications);
}

void
test_omission_signal (void)
{
    gint n_omissions = 0;
    g_signal_connect(test_object, "omission",
                     G_CALLBACK(cb_count_status), &n_omissions);
    cuttest_add_test(test_object, "stub_omission_test",
                     stub_omission_test);
    cut_assert(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_omissions);
    cut_assert_equal_int(1, n_omissions);
}

void
test_crash_signal (void)
{
#ifdef G_OS_WIN32
    cut_omit("crash isn't supported yet on Windows.");
#else
    gint n_crashes = 0;

    cut_crash_backtrace_set_show_on_the_moment(FALSE);

    g_signal_connect(test_object, "crash",
                     G_CALLBACK(cb_count_status), &n_crashes);
    cuttest_add_test(test_object, "stub_crash_test",
                     stub_crash_test);
    cut_assert_false(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_crashes);
    cut_assert_equal_int(1, n_crashes);
#endif
}

void
test_failure_in_signal (void)
{
    gint n_failures_in = 0;
    gint n_failures = 0;

    set_failure_on_startup = TRUE;
    g_signal_connect(test_object, "failure",
                     G_CALLBACK(cb_count_status), &n_failures);
    g_signal_connect(test_object, "failure-in",
                     G_CALLBACK(cb_count_status_in), &n_failures_in);
    cut_assert(!cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status_in),
                                         &n_failures_in);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_failures);
    cut_assert_equal_int(1, n_failures);
    cut_assert_equal_int(1, n_failures_in);
    cut_assert_equal_int(0, n_setup);
    cut_assert_equal_int(1, n_shutdown);
}

void
test_error_in_signal (void)
{
    gint n_errors_in = 0;
    gint n_errors = 0;

    set_error_on_startup = TRUE;
    g_signal_connect(test_object, "error-in",
                     G_CALLBACK(cb_count_status_in), &n_errors_in);
    g_signal_connect(test_object, "error",
                     G_CALLBACK(cb_count_status), &n_errors);
    cut_assert(!cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status_in),
                                         &n_errors_in);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_errors);
    cut_assert_equal_int(1, n_errors);
    cut_assert_equal_int(1, n_errors_in);
    cut_assert_equal_int(0, n_setup);
    cut_assert_equal_int(1, n_shutdown);
}

void
test_pending_in_signal (void)
{
    gint n_pendings_in = 0;
    gint n_pendings = 0;

    set_pending_on_startup = TRUE;
    g_signal_connect(test_object, "pending-in",
                     G_CALLBACK(cb_count_status_in), &n_pendings_in);
    g_signal_connect(test_object, "pending",
                     G_CALLBACK(cb_count_status), &n_pendings);
    cut_assert(!cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status_in),
                                         &n_pendings_in);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_pendings);
    cut_assert_equal_int(1, n_pendings);
    cut_assert_equal_int(1, n_pendings_in);
    cut_assert_equal_int(0, n_setup);
    cut_assert_equal_int(1, n_shutdown);
}

void
test_notification_in_signal (void)
{
    gint n_notifications_in = 0;
    gint n_notifications = 0;

    set_notification_on_startup = TRUE;
    g_signal_connect(test_object, "notification-in",
                     G_CALLBACK(cb_count_status_in), &n_notifications_in);
    g_signal_connect(test_object, "notification",
                     G_CALLBACK(cb_count_status), &n_notifications);
    cut_assert(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status_in),
                                         &n_notifications_in);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_notifications);
    cut_assert_equal_int(1, n_notifications);
    cut_assert_equal_int(1, n_notifications_in);
    cut_assert_equal_int(3, n_setup);
    cut_assert_equal_int(1, n_shutdown);
}

void
test_omission_in_signal (void)
{
    gint n_omissions_in = 0;
    gint n_omissions = 0;

    set_omission_on_startup = TRUE;
    g_signal_connect(test_object, "omission-in",
                     G_CALLBACK(cb_count_status_in), &n_omissions_in);
    g_signal_connect(test_object, "omission",
                     G_CALLBACK(cb_count_status), &n_omissions);
    cut_assert(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status_in),
                                         &n_omissions_in);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_omissions);
    cut_assert_equal_int(1, n_omissions);
    cut_assert_equal_int(1, n_omissions_in);
    cut_assert_equal_int(0, n_setup);
    cut_assert_equal_int(1, n_shutdown);
}

void
test_crash_in_signal (void)
{
#ifdef G_OS_WIN32
    cut_omit("crash isn't supported yet on Windows.");
#else
    gint n_crashes_in = 0;
    gint n_crashes = 0;

    cut_crash_backtrace_set_show_on_the_moment(FALSE);

    set_crash_on_startup = TRUE;
    g_signal_connect(test_object, "crash-in",
                     G_CALLBACK(cb_count_status_in), &n_crashes_in);
    g_signal_connect(test_object, "crash",
                     G_CALLBACK(cb_count_status), &n_crashes);
    cut_assert_false(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status_in),
                                         &n_crashes_in);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_crashes);
    cut_assert_equal_int(1, n_crashes);
    cut_assert_equal_int(1, n_crashes_in);
    cut_assert_equal_int(0, n_setup);
    cut_assert_equal_int(1, n_shutdown);
#endif
}

void
test_complete_signal (void)
{
    gint n_complete_tests = 0;
    g_signal_connect(test_object, "complete-test",
                     G_CALLBACK(cb_count_complete_test), &n_complete_tests);
    cut_assert(cut_test_case_run(test_object, run_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_complete_test),
                                         &n_complete_tests);
    cut_assert_equal_int(3, n_complete_tests);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
