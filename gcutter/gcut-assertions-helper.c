/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gstdio.h>

#include "gcut-assertions-helper.h"

void
gcut_assert_equal_type_helper (CutTestContext *test_context,
                               GType           expected,
                               GType           actual,
                               const gchar    *expression_expected,
                               const gchar    *expression_actual,
                               const gchar    *user_message_format,
                               ...)
{
    if (expected == actual) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s == %s>\n"
                                   "expected: <%s>\n"
                                   "  actual: <%s>",
                                   expression_expected,
                                   expression_actual,
                                   g_type_name(expected),
                                   g_type_name(actual)),
            user_message_format);
    }
}

void
gcut_assert_equal_value_helper (CutTestContext *test_context,
                                GValue         *expected,
                                GValue         *actual,
                                const gchar    *expression_expected,
                                const gchar    *expression_actual,
                                const gchar    *user_message_format,
                                ...)
{
    if (gcut_value_equal(expected, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;
        const gchar *expected_type_name, *actual_type_name;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   g_strdup_value_contents(expected));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   g_strdup_value_contents(actual));
        expected_type_name = g_type_name(G_VALUE_TYPE(expected));
        actual_type_name = g_type_name(G_VALUE_TYPE(actual));

        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s> (%s)\n"
                                         "  actual: <%s> (%s)",
                                         expression_expected, expression_actual,
                                         inspected_expected, expected_type_name,
                                         inspected_actual, actual_type_name);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);

        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_equal_list_int_helper (CutTestContext *test_context,
                                   const GList    *expected,
                                   const GList    *actual,
                                   const gchar    *expression_expected,
                                   const gchar    *expression_actual,
                                   const gchar    *user_message_format,
                                   ...)
{
    if (gcut_list_int_equal(expected, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_list_int_inspect(expected));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_list_int_inspect(actual));

        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s>\n"
                                         "  actual: <%s>",
                                         expression_expected, expression_actual,
                                         inspected_expected,
                                         inspected_actual);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);
        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_equal_list_uint_helper (CutTestContext *test_context,
                                    const GList    *expected,
                                    const GList    *actual,
                                    const gchar    *expression_expected,
                                    const gchar    *expression_actual,
                                    const gchar    *user_message_format,
                                    ...)
{
    if (gcut_list_uint_equal(expected, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_list_uint_inspect(expected));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_list_uint_inspect(actual));

        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s>\n"
                                         "  actual: <%s>",
                                         expression_expected, expression_actual,
                                         inspected_expected,
                                         inspected_actual);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);
        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_equal_list_string_helper (CutTestContext *test_context,
                                      const GList    *expected,
                                      const GList    *actual,
                                      const gchar    *expression_expected,
                                      const gchar    *expression_actual,
                                      const gchar    *user_message_format,
                                      ...)
{
    if (gcut_list_string_equal(expected, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_list_string_inspect(expected));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_list_string_inspect(actual));

        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s>\n"
                                         "  actual: <%s>",
                                         expression_expected, expression_actual,
                                         inspected_expected,
                                         inspected_actual);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);
        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_equal_list_enum_helper (CutTestContext *test_context,
                                    GType           type,
                                    const GList    *expected,
                                    const GList    *actual,
                                    const gchar    *expression_type,
                                    const gchar    *expression_expected,
                                    const gchar    *expression_actual,
                                    const gchar    *user_message_format,
                                    ...)
{
    if (gcut_list_equal(expected, actual, g_direct_equal)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_list_enum_inspect(expected, type));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_list_enum_inspect(actual, type));

        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s>\n"
                                         "  actual: <%s>",
                                         expression_expected, expression_actual,
                                         inspected_expected,
                                         inspected_actual);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);
        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_equal_list_flags_helper (CutTestContext *test_context,
                                     GType           type,
                                     const GList    *expected,
                                     const GList    *actual,
                                     const gchar    *expression_type,
                                     const gchar    *expression_expected,
                                     const gchar    *expression_actual,
                                     const gchar    *user_message_format,
                                     ...)
{
    if (gcut_list_equal(expected, actual, g_direct_equal)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_list_flags_inspect(expected, type));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_list_flags_inspect(actual, type));

        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s>\n"
                                         "  actual: <%s>",
                                         expression_expected, expression_actual,
                                         inspected_expected,
                                         inspected_actual);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);
        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_equal_list_object_helper (CutTestContext *test_context,
                                      const GList    *expected,
                                      const GList    *actual,
                                      GEqualFunc      equal_function,
                                      const gchar    *expression_expected,
                                      const gchar    *expression_actual,
                                      const gchar    *user_message_format,
                                      ...)
{
    if (gcut_list_equal(expected, actual, equal_function)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        const gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_list_object_inspect(expected));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_list_object_inspect(actual));

        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s>\n"
                                         "  actual: <%s>",
                                         expression_expected, expression_actual,
                                         inspected_expected,
                                         inspected_actual);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);
        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_equal_hash_table_string_string_helper (
    CutTestContext *test_context,
    GHashTable     *expected,
    GHashTable     *actual,
    const gchar    *expression_expected,
    const gchar    *expression_actual,
    const gchar    *user_message_format,
    ...)
{
    if (gcut_hash_table_string_equal(expected, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *message;
        gchar *inspected_expected, *inspected_actual;

        inspected_expected =
            gcut_hash_table_string_string_inspect(expected);
        inspected_actual =
            gcut_hash_table_string_string_inspect(actual);
        message = cut_take_printf_helper(test_context,
                                         "<%s == %s>\n"
                                         "expected: <%s>\n"
                                         "  actual: <%s>",
                                         expression_expected, expression_actual,
                                         inspected_expected,
                                         inspected_actual);
        message = cut_append_diff_helper(test_context,
                                         message,
                                         inspected_expected,
                                         inspected_actual);
        g_free(inspected_expected);
        g_free(inspected_actual);
        cut_test_fail_va_list_helper(test_context, message, user_message_format);
    }
}

void
gcut_assert_error_helper (CutTestContext *test_context,
                          GError         *error,
                          const gchar    *expression_error,
                          const gchar    *user_message_format,
                          ...)
{
    if (error == NULL) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *inspected;

        inspected = cut_take_string_helper(test_context,
                                           gcut_error_inspect(error));
        g_error_free(error);
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s> is NULL\n"
                                   "  actual: <%s>",
                                   expression_error, inspected),
            user_message_format);
    }
}

void
gcut_assert_equal_error_helper (CutTestContext *test_context,
                                const GError   *expected,
                                const GError   *actual,
                                const gchar    *expression_expected,
                                const gchar    *expression_actual,
                                const gchar    *user_message_format,
                                ...)
{
    if (gcut_error_equal(expected, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context, gcut_error_inspect(expected));
        inspected_actual =
            cut_take_string_helper(test_context, gcut_error_inspect(actual));
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s == %s>\n"
                                   "expected: <%s>\n"
                                   "  actual: <%s>",
                                   expression_expected,
                                   expression_actual,
                                   inspected_expected,
                                   inspected_actual),
            user_message_format);
    }
}

void
gcut_assert_equal_enum_helper (CutTestContext *test_context,
                               GType           enum_type,
                               gint            expected,
                               gint            actual,
                               const gchar    *expression_enum_type,
                               const gchar    *expression_expected,
                               const gchar    *expression_actual,
                               const gchar    *user_message_format,
                               ...)
{
    if (expected == actual) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_enum_inspect(enum_type, expected));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_enum_inspect(enum_type, actual));
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s == %s> (%s)\n"
                                   "expected: <%s>\n"
                                   "  actual: <%s>",
                                   expression_expected,
                                   expression_actual,
                                   expression_enum_type,
                                   inspected_expected,
                                   inspected_actual),
            user_message_format);
    }
}

void
gcut_assert_equal_flags_helper (CutTestContext *test_context,
                                GType           flags_type,
                                gint            expected,
                                gint            actual,
                                const gchar    *expression_flags_type,
                                const gchar    *expression_expected,
                                const gchar    *expression_actual,
                                const gchar    *user_message_format,
                                ...)
{
    if (expected == actual) {
        cut_test_pass_helper(test_context);
    } else {
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   gcut_flags_inspect(flags_type, expected));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   gcut_flags_inspect(flags_type, actual));
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s == %s> (%s)\n"
                                   "expected: <%s>\n"
                                   "  actual: <%s>",
                                   expression_expected,
                                   expression_actual,
                                   expression_flags_type,
                                   inspected_expected,
                                   inspected_actual),
            user_message_format);
    }
}

void
gcut_assert_equal_object_helper (CutTestContext *test_context,
                                 GObject        *expected,
                                 GObject        *actual,
                                 GEqualFunc      equal_function,
                                 const gchar    *expression_expected,
                                 const gchar    *expression_actual,
                                 const gchar    *expression_equal_function,
                                 const gchar    *user_message_format,
                                 ...)
{
    if (gcut_object_equal(expected, actual, equal_function)) {
        cut_test_pass_helper(test_context);
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
            fail_message = cut_append_diff_helper(test_context,
                                                  message->str,
                                                  inspected_expected,
                                                  inspected_actual);
        else
            fail_message = cut_take_strdup_helper(test_context, message->str);
        g_free(inspected_expected);
        g_free(inspected_actual);
        g_string_free(message, TRUE);

        cut_test_fail_va_list_helper(test_context,
                                     fail_message,
                                     user_message_format);
    }
}

void
gcut_assert_equal_int64_helper (CutTestContext *test_context,
                                gint64          expected,
                                gint64          actual,
                                const char     *expression_expected,
                                const char     *expression_actual,
                                const char     *user_message_format,
                                ...)
{
    if (expected == actual) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s == %s>\n"
                                   "expected: <%" G_GINT64_FORMAT ">\n"
                                   "  actual: <%" G_GINT64_FORMAT ">",
                                   expression_expected,
                                   expression_actual,
                                   expected, actual),
            user_message_format);
    }
}

void
gcut_assert_equal_uint64_helper (CutTestContext *test_context,
                                 guint64         expected,
                                 guint64         actual,
                                 const char     *expression_expected,
                                 const char     *expression_actual,
                                 const char     *user_message_format,
                                 ...)
{
    if (expected == actual) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s == %s>\n"
                                   "expected: <%" G_GUINT64_FORMAT ">\n"
                                   "  actual: <%" G_GUINT64_FORMAT ">",
                                   expression_expected,
                                   expression_actual,
                                   expected, actual),
            user_message_format);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
