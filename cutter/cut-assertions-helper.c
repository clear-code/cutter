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
#include <errno.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gstdio.h>

#include <errno.h>

#include "cut-helper.h"
#include <gcutter/gcut-assertions-helper.h>

void
cut_assert_helper (cut_boolean     result,
                   const char     *expression)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> is neither FALSE nor NULL",
                                      expression));
    }
}

void
cut_assert_true_helper (cut_boolean     result,
                        const char     *expression)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> is TRUE value",
                                      expression));
    }
}

void
cut_assert_false_helper (cut_boolean     result,
                         const char     *expression)
{
    if (result) {
        cut_test_fail(cut_take_printf("expected: <%s> is FALSE/NULL",
                                      expression));
    } else {
        cut_test_pass();
    }
}

void
cut_assert_equal_boolean_helper (cut_boolean     expected,
                                 cut_boolean     actual,
                                 const char     *expression_expected,
                                 const char     *expression_actual)
{
    if ((expected && actual) || (!expected && !actual)) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%s>\n"
                                      "  actual: <%s>",
                                      expression_expected,
                                      expression_actual,
                                      expected ? "true" : "false",
                                      actual ? "true" : "false"));
    }
}

void
cut_assert_not_equal_boolean_helper (cut_boolean     expected,
                                     cut_boolean     actual,
                                     const char     *expression_expected,
                                     const char     *expression_actual)
{
    if ((expected && actual) || (!expected && !actual)) {
        cut_test_fail(cut_take_printf("<%s != %s>\n"
                                      "expected: <%s>\n"
                                      "  actual: <%s>",
                                      expression_expected,
                                      expression_actual,
                                      expected ? "true" : "false",
                                      actual ? "true" : "false"));
    } else {
        cut_test_pass();
    }
}

void
cut_assert_null_helper (const void     *object,
                        const char     *expression)
{
    if (object == NULL) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> is NULL",
                                      expression));
    }
}

void
cut_assert_null_string_helper (const char     *string,
                               const char     *expression)
{
    if (string == NULL) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> is NULL\n"
                                      "  actual: <%s>",
                                      expression, string));
    }
}

void
cut_assert_not_null_helper (const void     *object,
                            const char     *expression)
{
    if (object != NULL) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> is not NULL",
                                      expression));
    }
}

void
cut_assert_equal_int_helper (long            expected,
                             long            actual,
                             const char     *expression_expected,
                             const char     *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%ld>\n"
                                      "  actual: <%ld>",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
cut_assert_not_equal_int_helper (long            expected,
                                 long            actual,
                                 const char     *expression_expected,
                                 const char     *expression_actual)
{
    if (expected != actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s != %s>\n"
                                      "expected: <%ld>\n"
                                      "  actual: <%ld>",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
cut_assert_equal_uint_helper (unsigned long   expected,
                              unsigned long   actual,
                              const char     *expression_expected,
                              const char     *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%lu>\n"
                                      "  actual: <%lu>",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
cut_assert_not_equal_uint_helper (unsigned long   expected,
                                  unsigned long   actual,
                                  const char     *expression_expected,
                                  const char     *expression_actual)
{
    if (expected != actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s != %s>\n"
                                      "expected: <%lu>\n"
                                      "  actual: <%lu>",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
cut_assert_equal_size_helper (size_t          expected,
                              size_t          actual,
                              const char     *expression_expected,
                              const char     *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%" G_GSIZE_FORMAT ">\n"
                                      "  actual: <%" G_GSIZE_FORMAT ">",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
cut_assert_not_equal_size_helper (size_t          expected,
                                  size_t          actual,
                                  const char     *expression_expected,
                                  const char     *expression_actual)
{
    if (expected != actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s != %s>\n"
                                      "expected: <%" G_GSIZE_FORMAT ">\n"
                                      "  actual: <%" G_GSIZE_FORMAT ">",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
cut_assert_equal_double_helper (double          expected,
                                double          error,
                                double          actual,
                                const char     *expression_expected,
                                const char     *expression_error,
                                const char     *expression_actual)
{
    if (cut_utils_equal_double(expected, actual, error)) {
        cut_test_pass();
    } else {
        double min, max;
        const gchar *relation;

        if (error > 0) {
            min = expected - error;
            max = expected + error;
        } else {
            min = expected + error;
            max = expected - error;
        }

        if (actual < min)
            relation = "actual < min < max";
        else
            relation = "min < max < actual";

        cut_test_fail(cut_take_printf("<%s-%s <= %s <= %s+%s>\n"
                                      "expected: <%g>\n"
                                      "   error: <%g>\n"
                                      "     min: <%g>\n"
                                      "     max: <%g>\n"
                                      "  actual: <%g>\n"
                                      "relation: <%s>",
                                      expression_expected,
                                      expression_error,
                                      expression_actual,
                                      expression_expected,
                                      expression_error,
                                      expected,
                                      error,
                                      min,
                                      max,
                                      actual,
                                      relation));
    }
}

void
cut_assert_not_equal_double_helper (double          expected,
                                    double          error,
                                    double          actual,
                                    const char     *expression_expected,
                                    const char     *expression_error,
                                    const char     *expression_actual)
{
    if (!cut_utils_equal_double(expected, actual, error)) {
        cut_test_pass();
    } else {
        double min, max;

        if (error > 0) {
            min = expected - error;
            max = expected + error;
        } else {
            min = expected + error;
            max = expected - error;
        }

        cut_test_fail(cut_take_printf("<(%s < %s-%s) && (%s+%s < %s)>\n"
                                      "expected: <%g>\n"
                                      "   error: <%g>\n"
                                      "     min: <%g>\n"
                                      "     max: <%g>\n"
                                      "  actual: <%g>\n"
                                      "relation: <min < actual < max>",
                                      expression_actual,
                                      expression_expected,
                                      expression_error,
                                      expression_expected,
                                      expression_error,
                                      expression_actual,
                                      expected,
                                      error,
                                      min,
                                      max,
                                      actual));
    }
}

void
cut_assert_equal_string_helper (const char     *expected,
                                const char     *actual,
                                const char     *expression_expected,
                                const char     *expression_actual)
{
    if (expected == NULL) {
        if (actual == NULL) {
            cut_test_pass();
        } else {
            cut_test_fail(cut_take_printf("expected: <%s> is NULL\n"
                                          "  actual: <%s>",
                                          expression_actual,
                                          actual));
        }
    } else {
        if (cut_utils_equal_string(expected, actual)) {
            cut_test_pass();
        } else {
            const char *message;

            message = cut_take_printf("<%s == %s>",
                                      expression_expected,
                                      expression_actual);
            cut_set_expected(expected);
            cut_set_actual(actual);
            cut_test_fail(message);
        }
    }
}

void
cut_assert_not_equal_string_helper (const char     *expected,
                                    const char     *actual,
                                    const char     *expression_expected,
                                    const char     *expression_actual)
{
    if (expected == NULL) {
        if (actual) {
            cut_test_pass();
        } else {
            cut_test_fail(cut_take_printf("expected: <%s> is not NULL\n"
                                          "  actual: <%s>",
                                          expression_actual,
                                          actual));
        }
    } else {
        if (!cut_utils_equal_string(expected, actual)) {
            cut_test_pass();
        } else {
            const char *message;

            message = cut_take_printf("<%s != %s>",
                                      expression_expected,
                                      expression_actual);
            cut_set_expected(expected);
            cut_set_actual(actual);
            cut_test_fail(message);
        }
    }
}

void
cut_assert_equal_substring_helper (const char     *expected,
                                   const char     *actual,
                                   size_t          length,
                                   const char     *expression_expected,
                                   const char     *expression_actual,
                                   const char     *expression_length)
{
    if (expected == NULL) {
        if (actual == NULL) {
            cut_test_pass();
        } else {
            const gchar *actual_substring;

            actual_substring = cut_take_string(g_strndup(actual, length));
            cut_test_fail(cut_take_printf("expected: <%s> is NULL\n"
                                          "  actual: <%s>",
                                          expression_actual,
                                          actual_substring));
        }
    } else {
        if (cut_utils_equal_substring(expected, actual, length)) {
            cut_test_pass();
        } else {
            const gchar *actual_substring;
            const char *message;

            actual_substring = cut_take_string(g_strndup(actual, length));
            message =
                cut_take_printf("<%s == (%s)[0..%s]>",
                                expression_expected,
                                expression_actual,
                                expression_length);
            cut_set_expected(expected);
            cut_set_actual(actual_substring);
            cut_test_fail(message);
        }
    }
}

void
cut_assert_not_equal_substring_helper (const char     *expected,
                                       const char     *actual,
                                       size_t          length,
                                       const char     *expression_expected,
                                       const char     *expression_actual,
                                       const char     *expression_length)
{
    if (expected == NULL) {
        if (actual) {
            cut_test_pass();
        } else {
            const gchar *actual_substring;

            actual_substring = cut_take_string(g_strndup(actual, length));
            cut_test_fail(cut_take_printf("expected: <%s> is not NULL\n"
                                          "  actual: <%s>",
                                          expression_actual,
                                          actual_substring));
        }
    } else {
        if (!cut_utils_equal_substring(expected, actual, length)) {
            cut_test_pass();
        } else {
            const gchar *actual_substring = NULL;
            const char *message;

            if (actual)
                actual_substring = cut_take_string(g_strndup(actual, length));
            message =
                cut_take_printf("<%s != (%s)[0..%s]>\n"
                                "expected: <%s>\n"
                                "  actual: <%s>",
                                expression_expected,
                                expression_actual,
                                expression_length,
                                cut_utils_inspect_string(expected),
                                cut_utils_inspect_string(actual_substring));
            if (expected && actual_substring)
                message = cut_append_diff(message, expected, actual_substring);
            cut_test_fail(message);
        }
    }
}

void
cut_assert_equal_memory_helper (const void     *expected,
                                size_t          expected_size,
                                const void     *actual,
                                size_t          actual_size,
                                const char     *expression_expected,
                                const char     *expression_expected_size,
                                const char     *expression_actual,
                                const char     *expression_actual_size)
{
    if (expected_size == actual_size &&
        memcmp(expected, actual, expected_size) == 0) {
        cut_test_pass();
    } else {
        const char *message;
        const char *inspected_expected;
        const char *inspected_actual;
        gsize max_diff_target_size = 8092;

        inspected_expected =
            cut_take_string(cut_utils_inspect_memory(expected, expected_size));
        inspected_actual =
            cut_take_string(cut_utils_inspect_memory(actual, actual_size));
        message = cut_take_printf(
            "<%s(size: %s) == %s(size: %s)>\n"
            "expected: <%s (size: %" G_GSIZE_FORMAT ")>\n"
            "  actual: <%s (size: %" G_GSIZE_FORMAT ")>",
            expression_expected,
            expression_expected_size,
            expression_actual,
            expression_actual_size,
            inspected_expected,
            expected_size,
            inspected_actual,
            actual_size);
        if (0 < expected_size && expected_size < max_diff_target_size &&
            0 < actual_size && actual_size < max_diff_target_size)
            message = cut_append_diff(message,
                                      inspected_expected,
                                      inspected_actual);
        cut_test_fail(message);
    }
}

void
cut_assert_not_equal_memory_helper (const void     *expected,
                                    size_t          expected_size,
                                    const void     *actual,
                                    size_t          actual_size,
                                    const char     *expression_expected,
                                    const char     *expression_expected_size,
                                    const char     *expression_actual,
                                    const char     *expression_actual_size)
{
    if ((expected_size != actual_size) ||
        memcmp(expected, actual, expected_size) != 0) {
        cut_test_pass();
    } else {
        const char *message;
        const char *inspected_expected;
        const char *inspected_actual;
        gsize max_diff_target_size = 8092;

        inspected_expected =
            cut_take_string(cut_utils_inspect_memory(expected, expected_size));
        inspected_actual =
            cut_take_string(cut_utils_inspect_memory(actual, actual_size));
        message = cut_take_printf(
            "<%s(size: %s) != %s(size: %s)>\n"
            "expected: <%s (size: %" G_GSIZE_FORMAT ")>\n"
            "  actual: <%s (size: %" G_GSIZE_FORMAT ")>",
            expression_expected,
            expression_expected_size,
            expression_actual,
            expression_actual_size,
            inspected_expected,
            expected_size,
            inspected_actual,
            actual_size);
        if (0 < expected_size && expected_size < max_diff_target_size &&
            0 < actual_size && actual_size < max_diff_target_size)
            message = cut_append_diff(message,
                                      inspected_expected,
                                      inspected_actual);
        cut_test_fail(message);
    }
}

void
cut_assert_equal_string_array_helper (char          **expected,
                                      char          **actual,
                                      const char     *expression_expected,
                                      const char     *expression_actual)
{
    if (expected && actual &&
        cut_utils_equal_string_array(expected, actual)) {
        cut_test_pass();
    } else {
        const gchar *inspected_expected;
        const gchar *inspected_actual;
        const gchar *message;

        inspected_expected = cut_inspect_string_array(expected);
        inspected_actual = cut_inspect_string_array(actual);
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
cut_assert_operator_helper (cut_boolean     result,
                            const char     *expression_lhs,
                            const char     *expression_operator,
                            const char     *expression_rhs)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s %s %s> is TRUE",
                                      expression_lhs,
                                      expression_operator,
                                      expression_rhs));
    }
}

void
cut_assert_operator_int_helper (cut_boolean     result,
                                long            lhs,
                                long            rhs,
                                const char     *expression_lhs,
                                const char     *expression_operator,
                                const char     *expression_rhs)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> %s <%s>\n"
                                      "  actual: <%ld> %s <%ld>",
                                      expression_lhs,
                                      expression_operator,
                                      expression_rhs,
                                      lhs,
                                      expression_operator,
                                      rhs));
    }
}

void
cut_assert_operator_uint_helper (cut_boolean     result,
                                 unsigned long   lhs,
                                 unsigned long   rhs,
                                 const char     *expression_lhs,
                                 const char     *expression_operator,
                                 const char     *expression_rhs)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> %s <%s>\n"
                                      "  actual: <%lu> %s <%lu>",
                                      expression_lhs,
                                      expression_operator,
                                      expression_rhs,
                                      lhs,
                                      expression_operator,
                                      rhs));
    }
}

void
cut_assert_operator_size_helper (cut_boolean     result,
                                 size_t          lhs,
                                 size_t          rhs,
                                 const char     *expression_lhs,
                                 const char     *expression_operator,
                                 const char     *expression_rhs)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> %s <%s>\n"
                                      "  actual: "
                                      "<%" G_GSIZE_FORMAT ">"
                                      " %s "
                                      "<%" G_GSIZE_FORMAT ">",
                                      expression_lhs,
                                      expression_operator,
                                      expression_rhs,
                                      lhs,
                                      expression_operator,
                                      rhs));
    }
}

void
cut_assert_operator_double_helper (cut_boolean     result,
                                   double          lhs,
                                   double          rhs,
                                   const char     *expression_lhs,
                                   const char     *expression_operator,
                                   const char     *expression_rhs)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s> %s <%s>\n"
                                      "  actual: <%g> %s <%g>",
                                      expression_lhs,
                                      expression_operator,
                                      expression_rhs,
                                      lhs,
                                      expression_operator,
                                      rhs));
    }
}

void
cut_assert_equal_helper (cut_boolean     result,
                         const char     *expression_function,
                         const char     *expression_expected,
                         const char     *expression_actual)
{
    if (result) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <%s(%s, %s)> is TRUE",
                                      expression_function,
                                      expression_expected,
                                      expression_actual));
    }
}

void
cut_assert_errno_helper (void)
{
    int current_errno = errno;

    if (current_errno == 0) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("expected: <0> (errno)\n"
                                      "  actual: <%d> (%s)",
                                      current_errno,
                                      g_strerror(current_errno)));
    }
}

void
cut_assert_path_exist_helper (const char     *path,
                              const char     *expression_path)
{
    if (!path) {
        cut_test_fail(cut_take_printf("<%s>\n"
                                      "expected: <%s> "
                                      "should not be NULL",
                                      expression_path,
                                      path));
    } else if (cut_utils_path_exist(path)) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s>\n"
                                      "expected: <%s> exists",
                                      expression_path,
                                      path));
    }
}

void
cut_assert_path_not_exist_helper (const char     *path,
                                  const char     *expression_path)
{
    if (!path) {
        cut_test_fail(cut_take_printf("<%s>\n"
                                      "expected: <%s> "
                                      "should not be NULL",
                                      expression_path,
                                      path));
    } else if (!cut_utils_path_exist(path)) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s>\n"
                                      "expected: <%s> "
                                      "doesn't exist",
                                      expression_path,
                                      path));
    }
}

void
cut_assert_match_helper (const char     *pattern,
                         const char     *actual,
                         const char     *expression_pattern,
                         const char     *expression_actual)
{
    if (cut_utils_regex_match(pattern, actual)) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s> =~ <%s>\n"
                                      " pattern: <%s>\n"
                                      "  actual: <%s>",
                                      expression_pattern,
                                      expression_actual,
                                      pattern, actual));
    }
}

void
cut_assert_equal_pointer_helper (const void     *expected,
                                 const void     *actual,
                                 const char     *expression_expected,
                                 const char     *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        cut_test_fail(cut_take_printf("<%s == %s>\n"
                                      "expected: <%p>\n"
                                      "  actual: <%p>",
                                      expression_expected,
                                      expression_actual,
                                      expected, actual));
    }
}

void
cut_assert_equal_fixture_data_string_helper (const void     *expected,
                                             const gchar    *expression_expected,
                                             const void     *path,
                                             ...)
{
    GError *error = NULL;
    const char *data;
    char *full_path;
    va_list args;

    va_start(args, path);
    data = cut_utils_get_fixture_data_string_va_list(
        cut_get_current_test_context(), &full_path, path, args);
    cut_test_context_keep_user_message(cut_get_current_test_context());
    gcut_assert_error_helper(error, expression_expected);
    cut_assert_equal_string_helper(expected, data,
                                   expression_expected,
                                   cut_take_string(full_path));
}

void
cut_error_errno_helper (void)
{
    int current_errno = errno;

    if (current_errno != 0) {
        cut_test_terminate(ERROR,
                           cut_take_printf("<%d> (%s)",
                                           current_errno,
                                           g_strerror(current_errno)));
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
