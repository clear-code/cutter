#include "cutter.h"
#include "cut-context-private.h"

void test_equal_int(void);
void test_equal_string(void);
void test_equal_double(void);
void test_fail(void);
void test_pending(void);

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
test_pending (void)
{
    cut_pending("This test has not been implemented yet.");
}

void
test_fail (void)
{
    CutTest *test_object;
    CutContext *original_context, *test_context;
    gboolean ret;

    test_object = cut_test_new("dummy-fail-test", dummy_fail_test_function);
    cut_assert(test_object);

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_VERBOSE);
    cut_context_set_test(test_context, test_object);

    original_context = cut_context_get_current();
    cut_context_set_current(test_context);
    ret = cut_test_run(test_object);
    cut_context_set_current(original_context);

    cut_assert(!ret);
    g_object_unref(test_object);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

