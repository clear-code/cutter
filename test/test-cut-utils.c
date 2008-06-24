#include <gcutter.h>
#include <cutter/cut-utils.h>
#include <cuttest-utils.h>

void test_compare_string_array (void);
void test_inspect_string_array (void);
void test_is_interested_diff (void);
void test_strv_concat (void);
void test_remove_path_recursive (void);

static gchar *tmp_dir;

void
setup (void)
{
    tmp_dir = g_build_filename(cuttest_get_base_dir(), "tmp", NULL);
    cut_remove_path(tmp_dir);

    if (g_mkdir_with_parents(tmp_dir, 0700) == -1)
        cut_error_errno();
}

void
teardown (void)
{
    if (tmp_dir) {
        cut_utils_remove_path_recursive(tmp_dir, NULL);
        g_free(tmp_dir);
    }
}

void
test_compare_string_array (void)
{
    gchar *strings1[] = {"a", "b", "c", NULL};
    gchar *strings2[] = {"a", "b", "c", NULL};
    gchar *strings3[] = {"a", "b", NULL};
    gchar *strings4[] = {"a", "b", "d", NULL};

    cut_assert(cut_utils_compare_string_array(strings1, strings2));
    cut_assert(!cut_utils_compare_string_array(NULL, strings2));
    cut_assert(cut_utils_compare_string_array(NULL, NULL));
    cut_assert(!cut_utils_compare_string_array(strings1, strings3));
    cut_assert(!cut_utils_compare_string_array(strings1, strings4));
}


void
test_inspect_string_array (void)
{
    gchar *strings[] = {"a", "b", "c", NULL};

    cut_assert_equal_string_with_free("(null)",
                                      cut_utils_inspect_string_array(NULL));
    cut_assert_equal_string_with_free("[\"a\", \"b\", \"c\"]",
                                      cut_utils_inspect_string_array(strings));
}

void
test_is_interested_diff (void)
{
    cut_assert_false(cut_utils_is_interested_diff(NULL));
    cut_assert_false(cut_utils_is_interested_diff(""));
    cut_assert_false(cut_utils_is_interested_diff(" a\n"
                                                  " b\n"
                                                  " c"));
    cut_assert_false(cut_utils_is_interested_diff("- abc\n"
                                                  "+ abc"));
    cut_assert_true(cut_utils_is_interested_diff("- a\n"
                                                 "+ b\n"
                                                 "+ c"));
    cut_assert_true(cut_utils_is_interested_diff("- abc\n"
                                                 "+ abc\n"
                                                 "  xyz"));
    cut_assert_true(cut_utils_is_interested_diff("- abc def ghi xyz\n"
                                                 "?     ^^^\n"
                                                 "+ abc DEF ghi xyz\n"
                                                 "?     ^^^"));
    cut_assert_true(cut_utils_is_interested_diff("  a\n"
                                                 "- abc def ghi xyz\n"
                                                 "?     ^^^\n"
                                                 "+ abc DEF ghi xyz\n"
                                                 "?     ^^^"));
}

void
test_strv_concat (void)
{
    const gchar *strings[] = {"a", "b", "c", NULL};
    gchar *expected[] = {"a", "b", "c", "d", "e", NULL};

    cut_assert_equal_string_array(expected,
                                  cut_utils_strv_concat(strings, "d", "e",
                                                        NULL));
}

void
test_remove_path_recursive (void)
{
    GError *error = NULL;
    const gchar *file, *sub_dir, *sub_file;

    file = cut_take_string(g_build_filename(tmp_dir, "file", NULL));
    sub_dir = cut_take_string(g_build_filename(tmp_dir, "sub", NULL));
    sub_file = cut_take_string(g_build_filename(sub_dir, "file", NULL));

    cut_assert_path_exist(tmp_dir);
    cut_assert_path_not_exist(file);
    cut_assert_path_not_exist(sub_dir);
    cut_assert_path_not_exist(sub_file);

    g_file_set_contents(file, "top file", -1, &error);
    cut_assert_g_error(error);

    if (g_mkdir_with_parents(sub_dir, 0700) == -1)
        cut_assert_errno();

    g_file_set_contents(sub_file, "sub file", -1, &error);
    cut_assert_g_error(error);

    cut_utils_remove_path_recursive(tmp_dir, &error);
    cut_assert_g_error(error);

    cut_assert_path_not_exist(sub_file);
    cut_assert_path_not_exist(sub_dir);
    cut_assert_path_not_exist(file);
    cut_assert_path_not_exist(tmp_dir);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
