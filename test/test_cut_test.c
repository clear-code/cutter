#include "cut.h"

void test_assertion_count(void);

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
    test_object = cut_test_new("dummy-test", dummy_test_function);
}

void
teardown (void)
{
    g_object_unref(test_object);
}

void
test_assertion_count (void)
{
    cut_context_set_test(cut_context_get_current(), test_object);
    cut_assert(cut_test_run(test_object));
    cut_assert_equal_int(3, cut_test_get_assertion_count(test_object));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
