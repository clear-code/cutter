/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-utils.h>
#include "../lib/cuttest-utils.h"

void test_take_replace (void);
void test_remove_path (void);
void test_build_path (void);
void test_make_directory (void);

static gchar *tmp_dir;

void
cut_setup (void)
{
    tmp_dir = g_build_filename(cuttest_get_base_dir(), "tmp", NULL);
    cut_remove_path(tmp_dir, NULL);

    if (g_mkdir_with_parents(tmp_dir, 0700) == -1)
        cut_error_errno();
}

void
cut_teardown (void)
{
    if (tmp_dir) {
        cut_remove_path(tmp_dir, NULL);
        g_free(tmp_dir);
    }
}

void
test_take_replace (void)
{
    cut_assert_equal_string("aaa BBB ccc",
                            cut_take_replace("aaa bbb ccc", "bbb", "BBB"));
    cut_assert_equal_string("Xaa\nXbb\nXcc",
                            cut_take_replace("aaa\nbbb\nccc", "^.", "X"));
}

void
test_remove_path (void)
{
    const gchar *sub_dir;
    const gchar *sub_sub_dir;

    sub_dir = cut_build_path(tmp_dir, "sub", NULL);
    sub_sub_dir = cut_build_path(sub_dir, "sub-sub", NULL);
    cut_make_directory(tmp_dir, "sub", "sub-sub", NULL);
    cut_assert_true(g_file_test(sub_sub_dir, G_FILE_TEST_IS_DIR));

    cut_remove_path(tmp_dir, "sub", NULL);
    cut_assert_false(g_file_test(sub_sub_dir, G_FILE_TEST_IS_DIR));
    cut_assert_false(g_file_test(sub_dir, G_FILE_TEST_IS_DIR));
    cut_assert_true(g_file_test(tmp_dir, G_FILE_TEST_IS_DIR));
}

void
test_build_path (void)
{
    const gchar *expected;
    cut_assert_equal_string("a",
                            cut_build_path("a", NULL));
#ifdef G_OS_WIN32
    expected = "a\\b\\c";
#else
    expected = "a/b/c";
#endif
    cut_assert_equal_string(expected,
                            cut_build_path("a", "b", "c", NULL));
}

void
test_make_directory (void)
{
    const gchar *new_dir;

    new_dir = cut_build_path(tmp_dir, "sub", "sub-sub", NULL);
    cut_assert_false(g_file_test(new_dir, G_FILE_TEST_IS_DIR));
    cut_make_directory(tmp_dir, "sub", "sub-sub", NULL);
    cut_assert_true(g_file_test(new_dir, G_FILE_TEST_IS_DIR));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
