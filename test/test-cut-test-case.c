#include <cutter.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-runner.h>

#include "cuttest-utils.h"

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
void test_complete_signal(void);

static CutTestCase *test_object;
static CutRunner *runner;

static gboolean set_error_on_setup = FALSE;

static gint n_setup = 0;
static gint n_teardown = 0;
static gint n_run_dummy_run_test_function = 0;
static gint n_run_dummy_test_function1 = 0;
static gint n_run_dummy_test_function2 = 0;

static void
dummy_test_function1 (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    n_run_dummy_test_function1++;
}

static void
dummy_test_function2 (void)
{
    n_run_dummy_test_function2++;
}

static void
dummy_run_test_function (void)
{
    n_run_dummy_run_test_function++;
}

static void
dummy_failure_test (void)
{
    cut_fail("Failed!!!");
}

static void
dummy_error_test (void)
{
    cut_error("Error!!!");
}

static void
dummy_pending_test (void)
{
    cut_pend("Pend!!!");
}

static void
dummy_notification_test (void)
{
    cut_notify("Notify!!!");
}

static void
dummy_setup_function (void)
{
    if (set_error_on_setup)
        cut_error("Error in setup");
    n_setup++;
}

static void
dummy_teardown_function (void)
{
    n_teardown++;
}

void
setup (void)
{
    const gchar *test_names[] = {"/.*/", NULL};

    set_error_on_setup = FALSE;

    n_setup = 0;
    n_teardown = 0;
    n_run_dummy_run_test_function = 0;
    n_run_dummy_test_function1 = 0;
    n_run_dummy_test_function2 = 0;

    runner = cut_runner_new();
    cut_runner_set_target_test_names(runner, test_names);

    test_object = cut_test_case_new("dummy test case",
                                    dummy_setup_function,
                                    dummy_teardown_function,
                                    get_current_test_context,
                                    set_current_test_context,
                                    NULL, NULL);
    cuttest_add_test(test_object, "dummy_test_1", dummy_test_function1);
    cuttest_add_test(test_object, "dummy_test_2", dummy_test_function2);
    cuttest_add_test(test_object, "run_test_function", dummy_run_test_function);
}

void
teardown (void)
{
    g_object_unref(test_object);
    g_object_unref(runner);
}

static void
cb_count_around_test (CutTestCase *test_case, CutTest *test,
                      CutTestContext *test_context, gpointer data)
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

static gboolean
run_the_test (void)
{
    return cut_test_case_run(test_object, runner);
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
    const gchar *test_names[] = {"/.*/", NULL};
    cut_assert_equal_int(3, cut_test_case_get_n_tests(test_object, NULL));
    cut_assert_equal_int(3, cut_test_case_get_n_tests(test_object, test_names));
}

void
test_run (void)
{
    cut_assert(run_the_test());
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
}

void
test_run_with_setup_error (void)
{
    set_error_on_setup = TRUE;
    cut_assert(!run_the_test());
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
}

void
test_run_this_function (void)
{
    cut_assert(cut_test_case_run_test(test_object, runner,
                                      "run_test_function"));

    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_dummy_test_function1);
}

void
test_run_tests_with_regex (void)
{
    cut_assert(cut_test_case_run_test(test_object, runner, "/dummy/"));
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
}

void
test_run_with_name_filter (void)
{
    const gchar *names[] = {"dummy_test_1", "run_test_function", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, runner, names));
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_test_function2);
}

void
test_run_with_regex_filter (void)
{
    const gchar *regex[] = {"/dummy/", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, runner, regex));
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
}

void
test_run_with_name_and_regex_filter (void)
{
    const gchar *name_and_regex[] = {"/dummy/", "run_test_function", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, runner,
                                             name_and_regex));
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
}

void
test_get_name (void)
{
    cut_assert_equal_string("dummy test case",
                            cut_test_get_name(CUT_TEST(test_object)));
}

void
test_start_signal (void)
{
    gint n_start_tests = 0;
    g_signal_connect(test_object, "start-test",
                     G_CALLBACK(cb_count_around_test), &n_start_tests);
    cut_assert(cut_test_case_run(test_object, runner));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_around_test),
                                         &n_start_tests);
    cut_assert_equal_int(3, n_start_tests);
}

void
test_success_signal (void)
{
    gint n_successes = 0;
    g_signal_connect(test_object, "success",
                     G_CALLBACK(cb_count_status), &n_successes);
    cut_assert(cut_test_case_run(test_object, runner));
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
    cuttest_add_test(test_object, "dummy_failure_test", dummy_failure_test);
    cut_assert(!cut_test_case_run(test_object, runner));
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
    cuttest_add_test(test_object, "dummy_error_test", dummy_error_test);
    cut_assert(!cut_test_case_run(test_object, runner));
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
    cuttest_add_test(test_object, "dummy_pending_test", dummy_pending_test);
    cut_assert(!cut_test_case_run(test_object, runner));
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
    cuttest_add_test(test_object, "dummy_notification_test",
                     dummy_notification_test);
    cut_assert(cut_test_case_run(test_object, runner));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_status),
                                         &n_notifications);
    cut_assert_equal_int(1, n_notifications);
}

void
test_complete_signal (void)
{
    gint n_complete_tests = 0;
    g_signal_connect(test_object, "complete-test",
                     G_CALLBACK(cb_count_around_test), &n_complete_tests);
    cut_assert(cut_test_case_run(test_object, runner));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_count_around_test),
                                         &n_complete_tests);
    cut_assert_equal_int(3, n_complete_tests);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
