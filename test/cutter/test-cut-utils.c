/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-utils.h>
#include "../lib/cuttest-utils.h"

void test_inspect_memory (void);
void test_inspect_memory_with_printable (void);
void test_compare_string_array (void);
void test_inspect_string_array (void);
void test_strv_concat (void);
void test_remove_path_recursive (void);
void test_fold (void);
void test_equal_string (void);
void test_equal_double (void);

static gchar *tmp_dir;
static gchar **actual_string_array;

void
setup (void)
{
    tmp_dir = g_build_filename(cuttest_get_base_dir(), "tmp", NULL);
    cut_remove_path(tmp_dir, NULL);

    if (g_mkdir_with_parents(tmp_dir, 0700) == -1)
        cut_error_errno();

    actual_string_array = NULL;
}

void
teardown (void)
{
    if (tmp_dir) {
        cut_remove_path(tmp_dir, NULL);
        g_free(tmp_dir);
    }

    if (actual_string_array)
        g_strfreev(actual_string_array);
}

void
test_inspect_memory (void)
{
    gchar binary[] = {0x00, 0x01, 0x02, 0x1f};
    gchar empty[] = {};

    cut_assert_equal_string_with_free("(null)",
                                      cut_utils_inspect_memory(empty, 0));
    cut_assert_equal_string_with_free("(null)",
                                      cut_utils_inspect_memory(NULL, 100));
    cut_assert_equal_string_with_free("0x00 0x01 0x02 0x1f",
                                      cut_utils_inspect_memory(binary,
                                                               sizeof(binary)));
    cut_assert_equal_string_with_free("0x00 0x01",
                                      cut_utils_inspect_memory(binary, 2));
}

void
test_inspect_memory_with_printable (void)
{
    gchar binary[] = {0x00, 0x01, 0x02, 0x1f, 'G', 'N', 'U', 0x01};

    cut_assert_equal_string_with_free(
        "0x00 0x01 0x02 0x1f 0x47 0x4e 0x55 0x01: ....GNU.",
        cut_utils_inspect_memory(binary, sizeof(binary)));

    cut_assert_equal_string_with_free(
        "0x00 0x01 0x02 0x1f 0x47",
        cut_utils_inspect_memory(binary, sizeof(binary) - 3));

    cut_assert_equal_string_with_free(
        "0x00 0x01 0x02 0x1f 0x47 0x4e: ....GN",
        cut_utils_inspect_memory(binary, sizeof(binary) - 2));
}

void
test_compare_string_array (void)
{
    gchar *strings1[] = {"a", "b", "c", NULL};
    gchar *strings2[] = {"a", "b", "c", NULL};
    gchar *strings3[] = {"a", "b", NULL};
    gchar *strings4[] = {"a", "b", "d", NULL};

    cut_assert_true(cut_utils_equal_string_array(strings1, strings2));
    cut_assert_false(cut_utils_equal_string_array(NULL, strings2));
    cut_assert_true(cut_utils_equal_string_array(NULL, NULL));
    cut_assert_false(cut_utils_equal_string_array(strings1, strings3));
    cut_assert_false(cut_utils_equal_string_array(strings1, strings4));
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
test_strv_concat (void)
{
    const gchar *strings[] = {"a", "b", "c", NULL};
    gchar *expected[] = {"a", "b", "c", "d", "e", NULL};

    actual_string_array = cut_utils_strv_concat(strings, "d", "e", NULL);
    cut_assert_equal_string_array(expected, actual_string_array);
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
    gcut_assert_error(error);

    if (g_mkdir_with_parents(sub_dir, 0700) == -1)
        cut_assert_errno();

    g_file_set_contents(sub_file, "sub file", -1, &error);
    gcut_assert_error(error);

    cut_utils_remove_path_recursive(tmp_dir, &error);
    gcut_assert_error(error);

    cut_assert_path_not_exist(sub_file);
    cut_assert_path_not_exist(sub_dir);
    cut_assert_path_not_exist(file);
    cut_assert_path_not_exist(tmp_dir);
}

void
test_fold (void)
{
    cut_assert_equal_string_with_free("0123456789"
                                      "1123456789"
                                      "2123456789"
                                      "3123456789"
                                      "4123456789"
                                      "5123456789"
                                      "6123456789"
                                      "71234567\n"
                                      "89"
                                      "8123456789",
                                      cut_utils_fold("0123456789"
                                                     "1123456789"
                                                     "2123456789"
                                                     "3123456789"
                                                     "4123456789"
                                                     "5123456789"
                                                     "6123456789"
                                                     "7123456789"
                                                     "8123456789"));
}

void
test_equal_string (void)
{
    cut_assert_true(cut_utils_equal_string(NULL, NULL));
    cut_assert_true(cut_utils_equal_string("string", "string"));
    cut_assert_false(cut_utils_equal_string("string", NULL));
    cut_assert_false(cut_utils_equal_string(NULL, "string"));
}

void
test_equal_double (void)
{
    cut_assert_true(cut_utils_equal_double(0, 0, 0.1));
    cut_assert_true(cut_utils_equal_double(0.11, 0.19, 0.1));
    cut_assert_true(cut_utils_equal_double(0.11, 0.12, 0.01));
    cut_assert_false(cut_utils_equal_double(0.11, 0.12, 0.009));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
