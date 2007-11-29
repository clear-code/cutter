#include "cutter.h"
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-utils.h"
#include "cut-runner.h"

void test_equal_int(void);
void test_equal_string(void);
void test_equal_double(void);
void test_operator(void);
void test_equal_string_array (void);
void test_null(void);
void test_error(void);
void test_fail(void);
void test_pending(void);
void test_notification(void);
void test_assert_message(void);
void test_assert_message_with_format_string(void);
void test_error_equal_string (void);
void test_error_equal_string_with_null (void);
void test_assert_equal_function (void);
void test_failure_from_nested_function (void);

static gboolean need_cleanup;
static gboolean compare_function_is_called;

static CutTest *test_object;
static CutRunner *runner;
static CutTestContext *test_context;
static CutTestResult *test_result;

static void
error_equal_string (void)
{
    cut_assert_equal_string("a", "ab");
}

static void
error_equal_string_with_null (void)
{
    cut_assert_equal_string("", NULL);
}

static void
dummy_error_test_function (void)
{
    cut_error("This test should error");
}

static void
dummy_fail_test_function (void)
{
    cut_fail("This test should fail");
}

static void
dummy_pending_test_function (void)
{
    cut_pending("This test has been pending ever!");
}

static void
dummy_notify_test_function (void)
{
    cut_notify("This test has been notify ever!");
}

static void
dummy_assert_message_test_function (void)
{
    cut_assert(FALSE, "The message of %s", "assertion");
}

static void
dummy_assert_message_with_format_string (void)
{
    cut_assert_equal_string("%s", "%d",
                            "%s and %s have format string",
                            "expected", "actual");
}

static void
cb_collect_result (CutTest *test, CutTestContext *test_context,
                   CutTestResult *result, CutTestResult **output)
{
    *output = result;
    g_object_ref(*output);
}

static gboolean
run (CutTest *test)
{
    gboolean success;
    CutTestContext *original_test_context;

    need_cleanup = TRUE;

    test_object = test;

    runner = cut_runner_new();

    test_context = cut_test_context_new(NULL, NULL, test_object);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test_object, test_context, runner);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    need_cleanup = FALSE;
    compare_function_is_called = FALSE;
    test_object = NULL;
    runner = NULL;
    test_context = NULL;
    test_result = NULL;
}

void
teardown (void)
{
    if (!need_cleanup)
        return;

    if (test_object)
        g_object_unref(test_object);
    if (runner)
        g_object_unref(runner);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);
}

void
test_equal_int (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(-1, -1);
    cut_assert_equal_int(0, 0);
}

void
test_equal_string (void)
{
    cut_assert_equal_string("", "");
    cut_assert_equal_string("a", "a");
}

void
test_equal_double (void)
{
    cut_assert_equal_double(1.0, 0.1, 1.0);
    cut_assert_equal_double(1.0, 0.01, 1.01);
}

void
test_operator (void)
{
    cut_assert_operator(1, <, 2);
    cut_assert_operator(1, <=, 1);
    cut_assert_operator(1, >=, 1);
    cut_assert_operator(2, >, 1);
    cut_assert_operator(NULL, ==, NULL);
    cut_assert_operator(TRUE, ||, FALSE);
    cut_assert_operator(TRUE, &&, TRUE);
}

void
test_equal_string_array (void)
{
    const gchar *strings1[] = {"a", "b", "c", NULL};
    const gchar *strings2[] = {"a", "b", "c", NULL};

    cut_assert_equal_string_array(strings1, strings2);
}

void
test_null (void)
{
    cut_assert_null(NULL);
}

void
test_error_equal_string_with_null (void)
{
    CutTest *test;

    test = cut_test_new("error-equal-string-with-null", NULL,
                        error_equal_string_with_null);
    cut_assert(test, "Creating a new CutTest object failed");

    cut_assert(!run(test));
}

void
test_error_equal_string (void)
{
    CutTest *test;

    test = cut_test_new("error-equal-string", NULL,
                        error_equal_string);
    cut_assert(test, "Creating a new CutTest object failed");

    cut_assert(!run(test));
}

static void
cut_assert_test_result_summary (gint n_tests, gint n_assertions,
                                gint n_failures, gint n_errors,
                                gint n_pendings, gint n_notifications)
{
    cut_assert_equal_int(n_tests, cut_runner_get_n_tests(runner));
    cut_assert_equal_int(n_assertions, cut_runner_get_n_assertions(runner));
    cut_assert_equal_int(n_failures, cut_runner_get_n_failures(runner));
    cut_assert_equal_int(n_errors, cut_runner_get_n_errors(runner));
    cut_assert_equal_int(n_pendings, cut_runner_get_n_pendings(runner));
    cut_assert_equal_int(n_notifications,
                         cut_runner_get_n_notifications(runner));
}

static void
cut_assert_test_result (gint i, CutTestResultStatus status,
                        const gchar *test_name,
                        const gchar *user_message, const gchar *system_message,
                        const gchar *function_name)
{
    const GList *results;
    CutTestResult *result;

    results = cut_runner_get_results(runner);
    cut_assert_operator_int(i, <, g_list_length((GList *)results));

    result = g_list_nth_data((GList *)results, i);
    cut_assert(result);
    cut_assert_equal_int(status, cut_test_result_get_status(result));
    cut_assert_equal_string(test_name, cut_test_result_get_test_name(result));
    cut_assert_equal_string_or_null(user_message,
                                    cut_test_result_get_user_message(result));
    cut_assert_equal_string_or_null(system_message,
                                    cut_test_result_get_system_message(result));
    cut_assert_equal_string(function_name,
                            cut_test_result_get_function_name(result));
}

void
test_error (void)
{
    CutTest *test;

    test = cut_test_new("dummy-error-test", NULL, dummy_error_test_function);
    cut_assert(test, "Creating a new CutTest object failed");

    cut_assert(!run(test));
    cut_assert_test_result_summary(1, 0, 0, 1, 0, 0);
    cut_assert_test_result(0, CUT_TEST_RESULT_ERROR, "dummy-error-test",
                           "This test should error", NULL,
                           "dummy_error_test_function");
}

void
test_pending (void)
{
    CutTest *test;

    test = cut_test_new("dummy-pending-test", NULL, dummy_pending_test_function);
    cut_assert(test, "Creating a new CutTest object failed");

    g_signal_connect(test, "pending", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test), "cut_pending() did not return FALSE!");
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"pending\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(test_result));
}

void
test_notification (void)
{
    CutTest *test;

    test = cut_test_new("dummy-notify-test", NULL, dummy_notify_test_function);
    cut_assert(test, "Creating a new CutTest object failed");

    g_signal_connect(test, "notification", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(run(test), "cut_notify() did return FALSE!");
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"notification\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_NOTIFICATION,
                         cut_test_result_get_status(test_result));
}

void
test_fail (void)
{
    CutTest *test;

    test = cut_test_new("dummy-fail-test", NULL, dummy_fail_test_function);
    cut_assert(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"failure\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(test_result));
}

void
test_assert_message (void)
{
    CutTest *test;

    test = cut_test_new("dummy-assertion-message-test", NULL,
                        dummy_assert_message_test_function);
    cut_assert(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"failure\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(test_result));
    cut_assert_equal_string("The message of assertion",
                            cut_test_result_get_user_message(test_result));
}

void
test_assert_message_with_format_string (void)
{
    CutTest *test;

    test = cut_test_new("dummy-assert-message-with-string", NULL,
                        dummy_assert_message_with_format_string);
    cut_assert(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"failure\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(test_result));
    cut_assert_equal_string("<\"%s\" == \"%d\">\nexpected: <%s>\n but was: <%d>",
                            cut_test_result_get_system_message(test_result));
    cut_assert_equal_string("expected and actual have format string",
                            cut_test_result_get_user_message(test_result));
}

static gboolean
compare_function (gpointer a, gpointer b)
{
    compare_function_is_called = TRUE;
    return TRUE;
}

void
test_assert_equal_function (void)
{
    cut_assert(!compare_function_is_called);
    cut_assert_equal(compare_function, "o", "o");
    cut_assert(compare_function_is_called);
}

static gboolean
fail_in_nested_function (void)
{
    cut_fail("Fail from nested function");

    return FALSE;
}

static void
just_call_fail_in_nested_function (void)
{
    fail_in_nested_function();
    cut_fail("Never here");
}

void
test_failure_from_nested_function (void)
{
    CutTest *test;
    CutTestResult *result;

    test = cut_test_new("fail from nested function", NULL,
                        just_call_fail_in_nested_function);
    cut_assert(test);

    cut_assert(!run(test));

    cut_assert(runner);
    cut_assert(1, cut_runner_get_n_tests(runner));
    cut_assert(1, cut_runner_get_n_assertions(runner));
    cut_assert(1, g_list_length((GList *)cut_runner_get_results(runner)));
    cut_assert(1, cut_runner_get_n_failures(runner));

    result = cut_runner_get_results(runner)->data;
    cut_assert_equal_string("Fail from nested function",
                            cut_test_result_get_user_message(result));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

