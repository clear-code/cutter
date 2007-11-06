#include "cutter.h"
#include "cut-test-case.h"

void test_setup(void);
void test_teardown(void);
void test_test_case_count(void);
void test_run(void);
void test_run_this_function(void);
void test_get_name(void);

static CutTestCase *test_object;

static gboolean setup_flag = FALSE;
static gboolean teardown_flag = FALSE;
static gboolean run_dummy_run_test_function_flag = FALSE;
static gboolean run_dummy_test_function_flag = FALSE;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    run_dummy_test_function_flag = TRUE;
}

static void
dummy_run_test_function (void)
{
    run_dummy_run_test_function_flag = TRUE;
}

static void
dummy_setup_function (void)
{
    run_dummy_run_test_function_flag = FALSE;
    run_dummy_test_function_flag = FALSE;
    setup_flag = TRUE;
}

static void
dummy_teardown_function (void)
{
    teardown_flag = TRUE;
}

void
setup (void)
{
    CutTest *test;
    test_object = cut_test_case_new("dummy test case",
                                    dummy_setup_function,
                                    dummy_teardown_function);
    test = cut_test_new("dummy test 1", dummy_test_function);
    cut_test_case_add_test(test_object, test);
    test = cut_test_new("dummy test 2", dummy_test_function);
    cut_test_case_add_test(test_object, test);
    test = cut_test_new("run_test_function", dummy_run_test_function);
    cut_test_case_add_test(test_object, test);
}

void
teardown (void)
{
    g_object_unref(test_object);
}

void
test_setup (void)
{
    setup_flag = FALSE;
    cut_assert(cut_test_run(CUT_TEST(test_object)));
    cut_assert(setup_flag);
}

void
test_teardown (void)
{
    teardown_flag = FALSE;
    cut_assert(cut_test_run(CUT_TEST(test_object)));
    cut_assert(teardown_flag);
}

void
test_test_case_count (void)
{
    cut_assert_equal_int(3, cut_test_case_get_test_count(test_object));
}

void
test_run (void)
{
    cut_assert(cut_test_run(CUT_TEST(test_object)));
    cut_assert(run_dummy_run_test_function_flag);
    cut_assert(run_dummy_test_function_flag);
}

void
test_run_this_function (void)
{
    cut_assert(cut_test_case_run_function(test_object, "run_test_function"));
    cut_assert(run_dummy_run_test_function_flag);
    cut_assert(!run_dummy_test_function_flag);
}

void
test_get_name (void)
{
    cut_assert_equal_string("dummy test case", cut_test_case_get_name(test_object));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

