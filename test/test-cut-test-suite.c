#include "cutter.h"
#include "cut-runner.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"
#include "cut-loader.h"

#include "cuttest-utils.h"

void test_run_test_case (void);
void test_run_test_case_with_regex (void);
void test_run_test (void);
void test_run_test_with_regex (void);
void test_run_test_in_test_case (void);
void test_run_test_with_regex_in_test_case (void);
void test_run_test_in_test_case_with_regex (void);
void test_run_test_with_regex_in_test_case_with_regex (void);
void test_run_test_in_test_case_with_null (void);
void test_run_test_with_filter_with_null (void);

static CutRunner *runner;
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

    runner = cut_runner_new();

    test_object = cut_test_suite_new();

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "loader_test_dir",
                                 ".libs",
                                 "dummy_loader_test.so",
                                 NULL);
    loader = cut_loader_new(test_path);
    g_free(test_path);
    test_case = cut_loader_load_test_case(loader);
    cut_test_suite_add_test_case(test_object, test_case);

    test_case = cut_test_case_new("dummy_test_case", NULL, NULL,
                                  get_current_test_context,
                                  set_current_test_context,
                                  NULL, NULL);
    test = cut_test_new("dummy_test_1", dummy_test_function1);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("dummy_test_2", dummy_test_function2);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("run_test_function", dummy_run_test_function);
    cut_test_case_add_test(test_case, test);
    cut_test_suite_add_test_case(test_object, test_case);

    test_case = cut_test_case_new("bummy_test_case", NULL, NULL,
                                  get_current_test_context,
                                  set_current_test_context,
                                  NULL, NULL);
    test = cut_test_new("bummy_test_1", bummy_test_function1);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("bummy_test_2", bummy_test_function2);
    cut_test_case_add_test(test_case, test);
    test = cut_test_new("bummy_run_test_function",
                        bummy_run_test_function);
    cut_test_case_add_test(test_case, test);
    cut_test_suite_add_test_case(test_object, test_case);
}

void
teardown (void)
{
    g_object_unref(loader);
    g_object_unref(test_object);
    g_object_unref(runner);
}

static gboolean
run_test_case (const gchar *test_case_name)
{
    return cut_test_suite_run_test_in_test_case(test_object,
                                                runner,
                                                "/.*/",
                                                test_case_name);
}

static gboolean
run_test (const gchar *test_name)
{
    return cut_test_suite_run_test(test_object, runner, test_name);
}

static gboolean
run_test_in_test_case (const gchar *test_name, const gchar *test_case_name)
{
    return cut_test_suite_run_test_in_test_case(test_object,
                                                runner,
                                                test_name,
                                                test_case_name);
}

static gboolean
run_test_with_filter (const gchar **test_case_names, const gchar **test_names)
{
    return cut_test_suite_run_with_filter(test_object,
                                          runner,
                                          test_case_names,
                                          test_names);
}

void
test_run_test_case (void)
{
    cut_assert(run_test_case("dummy_test_case"));
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_case_with_regex (void)
{
    cut_assert(run_test_case("/dummy/"));
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test (void)
{
    cut_assert(run_test("run_test_function"));
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_with_regex (void)
{
    cut_assert(run_test("/dummy/"));
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_in_test_case (void)
{
    cut_assert(run_test_in_test_case("run_test_function", "dummy_test_case"));
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_with_regex_in_test_case (void)
{
    cut_assert(run_test_in_test_case("/dummy/", "dummy_test_case"));
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_in_test_case_with_regex (void)
{
    cut_assert(run_test_in_test_case("run_test_function", "/dummy/"));
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_with_regex_in_test_case_with_regex (void)
{
    cut_assert(run_test_in_test_case("/dummy/", "/dummy/"));
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_in_test_case_with_null (void)
{
    cut_assert(run_test(NULL));
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_test_function2);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

void
test_run_test_with_filter_with_null (void)
{
    cut_assert(run_test_with_filter(NULL, NULL));
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_test_function2);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_bummy_test_function1);
    cut_assert_equal_int(0, n_run_bummy_test_function2);
    cut_assert_equal_int(0, n_run_bummy_run_test_function);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/

