#include "cut.h"

void test_equal_int(void);
void test_equal_string(void);
void test_equal_double(void);
void test_fail(void);

static void
dummy_fail_test_function (void)
{
    cut_fail("This test should fail");
}

void
test_equal_int (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(-1, -1);
    cut_assert_equal_int(0, 0);
}

void
test_equal_string (void)
{
    cut_assert_equal_string("", "");
    cut_assert_equal_string("a", "a");
}

void
test_equal_double (void)
{
    cut_assert_equal_double(1.0, 0.1, 1.0);
    cut_assert_equal_double(1.0, 0.01, 1.01);
}

void
test_fail (void)
{
    CutTest *test_object;
    CutTest *original_test;
    gboolean ret;

    test_object = cut_test_new("dummy-fail-test", dummy_fail_test_function);
    cut_assert(test_object);
    original_test = cut_context_get_current_test(cut_context_get_current());
    cut_context_set_test(cut_context_get_current(), test_object);
    ret = cut_test_run(test_object);
    cut_context_set_test(cut_context_get_current(), original_test);
    cut_assert(!ret);
    g_object_unref(test_object);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

