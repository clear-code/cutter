#include "cutter.h"
#include "cut-test-result.h"

void test_get_status(void);
void test_get_message_full(void);
void test_get_message_only_user(void);
void test_get_message_only_system(void);
void test_get_message_none(void);
void test_get_function_name(void);
void test_get_filename(void);
void test_get_line(void);

void
test_get_status (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_ERROR,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_PENDING,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(result));
    g_object_unref(result);
}

void
test_get_message_full (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("user-message\nsystem-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));
    g_object_unref(result);
}

void
test_get_message_only_user (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 NULL,
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("user-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));
    cut_assert(cut_test_result_get_system_message(result) == NULL);
    g_object_unref(result);
}

void
test_get_message_only_system (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 NULL,
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("system-message",
                            cut_test_result_get_message(result));
    cut_assert(cut_test_result_get_user_message(result) == NULL);
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));
    g_object_unref(result);
}

void
test_get_message_none (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 NULL,
                                 NULL,
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert(cut_test_result_get_message(result) == NULL,
               "cut_test_result_get_message() should return NULL!");
    cut_assert(cut_test_result_get_user_message(result) == NULL,
               "cut_test_result_get_user_message() should return NULL!");
    cut_assert(cut_test_result_get_system_message(result) == NULL,
               "cut_test_result_get_system_message() should return NULL!");
    g_object_unref(result);
}

void
test_get_function_name (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("function-name",
                            cut_test_result_get_function_name(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 NULL,
                                 NULL,
                                 NULL,
                                 "filename",
                                 999);
    cut_assert(cut_test_result_get_function_name(result) == NULL,
               "cut_test_result_get_function_name() should return NULL!");
    g_object_unref(result);
}

void
test_get_filename (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("filename", cut_test_result_get_filename(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 999);
    cut_assert(cut_test_result_get_filename(result) == NULL,
               "cut_test_result_get_filename() should return NULL!");
    g_object_unref(result);
}

void
test_get_line (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(999, cut_test_result_get_line(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 0);
    cut_assert_equal_int(0, cut_test_result_get_line(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test-name",
                                 "test-case-name",
                                 "test-suite-name",
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 G_MAXUINT);
    cut_assert_equal_int(G_MAXUINT, cut_test_result_get_line(result));
    g_object_unref(result);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
