#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib/gstdio.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <errno.h>

#include <cutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-utils.h>
#include <cutter/cut-test-runner.h>
#include "lib/cuttest-assertions.h"

void test_equal_int(void);
void test_equal_string(void);
void test_equal_string_with_diff(void);
void test_equal_string_with_folded_diff(void);
void test_equal_double(void);
void test_operator(void);
void test_operator_int(void);
void test_operator_double(void);
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
void test_path_exist (void);
void test_path_not_exist (void);
void test_match (void);
void test_equal_pointer (void);
void test_equal_fixture_data_string (void);
void test_equal_fixture_data_string_without_file (void);
void test_error_errno (void);

static gboolean compare_function_is_called;
static gchar *tmp_file_name;

static CutTest *test;
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
stub_error_test_function (void)
{
    cut_error("This test should error");
}

static void
stub_fail_test_function (void)
{
    cut_fail("This test should fail");
}

static void
stub_pend_test_function (void)
{
    cut_pend("This test has been pending ever!");
}

static void
stub_notify_test_function (void)
{
    cut_notify("This test has been notifying ever!");
}

static void
stub_assert_message_test_function (void)
{
    cut_assert(FALSE, "The message of %s", "assertion");
}

static void
stub_assert_message_with_format_string (void)
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
run (void)
{
    gboolean success;
    CutTestContext *original_test_context;

    cut_assert_not_null(test);

    run_context = cut_test_runner_new();

    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_set_fixture_data_dir(test_context,
                                          cuttest_get_base_dir(),
                                          "fixtures", "assertions", NULL);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    compare_function_is_called = FALSE;
    tmp_file_name = NULL;
    test = NULL;
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

    if (test)
        g_object_unref(test);
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
    test = cut_test_new("assert-equal-string-with-diff",
                        equal_string_with_diff);
    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 0, 0, 1, 0, 0, 0, 0);
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

static void
equal_string_with_folded_diff (void)
{
    cut_assert_equal_string("0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789",

                            "0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789");
}

void
test_equal_string_with_folded_diff (void)
{
    test = cut_test_new("assert-equal-string-with-folded-diff",
                        equal_string_with_folded_diff);
    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_equal_string("<"
                            "\"0123456789\" "
                            "\"1123456789\" "
                            "\"2123456789\" "
                            "\"3123456789\" "
                            "\"4123456789\" "
                            "\"5123456789\" "
                            "\"6123456789\" "
                            "\"7123456789\" "
                            "\"8123456789\""
                            " == "
                            "\"0000000000\" "
                            "\"1123456789\" "
                            "\"2123456789\" "
                            "\"3123456789\" "
                            "\"4123456789\" "
                            "\"5123456789\" "
                            "\"6123456789\" "
                            "\"7123456789\" "
                            "\"8123456789\""
                            ">\n"
                            "expected: <"
                            "0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789"
                            ">\n"
                            " but was: <"
                            "0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789"
                            ">\n"
                            "\n"
                            "diff:\n"
                            "- 0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789\n"
                            "?  ^^^^^^^^^\n"
                            "+ 0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789\n"
                            "?  ^^^^^^^^^\n"
                            "\n"
                            "folded diff:\n"
                            "- 0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "71234567\n"
                            "?  ^^^^^^^^^\n"
                            "+ 0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "71234567\n"
                            "?  ^^^^^^^^^\n"
                            "  89"
                            "8123456789",
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

static void
stub_operator_int (void)
{
    cut_assert_operator_int(1, <, 2 + 3);
    cut_assert_operator_int(2, ==, 1 + 1);
    cut_assert_operator_int(1 + 1, >=, 2 + 4);
}

void
test_operator_int (void)
{
    test = cut_test_new("stub-operator-int", stub_operator_int);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-operator-int", NULL,
                           "expected: <1 + 1> >= <2 + 4>\n"
                           " but was: <2> >= <6>",
                           "stub_operator_int");
}

static void
stub_operator_double (void)
{
    cut_assert_operator_double(1.2, <, 2.2 + 3.4);
    cut_assert_operator_double(1.1 + 1.1, >=, 2.2 + 4.4);
}

void
test_operator_double (void)
{
    test = cut_test_new("stub-operator-double", stub_operator_double);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-operator-double", NULL,
                           "expected: <1.1 + 1.1> >= <2.2 + 4.4>\n"
                           " but was: <2.2> >= <6.6>",
                           "stub_operator_double");
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
    test = cut_test_new("error-equal-string-with-null",
                        error_equal_string_with_null);
    cut_assert_not_null(test);
    cut_assert_false(run());
}

void
test_error_equal_string (void)
{
    test = cut_test_new("error-equal-string", error_equal_string);
    cut_assert_not_null(test);
    cut_assert_false(run());
}

void
test_error (void)
{
    test = cut_test_new("stub-error-test", stub_error_test_function);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR,
                           "stub-error-test",
                           "This test should error", NULL,
                           "stub_error_test_function");
}

void
test_pending (void)
{
    test = cut_test_new("stub-pend-test", stub_pend_test_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "pending", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run(), "cut_pend() did not return FALSE!");
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert_not_null(test_result,
                        "Could not get a CutTestResult object "
                        "since \"pending\" signal was not emitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(test_result));
}

void
test_notification (void)
{
    test = cut_test_new("stub-notify-test", stub_notify_test_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "notification", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_true(run(), "cut_notify() did return FALSE!");
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert_not_null(test_result,
                        "Could not get a CutTestResult object "
                        "since \"notification\" signal was not emitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_NOTIFICATION,
                         cut_test_result_get_status(test_result));
}

void
test_fail (void)
{
    test = cut_test_new("stub-fail-test", stub_fail_test_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert_not_null(test_result,
                        "Could not get a CutTestResult object "
                        "since \"failure\" signal was not emitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(test_result));
}

void
test_assert_message (void)
{
    test = cut_test_new("stub-assertion-message-test",
                        stub_assert_message_test_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert_not_null(test_result,
                        "Could not get a CutTestResult object "
                        "since \"failure\" signal was not emitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(test_result));
    cut_assert_equal_string("The message of assertion",
                            cut_test_result_get_user_message(test_result));
}

void
test_assert_message_with_format_string (void)
{
    test = cut_test_new("stub-assert-message-with-string",
                        stub_assert_message_with_format_string);
    cut_assert_not_null(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert_not_null(test_result,
                        "Could not get a CutTestResult object "
                        "since \"failure\" signal was not emitted.");
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
    cut_assert_false(compare_function_is_called);
    cut_assert_equal(compare_function, "o", "o");
    cut_assert_true(compare_function_is_called);
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
    CutTestResult *result;

    test = cut_test_new("fail from nested function",
                        just_call_fail_in_nested_function);
    cut_assert_not_null(test);

    cut_assert_false(run());

    cut_assert_not_null(run_context);
    cut_assert_test_result_summary(run_context, 0, 0, 0, 1, 0, 0, 0, 0);

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
    test = cut_test_new("assert-null-string", null_string_assertions);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
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
    test = cut_test_new("assert-string-equal-string-with-free",
                        equal_string_with_free_assertions);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 3, 0, 1, 0, 0, 0, 0);
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
    test = cut_test_new("assert-errno-for-eacces", assert_errno_for_eacces);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
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
    test = cut_test_new("omit-test", omit_test);
    cut_assert_true(run());
    cut_assert_test_result_summary(run_context, 0, 1, 1, 0, 0, 0, 0, 1);
}

static void
path_exist_test (void)
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

    cut_assert_path_exist(tmp_file_name);
    g_remove(tmp_file_name);
    cut_assert_path_exist(tmp_file_name);
}

void
test_path_exist (void)
{
    test = cut_test_new("path-exist-test", path_exist_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
}

static void
path_not_exist_test (void)
{
    gint fd;
    GError *error = NULL;

    fd = g_file_open_tmp(NULL, &tmp_file_name, &error);
    cut_assert_g_error(error);
    close(fd);

    g_remove(tmp_file_name);
    cut_assert_path_not_exist(tmp_file_name);

    g_file_set_contents(tmp_file_name, "XXX", -1, &error);
    cut_assert_g_error(error);
    cut_assert_path_not_exist(tmp_file_name);
}

void
test_path_not_exist (void)
{
    test = cut_test_new("path-not-exist-test", path_not_exist_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 3, 0, 1, 0, 0, 0, 0);
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
    test = cut_test_new("match-test", match_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
}

static void
equal_pointer_test (void)
{
    int int1, int2;

    cut_assert_equal_pointer(&int1, &int1);
    cut_assert_equal_pointer(&int1, &int2);
}

void
test_equal_pointer (void)
{
    test = cut_test_new("equal-pointer-test", equal_pointer_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
}

static void
equal_fixture_data_string_test (void)
{
    cut_assert_equal_fixture_data_string("top level data\n", "data.txt");
    cut_assert_equal_fixture_data_string("sub level data\n", "sub", "data.txt");
    cut_assert_equal_fixture_data_string("wrong data\n", "sub", "data.txt");
}

void
test_equal_fixture_data_string (void)
{
    test = cut_test_new("equal-fixture-data-sting-test",
                        equal_fixture_data_string_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
}

static void
equal_fixture_data_string_test_without_file (void)
{
    cut_assert_equal_fixture_data_string("NONE!", "nonexistent.txt");
}

void
test_equal_fixture_data_string_without_file (void)
{
    test = cut_test_new("equal-fixture-data-sting-test-without-file",
                        equal_fixture_data_string_test_without_file);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 0, 0, 0, 1, 0, 0, 0);
}

static void
error_errno (void)
{
    errno = 0;
    cut_error_errno("Not error");

    errno = EACCES;
    cut_error_errno("Should error");
}

void
test_error_errno (void)
{
    CutTestResult *result;

    test = cut_test_new("error-errno", error_errno);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 0, 0, 0, 1, 0, 0, 0);

    result = cut_run_context_get_results(run_context)->data;
    cut_assert_equal_string("Should error",
                            cut_test_result_get_user_message(result));
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
