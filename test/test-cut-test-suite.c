#include "cutter.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"
#include "cut-loader.h"
#include "cut-context-private.h"

void test_run_test_case (void);
void test_run_test_function (void);
void test_run_test_function_in_test_case (void);

static CutContext *test_context;
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

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);

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
    g_object_unref(test_context);
}

void
test_run_test_case (void)
{
    CutContext *original_context;
    gboolean ret;

    original_context = cut_context_get_current();

    cut_context_set_current(test_context);
    ret = cut_test_suite_run_test_case(test_object, "dummy_test_case");
    cut_context_set_current(original_context);

    cut_assert(ret);

    cut_assert(run_dummy_test_function_flag);
    cut_assert(run_dummy_run_test_function_flag);
}

void
test_run_test_function (void)
{
    CutContext *original_context;
    gboolean ret;

    original_context = cut_context_get_current();

    cut_context_set_current(test_context);
    ret = cut_test_suite_run_test_function(test_object, "run_test_function");
    cut_context_set_current(original_context);

    cut_assert(ret);

    cut_assert(run_dummy_run_test_function_flag);
}

void
test_run_test_function_in_test_case (void)
{
    CutContext *original_context;
    gboolean ret;

    original_context = cut_context_get_current();

    cut_context_set_current(test_context);
    ret = cut_test_suite_run_test_function_in_test_case(test_object,
                                                        "run_test_function",
                                                        "dummy_test_case");
    cut_context_set_current(original_context);

    cut_assert(ret);

    cut_assert(run_dummy_run_test_function_flag);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

