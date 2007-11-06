#include "cutter.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"
#include "cut-loader.h"

void test_run_test_case (void);

static CutTestSuite *test_object;
static CutLoader *loader;

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

void
setup (void)
{
    CutTestCase *test_case;
    CutTest *test;
    test_object = cut_test_suite_new();

    loader = cut_loader_new("loader_test_dir/.libs/libdummy_loader_test.so");
    test_case = cut_loader_load_test_case(loader);
    cut_test_suite_add_test_case(test_object, test_case);

    test_case = cut_test_case_new("dummy_test_case", NULL, NULL);
    test = cut_test_new("dummy test 1", dummy_test_function);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("run_test_function", dummy_run_test_function);
    cut_test_case_add_test(test_case, test);

    cut_test_suite_add_test_case(test_object, test_case);
}

void
teardown (void)
{
    g_object_unref(loader);
    g_object_unref(test_object);
}

void
test_run_test_case (void)
{
    cut_assert(cut_test_suite_run_test_case(test_object, "dummy_test_case"));
    cut_assert(run_dummy_test_function_flag);
    cut_assert(run_dummy_run_test_function_flag);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

