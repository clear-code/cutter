/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-differ-readable.h>

void test_same_contents_readable_diff(void);
void test_inserted_readable_diff(void);
void test_deleted_readable_diff(void);
void test_replace_readable_diff(void);
void test_difference_readable_diff(void);
void test_complex_readable_diff(void);
void test_double_width_readable_diff(void);
void test_empty_readable_diff(void);
void test_is_interested (void);
void test_need_fold (void);

#define cut_assert_readable_diff(expected, from, to)    \
    cut_assert_equal_string_with_free(expected, cut_diff_readable(from, to))

void
test_same_contents_readable_diff (void)
{
    cut_assert_readable_diff("  aaa", "aaa", "aaa");

    cut_assert_readable_diff("  aaa\n"
                             "  bbb",

                             "aaa\n"
                             "bbb",

                             "aaa\n"
                             "bbb");
}

void
test_inserted_readable_diff(void)
{
    cut_assert_readable_diff("  aaa\n"
                             "+ bbb\n"
                             "+ ccc\n"
                             "+ ddd",

                             "aaa",

                             "aaa\n"
                             "bbb\n"
                             "ccc\n"
                             "ddd");
}

void
test_deleted_readable_diff(void)
{
    cut_assert_readable_diff("  aaa\n"
                             "- bbb",

                             "aaa\n"
                             "bbb",

                             "aaa");

    cut_assert_readable_diff("  aaa\n"
                             "- bbb\n"
                             "- ccc\n"
                             "- ddd",

                             "aaa\n"
                             "bbb\n"
                             "ccc\n"
                             "ddd",

                             "aaa");
}

void
test_replace_readable_diff (void)
{
    cut_assert_readable_diff("  aaa\n"
                             "- bbb\n"
                             "+ BbB\n"
                             "  ccc\n"
                             "- ddd\n"
                             "- efg\n"
                             "?  -\n"
                             "+ eg",

                             "aaa\n"
                             "bbb\n"
                             "ccc\n"
                             "ddd\n"
                             "efg",

                             "aaa\n"
                             "BbB\n"
                             "ccc\n"
                             "eg");

    cut_assert_readable_diff("-  abcd xyz abc\n"
                             "? -\n"
                             "+ abcd abcd xyz abc\n"
                             "?      +++++",

                             " abcd xyz abc",

                             "abcd abcd xyz abc");
}

void
test_difference_readable_diff (void)
{
    cut_assert_readable_diff("- 1 tests, 0 assertions, 1 failures, 0 pendings\n"
                             "?                        ^           ^\n"
                             "+ 1 tests, 0 assertions, 0 failures, 1 pendings\n"
                             "?                        ^           ^",

                             "1 tests, 0 assertions, 1 failures, 0 pendings",

                             "1 tests, 0 assertions, 0 failures, 1 pendings");
}

void
test_complex_readable_diff (void)
{
    cut_assert_readable_diff("  aaa\n"
                             "- bbb\n"
                             "- ccc\n"
                             "+ \n"
                             "+   # \n"
                             "  ddd",

                             "aaa\n"
                             "bbb\n"
                             "ccc\n"
                             "ddd",

                             "aaa\n"
                             "\n"
                             "  # \n"
                             "ddd");

    cut_assert_readable_diff("- one1\n"
                             "?  ^\n"
                             "+ ore1\n"
                             "?  ^\n"
                             "- two2\n"
                             "- three3\n"
                             "?  -   -\n"
                             "+ tree\n"
                             "+ emu",

                             "one1\n"
                             "two2\n"
                             "three3",

                             "ore1\n"
                             "tree\n"
                             "emu");
}

void
test_double_width_readable_diff (void)
{
    cut_assert_readable_diff("- あいうえおかきくけこ\n"
                             "?           ^^^^\n"
                             "+ あいうえおカキくけこ\n"
                             "?           ^^^^",

                             "あいうえおかきくけこ",

                             "あいうえおカキくけこ");
}

void
test_empty_readable_diff (void)
{
    cut_assert_readable_diff("", "", "");
}

#define cut_assert_readable_diff_is_interested(diff)    \
    cut_assert_true(cut_diff_readable_is_interested(diff))

#define cut_assert_readable_diff_is_not_interested(diff)    \
    cut_assert_false(cut_diff_readable_is_interested(diff))

void
test_is_interested (void)
{
    cut_assert_readable_diff_is_not_interested(NULL);
    cut_assert_readable_diff_is_not_interested("");
    cut_assert_readable_diff_is_not_interested(" a\n"
                                               " b\n"
                                               " c");
    cut_assert_readable_diff_is_not_interested("- abc\n"
                                               "+ abc");

    cut_assert_readable_diff_is_interested("- a\n"
                                           "+ b\n"
                                           "+ c");
    cut_assert_readable_diff_is_interested("- abc\n"
                                           "+ abc\n"
                                           "  xyz");
    cut_assert_readable_diff_is_interested("- abc def ghi xyz\n"
                                           "?     ^^^\n"
                                           "+ abc DEF ghi xyz\n"
                                           "?     ^^^");
    cut_assert_readable_diff_is_interested("  a\n"
                                           "- abc def ghi xyz\n"
                                           "?     ^^^\n"
                                           "+ abc DEF ghi xyz\n"
                                           "?     ^^^");
}

#define cut_assert_readable_diff_need_fold(diff)    \
    cut_assert_true(cut_diff_readable_need_fold(diff))

#define cut_assert_readable_diff_not_need_fold(diff)    \
    cut_assert_false(cut_diff_readable_need_fold(diff))

void
test_need_fold (void)
{
    cut_assert_readable_diff_not_need_fold(NULL);
    cut_assert_readable_diff_not_need_fold("");
    cut_assert_readable_diff_not_need_fold("0123456789"
                                           "1123456789"
                                           "2123456789"
                                           "3123456789"
                                           "4123456789"
                                           "5123456789"
                                           "6123456789"
                                           "7123456789");

    cut_assert_readable_diff_not_need_fold("- 23456789"
                                           "1123456789"
                                           "2123456789"
                                           "3123456789"
                                           "4123456789"
                                           "5123456789"
                                           "6123456789"
                                           "712345678");
    cut_assert_readable_diff_not_need_fold("+ 23456789"
                                           "1123456789"
                                           "2123456789"
                                           "3123456789"
                                           "4123456789"
                                           "5123456789"
                                           "6123456789"
                                           "712345678");

    cut_assert_readable_diff_need_fold("- 23456789"
                                       "1123456789"
                                       "2123456789"
                                       "3123456789"
                                       "4123456789"
                                       "5123456789"
                                       "6123456789"
                                       "7123456789");
    cut_assert_readable_diff_need_fold("+ 23456789"
                                       "1123456789"
                                       "2123456789"
                                       "3123456789"
                                       "4123456789"
                                       "5123456789"
                                       "6123456789"
                                       "7123456789");

    cut_assert_readable_diff_need_fold("\n"
                                       "+ 23456789"
                                       "1123456789"
                                       "2123456789"
                                       "3123456789"
                                       "4123456789"
                                       "5123456789"
                                       "6123456789"
                                       "7123456789"
                                       "\n");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
