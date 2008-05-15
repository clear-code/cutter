#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "cutter.h"
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-utils.h"
#include "cut-run-context.h"
#include "cuttest-assertions.h"

#include <glib/gstdio.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

void test_equal_int(void);
void test_equal_string(void);
void test_equal_string_with_diff(void);
void test_equal_double(void);
void test_operator(void);
void test_equal_string_array (void);
void test_null(void);
void test_null_string(void);
void test_equal_string_with_free(void);
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
void test_assert_errno (void);
void test_omit (void);
void test_file_exist (void);
void test_match (void);

static gboolean need_cleanup;
static gboolean compare_function_is_called;
static gchar *tmp_file_name;

static CutTest *test_object;
static CutRunContext *run_context;
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
dummy_pend_test_function (void)
{
    cut_pend("This test has been pending ever!");
}

static void
dummy_notify_test_function (void)
{
    cut_notify("This test has been notifying ever!");
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

    run_context = cut_run_context_new();

    test_context = cut_test_context_new(NULL, NULL, test_object);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test_object, test_context, run_context);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    need_cleanup = FALSE;
    compare_function_is_called = FALSE;
    tmp_file_name = NULL;
    test_object = NULL;
    run_context = NULL;
    test_context = NULL;
    test_result = NULL;
}

void
teardown (void)
{
    if (tmp_file_name) {
        g_remove(tmp_file_name);
        g_free(tmp_file_name);
    }

    if (!need_cleanup)
        return;

    if (test_object)
        g_object_unref(test_object);
    if (run_context)
        g_object_unref(run_context);
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

static void
equal_string_with_diff (void)
{
    cut_assert_equal_string("abc def ghi jkl",
                            "abc DEF ghi jkl");
}

void
test_equal_string_with_diff (void)
{
    CutTest *test;

    test = cut_test_new("assert-equal-string-with-diff",
                        equal_string_with_diff);
    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_equal_string("<\"abc def ghi jkl\" == \"abc DEF ghi jkl\">\n"
                            "expected: <abc def ghi jkl>\n"
                            " but was: <abc DEF ghi jkl>\n"
                            "\n"
                            "diff:\n"
                            "- abc def ghi jkl\n"
                            "?     ^^^\n"
                            "+ abc DEF ghi jkl\n"
                            "?     ^^^",
                            cut_test_result_get_system_message(test_result));
}

void
test_equal_double (void)
{
    cut_assert_equal_double(1.0, 0.1, 1.0);
    cut_assert_equal_double(1.01, 0.01, 1.01);
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
    gchar *strings1[] = {"a", "b", "c", NULL};
    gchar *strings2[] = {"a", "b", "c", NULL};

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

    test = cut_test_new("error-equal-string-with-null",
                        error_equal_string_with_null);
    cut_assert(test, "Creating a new CutTest object failed");

    cut_assert(!run(test));
}

void
test_error_equal_string (void)
{
    CutTest *test;

    test = cut_test_new("error-equal-string",
                        error_equal_string);
    cut_assert(test, "Creating a new CutTest object failed");

    cut_assert(!run(test));
}

void
test_error (void)
{
    CutTest *test;

    test = cut_test_new("dummy-error-test", dummy_error_test_function);
    cut_assert(test, "Creating a new CutTest object failed");

    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR, "dummy-error-test",
                           "This test should error", NULL,
                           "dummy_error_test_function");
}

void
test_pending (void)
{
    CutTest *test;

    test = cut_test_new("dummy-pend-test", dummy_pend_test_function);
    cut_assert(test, "Creating a new CutTest object failed");

    g_signal_connect(test, "pending", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test), "cut_pend() did not return FALSE!");
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

    test = cut_test_new("dummy-notify-test", dummy_notify_test_function);
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

    test = cut_test_new("dummy-fail-test", dummy_fail_test_function);
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

    test = cut_test_new("dummy-assertion-message-test",
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

    test = cut_test_new("dummy-assert-message-with-string",
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
    cut_assert_equal_string("<\"%s\" == \"%d\">\n"
                            "expected: <%s>\n"
                            " but was: <%d>",
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
    const GList *results;

    test = cut_test_new("fail from nested function",
                        just_call_fail_in_nested_function);
    cut_assert(test);

    cut_assert(!run(test));

    cut_assert(run_context);
    cut_assert(1, cut_run_context_get_n_tests(run_context));
    cut_assert(1, cut_run_context_get_n_assertions(run_context));
    results = cut_run_context_get_results(run_context);
    cut_assert(1, g_list_length((GList *)results));
    cut_assert(1, cut_run_context_get_n_failures(run_context));

    result = cut_run_context_get_results(run_context)->data;
    cut_assert_equal_string("Fail from nested function",
                            cut_test_result_get_user_message(result));
}

static void
null_string_assertions (void)
{
    cut_assert_null_string(NULL);
    cut_assert_null_string("");
}

void
test_null_string (void)
{
    CutTest *test;

    test = cut_test_new("assert-null-string", null_string_assertions);
    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 1, 1, 1, 0, 0, 0, 0);
}

static void
equal_string_with_free_assertions (void)
{
    cut_assert_equal_string_with_free(NULL, NULL);
    cut_assert_equal_string_with_free("", g_strdup(""));
    cut_assert_equal_string_with_free("xxx", g_strdup("xxx"));
    cut_assert_equal_string_with_free("xxx", g_strdup("zzz"));
}

void
test_equal_string_with_free (void)
{
    CutTest *test;

    test = cut_test_new("assert-string-equal-string-with-free",
                        equal_string_with_free_assertions);
    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 1, 3, 1, 0, 0, 0, 0);
}

static void
assert_errno_for_eacces (void)
{
    errno = 0;
    cut_assert_errno("Passed");

    errno = EACCES;
    cut_assert_errno("Failed");
}

void
test_assert_errno (void)
{
    CutTest *test;

    test = cut_test_new("assert-errno-for-eacces", assert_errno_for_eacces);
    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 1, 1, 1, 0, 0, 0, 0);
}

static void
omit_test (void)
{
    cut_assert_equal_int(1, 1);
    cut_omit("Omit the following tests");
    cut_assert_equal_int(2, 2);
}

void
test_omit (void)
{
    CutTest *test;

    test = cut_test_new("omit-test", omit_test);
    cut_assert(run(test));
    cut_assert_test_result_summary(run_context, 1, 1, 0, 0, 0, 0, 1);
}

static void
file_exist_test (void)
{
    gint fd;
    GError *error = NULL;

    fd = g_file_open_tmp(NULL, &tmp_file_name, &error);
    if (fd == -1) {
        const gchar *message;
        message = cut_take_string(g_strdup(error->message));
        g_error_free(error);
        cut_error("can't create temporary file: %s", message);
    }
    close(fd);

    cut_assert_file_exist(tmp_file_name);
    g_remove(tmp_file_name);
    cut_assert_file_exist(tmp_file_name);
}

void
test_file_exist (void)
{
    CutTest *test;

    test = cut_test_new("file-exist-test", file_exist_test);
    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 1, 1, 1, 0, 0, 0, 0);
}

static void
match_test (void)
{
    cut_assert_match("^abc", "abc");
    cut_assert_match("^abc", " abc", "FAILED");
}

void
test_match (void)
{
    CutTest *test;

    test = cut_test_new("match-test", match_test);
    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 1, 1, 1, 0, 0, 0, 0);
}


/*
vi:nowrap:ai:expandtab:sw=4
*/
