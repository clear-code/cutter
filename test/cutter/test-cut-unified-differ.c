/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <gcutter.h>
#include <cutter/cut-unified-differ.h>

void test_same_contents_unified_diff(void);
void test_inserted_unified_diff(void);
void test_deleted_unified_diff(void);
void test_replace_unified_diff(void);
void test_difference_unified_diff(void);
void test_complex_unified_diff(void);
void test_double_width_unified_diff(void);
void test_empty_unified_diff(void);

#define cut_assert_unified_diff(expected, from, to)    \
    cut_assert_equal_string_with_free(expected, cut_diff_unified(from, to))

void
test_same_contents_unified_diff (void)
{
    cut_assert_unified_diff("", "aaa", "aaa");

    cut_assert_unified_diff("",

                            "aaa\n"
                            "bbb",

                            "aaa\n"
                            "bbb");
}

void
test_inserted_unified_diff(void)
{
    cut_assert_unified_diff("--- from\n"
                            "+++ to\n"
                            "@@ -1 +1,4 @@\n"
                            " aaa\n"
                            "+bbb\n"
                            "+ccc\n"
                            "+ddd",

                            "aaa",

                            "aaa\n"
                            "bbb\n"
                            "ccc\n"
                            "ddd");
}

void
test_deleted_unified_diff(void)
{
    cut_assert_unified_diff("--- from\n"
                            "+++ to\n"
                            "@@ -1,4 +1 @@\n"
                            " aaa\n"
                            "-bbb\n"
                            "-ccc\n"
                            "-ddd",

                            "aaa\n"
                            "bbb\n"
                            "ccc\n"
                            "ddd",

                            "aaa");
}

void
test_replace_unified_diff (void)
{
    cut_assert_unified_diff("--- from\n"
                            "+++ to\n"
                            "@@ -1,4 +1,4 @@\n"
                            "+zero\n"
                            " one\n"
                            "-two\n"
                            "-three\n"
                            "+thre\n"
                            " four",

                            "one\n"
                            "two\n"
                            "three\n"
                            "four",

                            "zero\n"
                            "one\n"
                            "thre\n"
                            "four");
}

void
test_empty_unified_diff (void)
{
    cut_assert_unified_diff("", "", "");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
