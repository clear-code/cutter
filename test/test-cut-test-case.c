#include "cutter.h"
#include "cut-test-case.h"
#include "cut-context-private.h"

void test_setup(void);
void test_teardown(void);
void test_test_case_count(void);
void test_run(void);
void test_run_with_setup_error(void);
void test_run_this_function(void);
void test_run_functions_with_regex(void);
void test_get_name(void);
void test_has_function(void);

static CutTestCase *test_object;
static CutContext *test_context;

static gboolean set_error_on_setup = FALSE;

static gint n_setup = 0;
static gint n_teardown = 0;
static gint n_run_dummy_run_test_function = 0;
static gint n_run_dummy_test_function1 = 0;
static gint n_run_dummy_test_function2 = 0;

static void
dummy_test_function1 (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    n_run_dummy_test_function1++;
}

static void
dummy_test_function2 (void)
{
    n_run_dummy_test_function2++;
}

static void
dummy_run_test_function (void)
{
    n_run_dummy_run_test_function++;
}

static void
dummy_setup_function (void)
{
    if (set_error_on_setup)
        cut_error("Error in setup");
    n_setup++;
}

static void
dummy_teardown_function (void)
{
    n_teardown++;
}

void
setup (void)
{
    CutTest *test;

    set_error_on_setup = FALSE;

    n_setup = 0;
    n_teardown = 0;
    n_run_dummy_run_test_function = 0;
    n_run_dummy_test_function1 = 0;
    n_run_dummy_test_function2 = 0;

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);

    test_object = cut_test_case_new("dummy test case",
                                    dummy_setup_function,
                                    dummy_teardown_function);
    test = cut_test_new("dummy test 1", dummy_test_function1);
    cut_test_case_add_test(test_object, test);
    test = cut_test_new("dummy test 2", dummy_test_function2);
    cut_test_case_add_test(test_object, test);
    test = cut_test_new("run_test_function", dummy_run_test_function);
    cut_test_case_add_test(test_object, test);
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
    ret = cut_test_case_run(test_object);
    cut_context_set_current(original_context);

    return ret;
}

void
test_setup (void)
{
    cut_assert(run_the_test());
    cut_assert(1 < n_setup);
}

void
test_teardown (void)
{
    cut_assert(run_the_test());
    cut_assert(1 < n_teardown);
}

void
test_test_case_count (void)
{
    cut_assert_equal_int(3, cut_test_case_get_test_count(test_object));
}

void
test_run (void)
{
    cut_assert(run_the_test());
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
}

void
test_run_with_setup_error (void)
{
    set_error_on_setup = TRUE;
    cut_assert(!run_the_test());
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
}

void
test_run_this_function (void)
{
    CutContext *original_context;
    gboolean ret;

    original_context = cut_context_get_current();
    cut_context_set_current(test_context);
    ret = cut_test_case_run_function(test_object, "run_test_function");
    cut_context_set_current(original_context);

    cut_assert(ret);

    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_dummy_test_function1);
}

void
test_run_functions_with_regex (void)
{
    CutContext *original_context;
    gboolean ret;

    original_context = cut_context_get_current();
    cut_context_set_current(test_context);
    ret = cut_test_case_run_function(test_object, "/^dummy/");
    cut_context_set_current(original_context);

    cut_assert(ret);

    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
}

void
test_get_name (void)
{
    cut_assert_equal_string("dummy test case",
                            cut_test_case_get_name(test_object));
}

void
test_has_function (void)
{
    cut_assert(cut_test_case_has_function(test_object, "run_test_function"));
    cut_assert(!cut_test_case_has_function(test_object, "not_exist_function"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

