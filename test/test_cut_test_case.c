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
    teardown_flag = FALSE;
}

static void
dummy_tear_down_function (void)
{
    setup_flag = FALSE;
    teardown_flag = TRUE;
}

void
setup (void)
{
    CutTest *test;
    test_object = cut_test_case_new(dummy_setup_function,
                                    dummy_tear_down_function);
    test = cut_test_new("dummy test", dummy_test_function);
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_object), test);
    test = cut_test_new("dummy test2", dummy_test_function);
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_object), test);
}

void
tear_down (void)
{
    g_object_unref(test_object);
}

static void
test_setup (void)
{
}

static void
test_test_case_count (void)
{
    cut_assert_equal_int(2, cut_test_case_get_test_count(test_object));
}

CutTestEntry cut_tests[] =
{
    {"test case count test", test_test_case_count},
};
const gint cut_tests_len = G_N_ELEMENTS(cut_tests);

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

