#include "cutter.h"
#include "cut-test.h"
#include "cut-context-private.h"

void test_n_tests(void);
void test_n_assertions(void);
void test_get_function_name(void);
void test_increment_assertion_count(void);
void test_run(void);
void test_result(void);
void test_is_success(void);

static CutContext *test_context;
static CutTest *test_object;
static gboolean run_test_flag = FALSE;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    run_test_flag = TRUE;
}

static void
dummy_success_test (void)
{
}

static void
dummy_failure_test (void)
{
    cut_fail("This test should be failed");
    cut_fail("This test should be failed");
    cut_fail("This test should be failed");
    cut_fail("This test should be failed");
}

void
setup (void)
{
    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);
    test_object = cut_test_new("dummy-test", dummy_test_function);
    run_test_flag = FALSE;
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
test_n_tests (void)
{
    cut_assert(run_the_test(test_object));
    cut_assert_equal_int(1, cut_test_get_n_tests(test_object));
}

void
test_n_assertions (void)
{
    cut_assert(run_the_test(test_object));
    cut_assert_equal_int(3, cut_test_get_n_assertions(test_object));
}

void
test_increment_assertion_count (void)
{
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_assert_equal_int(5, cut_test_get_n_assertions(test_object));
}

void
test_get_function_name (void)
{
    cut_assert_equal_string("dummy-test", cut_test_get_function_name(test_object));
}

void
test_run (void)
{
    cut_assert(run_the_test(test_object));
    cut_assert(run_test_flag);
}

void
test_result (void)
{
    CutTestResult *result;

    cut_test_set_result(test_object,
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

void
test_is_success (void)
{
    CutTest *test;

    test = cut_test_new("dummy-success-test", dummy_success_test);
    cut_assert(run_the_test(test));
    cut_assert(cut_test_is_success(test));
    g_object_unref(test);

    test = cut_test_new("dummy-failure-test", dummy_failure_test);
    cut_assert(!run_the_test(test));
    cut_assert(!cut_test_is_success(test));
    g_object_unref(test);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
