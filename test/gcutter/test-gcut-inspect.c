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
void test_int64 (void);
void test_uint64 (void);
void test_size (void);
void data_char (void);
void test_char (gconstpointer data);
void test_string (void);
void test_type (void);
void test_flags (void);
void test_enum (void);
void test_pointer (void);
void data_boolean (void);
void test_boolean (gconstpointer data);
void test_double (void);

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
    gint value = G_MININT;

    gcut_inspect_int(string, &value, NULL);
    cut_assert_equal_string("-2147483648", string->str);
}

void
test_uint (void)
{
    guint value = G_MAXUINT;

    gcut_inspect_uint(string, &value, NULL);
    cut_assert_equal_string("4294967295", string->str);
}

void
test_int64 (void)
{
    gint64 value = G_MININT64;

    gcut_inspect_int64(string, &value, NULL);
    cut_assert_equal_string("-9223372036854775808", string->str);
}

void
test_uint64 (void)
{
    guint64 value = G_MAXUINT64;

    gcut_inspect_uint64(string, &value, NULL);
    cut_assert_equal_string("18446744073709551615", string->str);
}

void
test_size (void)
{
    gsize value = 29;

    gcut_inspect_size(string, &value, NULL);
    cut_assert_equal_string("29", string->str);
}

void
data_char (void)
{
#define ADD_DATUM(label, expected, character)             \
    gcut_add_datum(label,                                 \
                   "expected", G_TYPE_STRING, expected,   \
                   "value", G_TYPE_CHAR, character,       \
                   NULL)

    ADD_DATUM("normal", "'X'", 'X');
    ADD_DATUM("NULL", "'\\0'", '\0');
    ADD_DATUM("escaped", "'\\n'", '\n');
    ADD_DATUM("backslash", "'\\\\'", '\\');
    ADD_DATUM("quote", "'\\''", '\'');

#undef ADD_DATUM
}

void
test_char (gconstpointer data)
{
    gchar value;
    const gchar *expected;


    expected = gcut_data_get_string(data, "expected");
    value = gcut_data_get_char(data, "value");
    gcut_inspect_char(string, &value, NULL);
    cut_assert_equal_string(expected, string->str);
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


void
data_boolean (void)
{
#define ADD_DATUM(label, expected, boolean)             \
    gcut_add_datum(label,                               \
                   "expected", G_TYPE_STRING, expected, \
                   "value", G_TYPE_BOOLEAN, boolean,    \
                   NULL)

    ADD_DATUM("true", "TRUE", TRUE);
    ADD_DATUM("false", "FALSE", FALSE);
    ADD_DATUM("not false", "TRUE", 100);

#undef ADD_DATUM
}

void
test_boolean (gconstpointer data)
{
    gboolean value;
    const gchar *expected;

    expected = gcut_data_get_string(data, "expected");
    value = gcut_data_get_boolean(data, "value");

    gcut_inspect_boolean(string, &value, NULL);
    cut_assert_equal_string(expected, string->str);
}

void
test_double (void)
{
    gdouble value = 2.9;

    gcut_inspect_double(string, &value, NULL);
    cut_assert_equal_string("2.9", string->str);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
