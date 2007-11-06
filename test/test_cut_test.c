#include "cutter.h"
#include "cut-test.h"
#include "cut-context-private.h"

void test_assertion_count(void);
void test_get_function_name(void);
void test_increment_assertion_count(void);
void test_run(void);
void test_set_error(void);
void test_get_error(void);

static CutContext *test_context;
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
    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);
    test_object = cut_test_new("dummy-test", dummy_test_function);
    run_test_flag = FALSE;
}

void
teardown (void)
{
    g_object_unref(test_object);
    g_object_unref(test_context);
}

static gboolean
run_the_test (void)
{
    CutContext *original_context;
    gboolean ret;

    original_context = cut_context_get_current();
    cut_context_set_current(test_context);
    cut_context_set_test(test_context, test_object);
    ret = cut_test_run(test_object);
    cut_context_set_current(original_context);

    return ret;
}

void
test_assertion_count (void)
{
    cut_assert(run_the_test());
    cut_assert_equal_int(3, cut_test_get_assertion_count(test_object));
}

void
test_increment_assertion_count (void)
{
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_test_increment_assertion_count(test_object);
    cut_assert_equal_int(5, cut_test_get_assertion_count(test_object));
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

void
test_get_error (void)
{
    cut_assert(FALSE);
}

void
test_set_error (void)
{
    cut_assert(FALSE);
} 

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
