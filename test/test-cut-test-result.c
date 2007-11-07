#include "cutter.h"
#include "cut-test-result.h"
#include "cut-context-private.h"

void test_result(void);

static CutContext *test_context;
static CutTest *test_object;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
}

void
setup (void)
{
    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);
    test_object = cut_test_new("dummy-test", dummy_test_function);
}

void
teardown (void)
{
    g_object_unref(test_object);
    g_object_unref(test_context);
}

static gboolean
run_the_test (CutTest *test)
{
    return cut_test_run(test, test_context);
}
void
test_result (void)
{
    CutTestResult *result;

    cut_test_result_new(test_object,
                        CUT_TEST_RESULT_FAILURE,
                        "result-message",
                        "function-name",
                        "filename",
                        999);
    result = cut_test_get_result(test_object);

    cut_assert(result);
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(result));
    cut_assert_equal_string("result-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("function-name",
                            cut_test_result_get_function_name(result));
    cut_assert_equal_string("filename",
                            cut_test_result_get_filename(result));
    cut_assert_equal_int(999, cut_test_result_get_line(result));

    cut_test_set_result(test_object,
                        CUT_TEST_RESULT_ERROR,
                        "result-message",
                        "function-name",
                        "filename",
                        999);
    result = cut_test_get_result(test_object);

    cut_assert(result);
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));

    cut_test_set_result(test_object,
                        CUT_TEST_RESULT_PENDING,
                        "result-message",
                        "function-name",
                        "filename",
                        999);
    result = cut_test_get_result(test_object);

    cut_assert(result);
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(result));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
