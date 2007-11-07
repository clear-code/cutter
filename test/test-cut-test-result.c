#include "cutter.h"
#include "cut-test-result.h"

void test_get_status(void);

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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
