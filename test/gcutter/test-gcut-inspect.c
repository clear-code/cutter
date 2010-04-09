/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void test_direct (void);
void test_int (void);
void test_uint (void);
void test_size (void);
void test_string (void);
void test_type (void);
void test_flags (void);
void test_enum (void);
void test_pointer (void);

static GString *string;

void
cut_setup (void)
{
    string = g_string_new(NULL);
}

void
cut_teardown (void)
{
    if (string)
        g_string_free(string, TRUE);
}

void
test_direct (void)
{
    gcut_inspect_direct(string, GUINT_TO_POINTER(100), NULL);
    cut_assert_equal_string("100", string->str);
}

void
test_int (void)
{
    gint value = 99;

    gcut_inspect_int(string, &value, NULL);
    cut_assert_equal_string("99", string->str);
}

void
test_uint (void)
{
    guint value = 99;

    gcut_inspect_uint(string, &value, NULL);
    cut_assert_equal_string("99", string->str);
}

void
test_size (void)
{
    gsize value = 29;

    gcut_inspect_size(string, &value, NULL);
    cut_assert_equal_string("29", string->str);
}

void
test_string (void)
{
    gcut_inspect_string(string, "XXX", NULL);
    cut_assert_equal_string("\"XXX\"", string->str);
}

void
test_type (void)
{
    GType type;

    type = GCUT_TYPE_DYNAMIC_DATA;
    gcut_inspect_type(string, &type, NULL);
    cut_assert_equal_string("<GCutDynamicData>", string->str);
}

void
test_flags (void)
{
    CuttestFlags flags;
    GType flags_type;

    flags = CUTTEST_FLAG_FIRST | CUTTEST_FLAG_SECOND;
    flags_type = CUTTEST_TYPE_FLAGS;
    gcut_inspect_flags(string, &flags, &flags_type);
    cut_assert_equal_string("#<CuttestFlags: first|second "
                            "(CUTTEST_FLAG_FIRST:0x1)|"
                            "(CUTTEST_FLAG_SECOND:0x2)>",
                            string->str);
}

void
test_enum (void)
{
    CutTestResultStatus value;
    GType enum_type;

    value = CUT_TEST_RESULT_SUCCESS;
    enum_type = CUT_TYPE_TEST_RESULT_STATUS;
    gcut_inspect_enum(string, &value, &enum_type);
    cut_assert_equal_string(cut_take_printf("#<CutTestResultStatus: "
                                            "success"
                                            "(CUT_TEST_RESULT_SUCCESS:%d)>",
                                            CUT_TEST_RESULT_SUCCESS),
                            string->str);
}

void
test_pointer (void)
{
    gpointer value;

    value = string;
    gcut_inspect_pointer(string, value, NULL);
    cut_assert_equal_string(cut_take_printf("#<%p>", value),
                            string->str);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
