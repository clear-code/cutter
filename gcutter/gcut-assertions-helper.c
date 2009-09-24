/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@clear-code.com>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gstdio.h>

#include <cutter/cut-utils.h>
#include "gcut-assertions-helper.h"

void
gcut_assert_equal_type_helper (GType           expected,
                               GType           actual,
                               const gchar    *expression_expected,
                               const gchar    *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%s>\n"
                                      "  actual: <%s>",
                                      expression_expected,
                                      expression_actual,
                                      g_type_name(expected),
                                      g_type_name(actual)));
    }
}

void
gcut_assert_equal_value_helper (GValue         *expected,
                                GValue         *actual,
                                const gchar    *expression_expected,
                                const gchar    *expression_actual)
{
    if (gcut_value_equal(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;
        const gchar *expected_type_name, *actual_type_name;

        inspected_expected = cut_take_string(g_strdup_value_contents(expected));
        inspected_actual = cut_take_string(g_strdup_value_contents(actual));
        expected_type_name = g_type_name(G_VALUE_TYPE(expected));
        actual_type_name = g_type_name(G_VALUE_TYPE(actual));

        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s> (%s)\n"
                                  "  actual: <%s> (%s)",
                                  expression_expected, expression_actual,
                                  inspected_expected, expected_type_name,
                                  inspected_actual, actual_type_name);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_list_helper (const GList    *expected,
                               const GList    *actual,
                               GEqualFunc      equal_function,
                               GCutInspectFunction inspect_function,
                               gpointer        inspect_user_data,
                               const gchar    *expression_expected,
                               const gchar    *expression_actual,
                               const gchar    *expression_equal_function)
{
    if (gcut_list_equal(expected, actual, equal_function)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string(gcut_list_inspect(expected,
                                              inspect_function,
                                              inspect_user_data));
        inspected_actual =
            cut_take_string(gcut_list_inspect(actual,
                                              inspect_function,
                                              inspect_user_data));

        message = cut_take_printf("<%s(%s[i], %s[i]) == TRUE>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_equal_function,
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_list_int_helper (const GList    *expected,
                                   const GList    *actual,
                                   const gchar    *expression_expected,
                                   const gchar    *expression_actual)
{
    if (gcut_list_equal_int(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected = cut_take_string(gcut_list_inspect_int(expected));
        inspected_actual = cut_take_string(gcut_list_inspect_int(actual));

        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_list_uint_helper (const GList    *expected,
                                    const GList    *actual,
                                    const gchar    *expression_expected,
                                    const gchar    *expression_actual)
{
    if (gcut_list_equal_uint(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected = cut_take_string(gcut_list_inspect_uint(expected));
        inspected_actual = cut_take_string(gcut_list_inspect_uint(actual));

        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_list_string_helper (const GList    *expected,
                                      const GList    *actual,
                                      const gchar    *expression_expected,
                                      const gchar    *expression_actual)
{
    if (gcut_list_equal_string(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected = cut_take_string(gcut_list_inspect_string(expected));
        inspected_actual = cut_take_string(gcut_list_inspect_string(actual));

        message = cut_take_printf("<%s == %s>",
                                  expression_expected, expression_actual);
        cut_set_expected(inspected_expected);
        cut_set_actual(inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_list_enum_helper (GType           type,
                                    const GList    *expected,
                                    const GList    *actual,
                                    const gchar    *expression_type,
                                    const gchar    *expression_expected,
                                    const gchar    *expression_actual)
{
    if (gcut_list_equal_int(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string(gcut_list_inspect_enum(type, expected));
        inspected_actual =
            cut_take_string(gcut_list_inspect_enum(type, actual));

        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_list_flags_helper (GType           type,
                                     const GList    *expected,
                                     const GList    *actual,
                                     const gchar    *expression_type,
                                     const gchar    *expression_expected,
                                     const gchar    *expression_actual)
{
    if (gcut_list_equal_uint(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string(gcut_list_inspect_flags(type, expected));
        inspected_actual =
            cut_take_string(gcut_list_inspect_flags(type, actual));

        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_list_object_helper (const GList    *expected,
                                      const GList    *actual,
                                      GEqualFunc      equal_function,
                                      const gchar    *expression_expected,
                                      const gchar    *expression_actual)
{
    if (gcut_list_equal(expected, actual, equal_function)) {
        cut_test_pass();
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string(gcut_list_inspect_object(expected));
        inspected_actual =
            cut_take_string(gcut_list_inspect_object(actual));

        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_hash_table_helper (GHashTable  *expected,
                                     GHashTable  *actual,
                                     GEqualFunc   equal_function,
                                     GCutInspectFunction key_inspect_function,
                                     GCutInspectFunction value_inspect_function,
                                     gpointer     inspect_user_data,
                                     const gchar *expression_expected,
                                     const gchar *expression_actual,
                                     const gchar *expression_equal_function)
{
    if (gcut_hash_table_equal(expected, actual, equal_function)) {
        cut_test_pass();
    } else {
        const gchar *message;
        gchar *inspected_expected, *inspected_actual;
        GCompareFunc compare_function = NULL;

        if (key_inspect_function == gcut_inspect_direct) {
            compare_function = cut_utils_compare_direct;
        } else if (key_inspect_function == gcut_inspect_string) {
            compare_function = cut_utils_compare_string;
        }
        inspected_expected =
            gcut_hash_table_inspect_sorted(expected,
                                           key_inspect_function,
                                           value_inspect_function,
                                           compare_function,
                                           inspect_user_data);
        inspected_actual =
            gcut_hash_table_inspect_sorted(actual,
                                           key_inspect_function,
                                           value_inspect_function,
                                           compare_function,
                                           inspect_user_data);
        message = cut_take_printf("<%s(%s[key], %s[key]) == TRUE>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_equal_function,
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        g_free(inspected_expected);
        g_free(inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_hash_table_string_string_helper (GHashTable  *expected,
                                                   GHashTable  *actual,
                                                   const gchar *expression_expected,
                                                   const gchar *expression_actual)
{
    if (gcut_hash_table_string_equal(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *message;
        gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            gcut_hash_table_string_string_inspect(expected);
        inspected_actual =
            gcut_hash_table_string_string_inspect(actual);
        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected, expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        g_free(inspected_expected);
        g_free(inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_error_helper (GError         *error,
                          const gchar    *expression_error)
{
    if (error == NULL) {
        cut_test_pass();
    } else {
        const gchar *inspected;

        inspected = cut_take_string(gcut_error_inspect(error));
        g_clear_error(&error);
        cut_test_fail(cut_take_printf("expected: <%s> is NULL\n"
                                      "  actual: <%s>",
                                      expression_error, inspected));
    }
}

void
gcut_assert_equal_error_helper (const GError   *expected,
                                const GError   *actual,
                                const gchar    *expression_expected,
                                const gchar    *expression_actual)
{
    if (gcut_error_equal(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *inspected_expected;
        const gchar *inspected_actual;
        const gchar *message;

        inspected_expected = cut_take_string(gcut_error_inspect(expected));
        inspected_actual = cut_take_string(gcut_error_inspect(actual));
        message = cut_take_printf("<%s == %s>\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected,
                                  expression_actual,
                                  inspected_expected,
                                  inspected_actual);
        if (expected && actual)
            message = cut_append_diff(message,
                                      inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_remove_path_helper (const gchar *path, ...)
{
    GError *remove_path_error = NULL;
    gchar *full_path;
    va_list args;

    va_start(args, path);
    full_path = cut_utils_build_path_va_list(path, args);
    va_end(args);

    cut_utils_remove_path_recursive(full_path, &remove_path_error);
    gcut_assert_error_helper(remove_path_error, cut_take_string(full_path));
}

void
gcut_assert_equal_time_val_helper (GTimeVal       *expected,
                                   GTimeVal       *actual,
                                   const gchar    *expression_expected,
                                   const gchar    *expression_actual)
{
    const gchar *expected_string = NULL;
    const gchar *actual_string = NULL;

    if (expected)
        expected_string = cut_take_string(g_time_val_to_iso8601(expected));
    if (actual)
        actual_string = cut_take_string(g_time_val_to_iso8601(actual));

    cut_assert_equal_string_helper(expected_string,
                                   actual_string,
                                   expression_expected,
                                   expression_actual);
}

void
gcut_assert_equal_enum_helper (GType           enum_type,
                               gint            expected,
                               gint            actual,
                               const gchar    *expression_enum_type,
                               const gchar    *expression_expected,
                               const gchar    *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        const gchar *inspected_expected;
        const gchar *inspected_actual;
        const gchar *message;

        inspected_expected =
            cut_take_string(gcut_enum_inspect(enum_type, expected));
        inspected_actual =
            cut_take_string(gcut_enum_inspect(enum_type, actual));
        message = cut_take_printf("<%s == %s> (%s)\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected,
                                  expression_actual,
                                  expression_enum_type,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_flags_helper (GType           flags_type,
                                gint            expected,
                                gint            actual,
                                const gchar    *expression_flags_type,
                                const gchar    *expression_expected,
                                const gchar    *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        const gchar *inspected_expected;
        const gchar *inspected_actual;
        const gchar *message;

        inspected_expected =
            cut_take_string(gcut_flags_inspect(flags_type, expected));
        inspected_actual =
            cut_take_string(gcut_flags_inspect(flags_type, actual));
        message = cut_take_printf("<%s == %s> (%s)\n"
                                  "expected: <%s>\n"
                                  "  actual: <%s>",
                                  expression_expected,
                                  expression_actual,
                                  expression_flags_type,
                                  inspected_expected,
                                  inspected_actual);
        message = cut_append_diff(message, inspected_expected, inspected_actual);
        cut_test_fail(message);
    }
}

void
gcut_assert_equal_object_helper (GObject        *expected,
                                 GObject        *actual,
                                 GEqualFunc      equal_function,
                                 const gchar    *expression_expected,
                                 const gchar    *expression_actual,
                                 const gchar    *expression_equal_function)
{
    if (gcut_object_equal(expected, actual, equal_function)) {
        cut_test_pass();
    } else {
        gchar *inspected_expected, *inspected_actual;
        GString *message;
        const gchar *fail_message;

        inspected_expected = gcut_object_inspect(expected);
        inspected_actual = gcut_object_inspect(actual);
        message = g_string_new(NULL);

        if (expression_equal_function)
            g_string_append_printf(message,
                                   "<%s(%s, %s)>\n",
                                   expression_equal_function,
                                   expression_expected, expression_actual);
        else
            g_string_append_printf(message,
                                   "<%s == %s>\n",
                                   expression_expected, expression_actual);
        g_string_append_printf(message,
                               "expected: <%s>\n"
                               "  actual: <%s>",
                               inspected_expected,
                               inspected_actual);
        if (expected && actual)
            fail_message = cut_append_diff(message->str,
                                           inspected_expected,
                                           inspected_actual);
        else
            fail_message = cut_take_strdup(message->str);
        g_free(inspected_expected);
        g_free(inspected_actual);
        g_string_free(message, TRUE);

        cut_test_fail(fail_message);
    }
}

void
gcut_assert_equal_int64_helper (gint64          expected,
                                gint64          actual,
                                const char     *expression_expected,
                                const char     *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%" G_GINT64_FORMAT ">\n"
                                      "  actual: <%" G_GINT64_FORMAT ">",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
gcut_assert_equal_uint64_helper (guint64         expected,
                                 guint64         actual,
                                 const char     *expression_expected,
                                 const char     *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%" G_GUINT64_FORMAT ">\n"
                                      "  actual: <%" G_GUINT64_FORMAT ">",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

#ifdef G_OS_WIN32
#define GCUT_GPID_FORMAT "p"
#else
#define GCUT_GPID_FORMAT "d"
#endif
void
gcut_assert_equal_pid_helper (GPid            expected,
                              GPid            actual,
                              const char     *expression_expected,
                              const char     *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%" GCUT_GPID_FORMAT ">\n"
                                      "  actual: <%" GCUT_GPID_FORMAT ">",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
gcut_assert_not_equal_pid_helper (GPid            expected,
                                  GPid            actual,
                                  const char     *expression_expected,
                                  const char     *expression_actual)
{
    if (expected != actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s != %s>\n"
                                      "expected: <%" GCUT_GPID_FORMAT ">\n"
                                      "  actual: <%" GCUT_GPID_FORMAT ">",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
