#include "cutter.h"
#include "cut-loader.h"

#include "cuttest-utils.h"

void test_load_function (void);
void test_load_startup_and_shutdown_function (void);
void test_fail_to_load (void);

static CutLoader *test_loader;
static CutTestCase *test_case;
static gchar **test_names;

void
setup (void)
{
    gchar *test_path;

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "fixtures",
                                 "loader",
                                 "test",
                                 ".libs",
                                 "dummy_loader_test." G_MODULE_SUFFIX,
                                 NULL);
    test_loader = cut_loader_new(test_path);
    g_free(test_path);

    test_case = NULL;
    test_names = NULL;
}

void
teardown (void)
{
    if (test_loader)
        g_object_unref(test_loader);
    if (test_case)
        g_object_unref(test_case);
    if (test_names)
        g_strfreev(test_names);
}

static gchar *expected_functions[] = {
    "test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789",
    "test_dummy_function1",
    "test_dummy_function2",
    "test_dummy_function3",
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

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);
    cut_assert_equal_int(4, cut_test_case_get_n_tests(test_case, NULL));
    cut_assert_equal_int(4, cut_test_case_get_n_tests(test_case,
                                                      target_test_names));

    container = CUT_TEST_CONTAINER(test_case);
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

    test_case = cut_loader_load_test_case(test_loader);
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
    CutLoader *loader;
    gchar *test_path;

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "module_test_dir",
                                 ".libs",
                                 "cannot_load_module." G_MODULE_SUFFIX,
                                 NULL);
    loader = cut_loader_new(test_path);
    g_free(test_path);

    cut_assert(loader);
    cut_assert_null(cut_loader_load_test_case(loader));
    g_object_unref(loader);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
