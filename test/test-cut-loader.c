#include <cutter.h>
#include <cutter/cut-loader.h>
#include <cutter/cut-test-runner.h>

#include "lib/cuttest-assertions.h"

void test_load_function (void);
void test_load_startup_and_shutdown_function (void);
void test_fail_to_load (void);
void test_load_test_iterator (void);

static CutLoader *loader;
static CutTestCase *test_case;
static CutRunContext *run_context;
static gchar **test_names;

void
setup (void)
{
    loader = NULL;
    test_case = NULL;
    test_names = NULL;
    run_context = NULL;
}

void
teardown (void)
{
    if (loader)
        g_object_unref(loader);
    if (test_case)
        g_object_unref(test_case);
    if (test_names)
        g_strfreev(test_names);
    if (run_context)
        g_object_unref(run_context);
}

static CutLoader *
loader_new (const gchar *directory, const gchar *module_name)
{
    CutLoader *loader;
    gchar *test_path;

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "fixtures",
                                 "loader",
                                 directory,
                                 ".libs",
                                 module_name,
                                 NULL);
    loader = cut_loader_new(test_path);
    g_free(test_path);

    return loader;
}

static gboolean
run (void)
{
    cut_assert_not_null(test_case);

    run_context = cut_test_runner_new();
    return cut_test_case_run(test_case, run_context);
}

static gchar *expected_functions[] = {
    "test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789",
    "test_stub_function1",
    "test_stub_function2",
    "test_stub_function3",
    NULL
};

static gint
compare_function_name (gconstpointer a, gconstpointer b)
{
    CutTest *ta, *tb;
    ta = CUT_TEST(a);
    tb = CUT_TEST(b);

    return strcmp(cut_test_get_name(ta), cut_test_get_name(tb));
}

void
test_load_function (void)
{
    CutTestContainer *container;
    GList *tests, *list;
    const gchar *target_test_names[] = {"/.*/", NULL};
    gint i;

    loader = loader_new("test", "stub-loader-test." G_MODULE_SUFFIX);
    test_case = cut_loader_load_test_case(loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    cut_assert_equal_int(4,
                         cut_test_container_get_n_tests(container, NULL));

    run_context = cut_test_runner_new();
    cut_run_context_set_target_test_names(run_context, target_test_names);
    cut_assert_equal_int(4,
                         cut_test_container_get_n_tests(container, run_context));

    tests = (GList *)cut_test_container_get_children(container);
    cut_assert(tests);

    tests = g_list_sort(tests, compare_function_name);
    test_names = g_new0(gchar *, g_list_length(tests) + 1);
    for (list = tests, i = 0; list; list = g_list_next(list), i++) {
        CutTest *test;

        cut_assert(list->data);
        cut_assert(CUT_IS_TEST(list->data));

        test = CUT_TEST(list->data);
        test_names[i] = g_strdup(cut_test_get_name(test));
    }
    cut_assert_equal_string_array(expected_functions, test_names);
}

void
test_load_startup_and_shutdown_function (void)
{
    CutStartupFunction startup_function = NULL;
    CutShutdownFunction shutdown_function = NULL;

    loader = loader_new("test", "stub-loader-test." G_MODULE_SUFFIX);
    test_case = cut_loader_load_test_case(loader);
    cut_assert(test_case);

    g_object_get(G_OBJECT(test_case),
                 "startup-function", &startup_function,
                 "shutdown-function", &shutdown_function,
                 NULL);
    cut_assert(startup_function);
    cut_assert(shutdown_function);
}

void
test_fail_to_load (void)
{
    loader = loader_new("module", "cannot-load-module." G_MODULE_SUFFIX);
    cut_assert_not_null(loader);

    test_case = cut_loader_load_test_case(loader);
    cut_assert_null(test_case);
}

void
test_load_test_iterator (void)
{
    const GList *tests;
    CutTestContainer *container;
    CutTestIterator *test_iterator;
    CutIteratedTestFunction iterated_test_function = NULL;
    CutDataSetupFunction data_setup_function = NULL;

    loader = loader_new("iterator", "success-iterated-test." G_MODULE_SUFFIX);
    cut_assert_not_null(loader);

    test_case = cut_loader_load_test_case(loader);
    cut_assert_not_null(test_case);
    container = CUT_TEST_CONTAINER(test_case);
    cut_assert_equal_int(1, cut_test_container_get_n_tests(container, NULL));

    tests = cut_test_container_get_children(container);
    cut_assert_not_null(tests);

    test_iterator = tests->data;
    cut_assert_not_null(test_iterator);

    g_object_get(test_iterator,
                 "iterated-test-function", &iterated_test_function,
                 "data-setup-function", &data_setup_function,
                 NULL);
    cut_assert_not_null(iterated_test_function);
    cut_assert_not_null(data_setup_function);

    cut_assert_true(run());
    cut_assert_test_result_summary(run_context, 2, 2, 2, 0, 0, 0, 0, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
