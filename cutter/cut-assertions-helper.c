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
#include <errno.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gstdio.h>

#include <errno.h>

#include "cut-helper.h"

void
cut_assert_helper (CutTestContext *test_context,
                   cut_boolean     result,
                   const char     *expression,
                   const char     *user_message_format,
                   ...)
{
    if (result) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s>"
                                   " is not FALSE/NULL",
                                   expression),
            user_message_format);
    }
}

void
cut_assert_true_helper (CutTestContext *test_context,
                        cut_boolean     result,
                        const char     *expression,
                        const char     *user_message_format,
                        ...)
{
    if (result) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s>"
                                   " is TRUE value",
                                   expression),
            user_message_format);
    }
}

void
cut_assert_false_helper (CutTestContext *test_context,
                         cut_boolean     result,
                         const char     *expression,
                         const char     *user_message_format,
                         ...)
{
    if (result) {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s>"
                                   " is FALSE/NULL",
                                   expression),
            user_message_format);
    } else {
        cut_test_pass_helper(test_context);
    }
}

void
cut_assert_null_helper (CutTestContext *test_context,
                        const void     *object,
                        const char     *expression,
                        const char     *user_message_format,
                        ...)
{
    if (object == NULL) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s> is NULL",
                                   expression),
            user_message_format);
    }
}

void
cut_assert_null_string_helper (CutTestContext *test_context,
                               const char     *string,
                               const char     *expression,
                               const char     *user_message_format,
                               ...)
{
    if (string == NULL) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s> is NULL\n"
                                   "  actual: <%s>",
                                   expression, string),
            user_message_format);
    }
}

void
cut_assert_not_null_helper (CutTestContext *test_context,
                            const void     *object,
                            const char     *expression,
                            const char     *user_message_format,
                            ...)
{
    if (object != NULL) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s> is not NULL",
                                   expression),
            user_message_format);
    }
}

void
cut_assert_equal_int_helper (CutTestContext *test_context,
                             long            expected,
                             long            actual,
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
                                   "expected: <%ld>\n"
                                   "  actual: <%ld>",
                                   expression_expected,
                                   expression_actual,
                                   expected, actual),
            user_message_format);
    }
}

void
cut_assert_equal_uint_helper (CutTestContext *test_context,
                              unsigned long   expected,
                              unsigned long   actual,
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
                                   "expected: <%lu>\n"
                                   "  actual: <%lu>",
                                   expression_expected,
                                   expression_actual,
                                   expected, actual),
            user_message_format);
    }
}

void
cut_assert_equal_double_helper (CutTestContext *test_context,
                                double          expected,
                                double          error,
                                double          actual,
                                const char     *expression_expected,
                                const char     *expression_error,
                                const char     *expression_actual,
                                const char     *user_message_format,
                                ...)
{
    if (cut_utils_equal_double(expected, actual, error)) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s-%s <= %s <= %s+%s>\n"
                                   "expected: <%g +/- %g>\n"
                                   "  actual: <%g>",
                                   expression_expected,
                                   expression_error,
                                   expression_actual,
                                   expression_expected,
                                   expression_error,
                                   expected, error,
                                   actual),
            user_message_format);
    }
}

void
cut_assert_equal_string_helper (CutTestContext *test_context,
                                const char     *expected,
                                const char     *actual,
                                const char     *expression_expected,
                                const char     *expression_actual,
                                const char     *user_message_format,
                                ...)
{
    if (expected == NULL) {
        if (actual == NULL) {
            cut_test_pass_helper(test_context);
        } else {
            const char *message;

            message = cut_take_printf_helper(test_context,
                                             "expected: <%s> is NULL\n"
                                             "  actual: <%s>",
                                             expression_actual,
                                             actual);
            cut_test_fail_va_list_helper(test_context,
                                         message, user_message_format);
        }
    } else {
        if (cut_utils_equal_string(expected, actual)) {
            cut_test_pass_helper(test_context);
        } else {
            const char *message;

            message = cut_take_printf_helper(test_context,
                                             "<%s == %s>\n"
                                             "expected: <%s>\n"
                                             "  actual: <%s>",
                                             expression_expected,
                                             expression_actual,
                                             cut_utils_inspect_string(expected),
                                             cut_utils_inspect_string(actual));
            if (expected && actual)
                message = cut_append_diff_helper(test_context,
                                                 message, expected, actual);
            cut_test_fail_va_list_helper(test_context,
                                         message, user_message_format);
        }
    }
}

void
cut_assert_equal_memory_helper (CutTestContext *test_context,
                                const void     *expected,
                                size_t          expected_size,
                                const void     *actual,
                                size_t          actual_size,
                                const char     *expression_expected,
                                const char     *expression_expected_size,
                                const char     *expression_actual,
                                const char     *expression_actual_size,
                                const char     *user_message_format,
                                ...)
{
    if (expected_size == actual_size &&
        memcmp(expected, actual, expected_size) == 0) {
        cut_test_pass_helper(test_context);
    } else {
        const char *message;
        const char *inspected_expected;
        const char *inspected_actual;

        inspected_expected =
            cut_take_string_helper(test_context,
                                   cut_utils_inspect_memory(expected,
                                                            expected_size));
        inspected_actual =
            cut_take_string_helper(test_context,
                                   cut_utils_inspect_memory(actual,
                                                            actual_size));
        message = cut_take_printf_helper(
            test_context,
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
        if (expected_size > 0 && actual_size > 0)
            message = cut_append_diff_helper(test_context,
                                             message,
                                             inspected_expected,
                                             inspected_actual);
        cut_test_fail_va_list_helper(test_context,
                                     message, user_message_format);
    }
}

void
cut_assert_equal_string_array_helper (CutTestContext *test_context,
                                      char          **expected,
                                      char          **actual,
                                      const char     *expression_expected,
                                      const char     *expression_actual,
                                      const char     *user_message_format,
                                      ...)
{
    if (expected && actual &&
        cut_utils_equal_string_array(expected, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        const char *inspected_expected;
        const char *inspected_actual;

        inspected_expected = cut_inspect_string_array_helper(test_context,
                                                             expected);
        inspected_actual = cut_inspect_string_array_helper(test_context,
                                                           actual);
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
cut_assert_operator_helper (CutTestContext *test_context,
                            cut_boolean     result,
                            const char     *expression_lhs,
                            const char     *expression_operator,
                            const char     *expression_rhs,
                            const char     *user_message_format,
                            ...)
{
    if (result) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s %s %s> "
                                   "is TRUE",
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs),
            user_message_format);
    }
}

void
cut_assert_operator_int_helper (CutTestContext *test_context,
                                cut_boolean     result,
                                long            lhs,
                                long            rhs,
                                const char     *expression_lhs,
                                const char     *expression_operator,
                                const char     *expression_rhs,
                                const char     *user_message_format,
                                ...)
{
    if (result) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s> %s <%s>\n"
                                   "  actual: <%ld> %s <%ld>",
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs,
                                   lhs,
                                   expression_operator,
                                   rhs),
            user_message_format);
    }
}

void
cut_assert_operator_double_helper (CutTestContext *test_context,
                                   cut_boolean     result,
                                   double          lhs,
                                   double          rhs,
                                   const char     *expression_lhs,
                                   const char     *expression_operator,
                                   const char     *expression_rhs,
                                   const char     *user_message_format,
                                   ...)
{
    if (result) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s> %s <%s>\n"
                                   "  actual: <%g> %s <%g>",
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs,
                                   lhs,
                                   expression_operator,
                                   rhs),
            user_message_format);
    }
}

void
cut_assert_equal_helper (CutTestContext *test_context,
                         cut_boolean     result,
                         const char     *expression_function,
                         const char     *expression_expected,
                         const char     *expression_actual,
                         const char     *user_message_format,
                         ...)
{
    if (result) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <%s(%s, %s)> "
                                   "is TRUE",
                                   expression_function,
                                   expression_expected,
                                   expression_actual),
            user_message_format);
    }
}

void
cut_assert_errno_helper (CutTestContext *test_context,
                         const char     *user_message_format,
                         ...)
{
    int current_errno = errno;

    if (current_errno == 0) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "expected: <0> (errno)\n"
                                   "  actual: <%d> (%s)",
                                   current_errno,
                                   strerror(current_errno)),
            user_message_format);
    }
}

void
cut_assert_path_exist_helper (CutTestContext *test_context,
                              const char     *path,
                              const char     *expression_path,
                              const char     *user_message_format,
                              ...)
{
    if (!path) {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s>\n"
                                   "expected: <%s> "
                                   "should not be NULL",
                                   expression_path,
                                   path),
            user_message_format);
    } else if (cut_utils_path_exist(path)) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s>\n"
                                   "expected: <%s> exists",
                                   expression_path,
                                   path),
            user_message_format);
    }
}

void
cut_assert_path_not_exist_helper (CutTestContext *test_context,
                                  const char     *path,
                                  const char     *expression_path,
                                  const char     *user_message_format,
                                  ...)
{
    if (!path) {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s>\n"
                                   "expected: <%s> "
                                   "should not be NULL",
                                   expression_path,
                                   path),
            user_message_format);
    } else if (!cut_utils_path_exist(path)) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s>\n"
                                   "expected: <%s> "
                                   "doesn't exist",
                                   expression_path,
                                   path),
            user_message_format);
    }
}

void
cut_assert_match_helper (CutTestContext *test_context,
                         const char     *pattern,
                         const char     *actual,
                         const char     *expression_pattern,
                         const char     *expression_actual,
                         const char     *user_message_format,
                         ...)
{
    if (cut_utils_regex_match(pattern, actual)) {
        cut_test_pass_helper(test_context);
    } else {
        cut_test_fail_va_list_helper(
            test_context,
            cut_take_printf_helper(test_context,
                                   "<%s> =~ <%s>\n"
                                   " pattern: <%s>\n"
                                   "  actual: <%s>",
                                   expression_pattern,
                                   expression_actual,
                                   pattern, actual),
            user_message_format);
    }
}

void
cut_assert_equal_pointer_helper (CutTestContext *test_context,
                                 const void     *expected,
                                 const void     *actual,
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
                                   "expected: <%p>\n"
                                   "  actual: <%p>",
                                   expression_expected,
                                   expression_actual,
                                   expected, actual),
            user_message_format);
    }
}

void
cut_error_errno_helper (CutTestContext *test_context,
                        const char     *user_message_format,
                        ...)
{
    int current_errno = errno;

    if (current_errno != 0) {
        cut_test_terminate_va_list_helper(
            test_context,
            ERROR,
            cut_take_printf_helper(test_context,
                                   "<%d> (%s)",
                                   current_errno,
                                   strerror(current_errno)),
            user_message_format);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
