#include "cutter.h"
#include "cut-context.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"
#include "cut-loader.h"

#include "cuttest-utils.h"

void test_run_test_case (void);
void test_run_test_case_with_regex (void);
void test_run_test_function (void);
void test_run_test_function_with_regex (void);
void test_run_test_function_in_test_case (void);
void test_run_test_function_with_regex_in_test_case (void);
void test_run_test_function_in_test_case_with_regex (void);
void test_run_test_function_with_regex_in_test_case_with_regex (void);

static CutContext *test_context;
static CutTestSuite *test_object;
static CutLoader *loader;

static gint n_run_dummy_run_test_function = 0;
static gint n_run_dummy_test_function1 = 0;
static gint n_run_dummy_test_function2 = 0;

static gint n_run_bummy_run_test_function = 0;
static gint n_run_bummy_test_function1 = 0;
static gint n_run_bummy_test_function2 = 0;


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
bummy_test_function1 (void)
{
    n_run_bummy_test_function1++;
}

static void
bummy_test_function2 (void)
{
    n_run_bummy_test_function2++;
}

static void
bummy_run_test_function (void)
{
    n_run_bummy_run_test_function++;
}

void
setup (void)
{
    CutTestCase *test_case;
    CutTest *test;
    gchar *test_path;

    n_run_dummy_test_function1 = 0;
    n_run_dummy_test_function2 = 0;
    n_run_dummy_run_test_function = 0;
    n_run_bummy_test_function1 = 0;
    n_run_bummy_test_function2 = 0;
    n_run_bummy_run_test_function = 0;

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);

    test_object = cut_test_suite_new();

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "loader_test_dir",
                                 ".libs",
                                 "libdummy_loader_test.so",
                                 NULL);
    loader = cut_loader_new(test_path);
    g_free(test_path);
    test_case = cut_loader_load_test_case(loader);
    cut_test_suite_add_test_case(test_object, test_case);

    test_case = cut_test_case_new("dummy_test_case", NULL, NULL,
                                  get_current_test_context,
                                  set_current_test_context);
    test = cut_test_new("dummy_test_1", NULL, dummy_test_function1);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("dummy_test_2", NULL, dummy_test_function2);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("run_test_function", NULL, dummy_run_test_function);
    cut_test_case_add_test(test_case, test);
    cut_test_suite_add_test_case(test_object, test_case);

    test_case = cut_test_case_new("bummy_test_case", NULL, NULL,
                                  get_current_test_context,
                                  set_current_test_context);
    test = cut_test_new("bummy_test_1", NULL, bummy_test_function1);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("bummy_test_2", NULL, bummy_test_function2);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("bummy_run_test_function", NULL,
                        bummy_run_test_function);
    cut_test_case_add_test(test_case, test);
    cut_test_suite_add_test_case(test_object, test_case);
}

void
teardown (void)
{
    g_object_unref(loader);
    g_object_unref(test_object);
    g_object_unref(test_context);
}

static gboolean
run_test_case (const gchar *test_case_name)
{
    return cut_test_suite_run_test_case(test_object,
                                        test_context,
                                        test_case_name);
}

static gboolean
run_test_function (const gchar *test_function_name)
{
    return cut_test_suite_run_test_function(test_object,
                                            test_context,
                                            test_function_name);
}

static gboolean
run_test_function_in_test_case (const gchar *test_function_name,
                                const gchar *test_case_name)
{
    return cut_test_suite_run_test_function_in_test_case(test_object,
                                                         test_context,
                                                         test_function_name,
                                                         test_case_name);
}

void
test_run_test_case (void)
{
    cut_assert(run_test_case("dummy_test_case"));
    cut_assert_equal_int(n_run_dummy_test_function1, 1);
    cut_assert_equal_int(n_run_dummy_test_function2, 1);
    cut_assert_equal_int(n_run_dummy_run_test_function, 1);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

void
test_run_test_case_with_regex (void)
{
    cut_assert(run_test_case("/^dummy.*/"));
    cut_assert_equal_int(n_run_dummy_test_function1, 1);
    cut_assert_equal_int(n_run_dummy_test_function2, 1);
    cut_assert_equal_int(n_run_dummy_run_test_function, 1);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

void
test_run_test_function (void)
{
    cut_assert(run_test_function("run_test_function"));
    cut_assert_equal_int(n_run_dummy_test_function1, 0);
    cut_assert_equal_int(n_run_dummy_test_function2, 0);
    cut_assert_equal_int(n_run_dummy_run_test_function, 1);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

void
test_run_test_function_with_regex (void)
{
    cut_assert(run_test_function("/^dummy.*/"));
    cut_assert_equal_int(n_run_dummy_test_function1, 1);
    cut_assert_equal_int(n_run_dummy_test_function2, 1);
    cut_assert_equal_int(n_run_dummy_run_test_function, 0);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

void
test_run_test_function_in_test_case (void)
{
    cut_assert(run_test_function_in_test_case("run_test_function", "dummy_test_case"));
    cut_assert_equal_int(n_run_dummy_test_function1, 0);
    cut_assert_equal_int(n_run_dummy_test_function2, 0);
    cut_assert_equal_int(n_run_dummy_run_test_function, 1);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

void
test_run_test_function_with_regex_in_test_case (void)
{
    cut_assert(run_test_function_in_test_case("/^dummy.*/", "dummy_test_case"));
    cut_assert_equal_int(n_run_dummy_test_function1, 1);
    cut_assert_equal_int(n_run_dummy_test_function2, 1);
    cut_assert_equal_int(n_run_dummy_run_test_function, 0);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

void
test_run_test_function_in_test_case_with_regex (void)
{
    cut_assert(run_test_function_in_test_case("run_test_function", "/^dummy.*/"));
    cut_assert_equal_int(n_run_dummy_test_function1, 0);
    cut_assert_equal_int(n_run_dummy_test_function2, 0);
    cut_assert_equal_int(n_run_dummy_run_test_function, 1);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

void
test_run_test_function_with_regex_in_test_case_with_regex (void)
{
    cut_assert(run_test_function_in_test_case("/^dummy.*/", "/^dummy.*/"));
    cut_assert_equal_int(n_run_dummy_test_function1, 1);
    cut_assert_equal_int(n_run_dummy_test_function2, 1);
    cut_assert_equal_int(n_run_dummy_run_test_function, 0);
    cut_assert_equal_int(n_run_bummy_test_function1, 0);
    cut_assert_equal_int(n_run_bummy_test_function2, 0);
    cut_assert_equal_int(n_run_bummy_run_test_function, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

