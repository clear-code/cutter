#include "cut-test.h"

static CutTest *test_object;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
}

static void
setup (void)
{
    test_object = cut_test_new("dummy test", dummy_test_function);
}

static void
tear_down (void)
{
    g_object_unref(test_object);
}

static void
test_assertion_count (void)
{
    cut_assert_equal_int(3, cut_test_get_assertion_count(test_object));
}

static void
test_name (void)
{
    cut_assert_equal_string("test name test", cut_test_get_name(test_object));
}

CutTestEntry cut_tests[] =
{
    {"assertion count test", test_assertion_count},
    {"test name test", test_name},
};
const gint cut_tests_len = G_N_ELEMENTS(cut_tests);

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

