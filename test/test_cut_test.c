#include "cutter.h"
#include "cut-test.h"
#include "cut-context-private.h"

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
    CutTest *original_test;
    gboolean ret;

    original_test = cut_context_get_current_test(cut_context_get_current());
    cut_context_set_test(cut_context_get_current(), test_object);
    ret = cut_test_run(test_object);
    cut_context_set_test(cut_context_get_current(), original_test);
    cut_assert(ret);
    cut_assert_equal_int(3, cut_test_get_assertion_count(test_object));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
