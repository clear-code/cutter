/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <cutter/cut-gassertions.h>
#include <cutter/cut-diff.h>

void test_same_contents_readable_diff(void);
void test_inserted_readable_diff(void);
void test_deleted_readable_diff(void);

void
setup (void)
{
}

void
teardown (void)
{
}

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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
