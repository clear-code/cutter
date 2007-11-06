#include "cutter.h"
#include "cut-loader.h"

void test_load_function (void);

static CutLoader *test_loader;

void
setup (void)
{
    const gchar *base_dir;
    gchar *test_path;

    base_dir = g_getenv("BASE_DIR");
    if (!base_dir)
        base_dir = ".";
    test_path = g_build_filename(base_dir,
                                 "loader_test_dir",
                                 ".libs",
                                 "libdummy_loader_test.so",
                                 NULL);
    test_loader = cut_loader_new(test_path);
    g_free(test_path);
}

void
teardown (void)
{
    g_object_unref(test_loader);
}

static const gchar *expected_functions[] = {
"test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789",
"test_dummy_function1",
"test_dummy_function2",
"test_dummy_function3"
};

static gint
compare_function_name (gconstpointer a, gconstpointer b)
{
    CutTest *ta, *tb;
    ta = CUT_TEST(a);
    tb = CUT_TEST(b);

    return strcmp(cut_test_get_function_name(ta), cut_test_get_function_name(tb));
}

void
test_load_function (void)
{
    CutTestCase *test_case;
    GList *tests, *list;
    gint i;

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);
    cut_assert_equal_int(4, cut_test_case_get_test_count(test_case));

    tests = (GList *) cut_test_container_get_children(CUT_TEST_CONTAINER(test_case));
    cut_assert(tests);

    tests = g_list_sort(tests, compare_function_name);
    for (list = tests, i = 0; list; list = g_list_next(list), i++) {
        CutTest *test;

        cut_assert(list->data);
        cut_assert(CUT_IS_TEST(list->data));

        test = CUT_TEST(list->data);
        cut_assert_equal_string(expected_functions[i], cut_test_get_function_name(test));
    }

    g_object_unref(test_case);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
