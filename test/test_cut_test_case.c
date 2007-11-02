#include "cut-test-case.h"
#include "cut-assertion.h"

static CutTestCase *test_object;

static gboolean setup_flag = FALSE;
static gboolean teardown_flag = FALSE;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
}

static void
dummy_setup_function (void)
{
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
    test_object = cut_test_case_new(dummy_setup_function,
                                    dummy_teardown_function);
    test = cut_test_new(dummy_test_function);
    cut_test_case_add_test(test_object, test);
    test = cut_test_new(dummy_test_function);
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
    cut_assert_equal_int(2, cut_test_case_get_test_count(test_object));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

