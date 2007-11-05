#include "cutter.h"
#include "cut-test.h"
#include "cut-context-private.h"

void test_assertion_count(void);
void test_get_function_name(void);
void test_run(void);

static CutTest *test_object;
static gboolean run_test_flag = FALSE;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    run_test_flag = TRUE;
}

void
setup (void)
{
    test_object = cut_test_new("dummy-test", dummy_test_function);
    run_test_flag = FALSE;
}

void
teardown (void)
{
    g_object_unref(test_object);
}

static gboolean
run_the_test (void)
{
    CutTest *original_test;
    gboolean ret;

    original_test = cut_context_get_current_test(cut_context_get_current());
    cut_context_set_test(cut_context_get_current(), test_object);
    ret = cut_test_run(test_object);
    cut_context_set_test(cut_context_get_current(), original_test);

    return ret;
}

void
test_assertion_count (void)
{
    cut_assert(run_the_test());
    cut_assert_equal_int(3, cut_test_get_assertion_count(test_object));
}

void
test_get_function_name (void)
{
    cut_assert_equal_string("dummy-test", cut_test_get_function_name(test_object));
}

void
test_run (void)
{
    cut_assert(run_the_test());
    cut_assert(run_test_flag);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
