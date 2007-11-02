#include "cut-test.h"
#include "cut-assertion.h"

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
    test_object = cut_test_new(dummy_test_function);
}

void
teardown (void)
{
    g_object_unref(test_object);
}

static void
test_assertion_count (void)
{
    cut_test_run(test_object);
    cut_assert_equal_int(3, cut_test_get_assertion_count(test_object));
}

CutTestEntry cut_tests[] =
{
    {test_assertion_count}
};
const gint cut_tests_len = G_N_ELEMENTS(cut_tests);

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

