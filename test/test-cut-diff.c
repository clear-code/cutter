/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <cutter/cut-gassertions.h>
#include <cutter/cut-diff.h>

void test_same_contents_readable_diff(void);
void test_inserted_readable_diff(void);
void test_deleted_readable_diff(void);
void test_replace_readable_diff(void);
void test_difference_readable_diff(void);
void test_complex_readable_diff(void);
void test_empty_readable_diff(void);

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
test_empty_readable_diff (void)
{
    cut_assert_readable_diff("", "", "");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
