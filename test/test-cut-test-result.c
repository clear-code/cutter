#include "cutter.h"
#include "cut-test-result.h"

void test_get_status(void);
void test_get_message(void);
void test_get_function_name(void);
void test_get_filename(void);
void test_get_line(void);

void
test_get_status (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS, cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE, cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_ERROR,
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR, cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_PENDING,
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING, cut_test_result_get_status(result));
    g_object_unref(result);
}

void
test_get_message (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("result-message", cut_test_result_get_message(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL,
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert(cut_test_result_get_message(result) == NULL,
               "cut_test_result_get_message() should return NULL!");
    g_object_unref(result);
}

void
test_get_function_name (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("function-name", cut_test_result_get_function_name(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
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
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("filename", cut_test_result_get_filename(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
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
                                 "result-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(999, cut_test_result_get_line(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL,
                                 NULL,
                                 NULL,
                                 -1);
    cut_assert_equal_int(-1, cut_test_result_get_line(result)); /* this test should be failed?*/
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL,
                                 NULL,
                                 NULL,
                                 G_MAXUINT);
    cut_assert_equal_int(G_MAXUINT, cut_test_result_get_line(result));
    g_object_unref(result);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
