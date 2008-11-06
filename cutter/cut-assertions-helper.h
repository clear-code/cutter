/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2008  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_ASSERTIONS_HELPER_H__
#define __CUT_ASSERTIONS_HELPER_H__

#include <cutter/cut-public.h>
#include <cutter/cut-test-utils.h>

#ifdef __cplusplus
extern "C" {
#endif

#define cut_test_register_result(status, system_message, ...) do        \
{                                                                       \
    cut_test_context_register_result(cut_get_current_test_context(),    \
                                     CUT_TEST_RESULT_ ## status,        \
                                     system_message,                    \
                                     ## __VA_ARGS__, NULL);             \
} while (0)

#define cut_test_register_result_va_list(status, system_message,        \
                                         user_message_format) do        \
{                                                                       \
    va_list args;                                                       \
                                                                        \
    va_start(args, user_message_format);                                \
    cut_test_context_register_result_va_list(                           \
        cut_get_current_test_context(),                                 \
        CUT_TEST_RESULT_ ## status,                                     \
        system_message,                                                 \
        user_message_format, args);                                     \
    va_end(args);                                                       \
} while (0)

#define cut_test_terminate(status, system_message, ...) do              \
{                                                                       \
    cut_test_register_result(status, system_message, ## __VA_ARGS__);   \
    cut_return();                                                       \
} while (0)

#define cut_test_terminate_va_list(status, system_message,              \
                                   user_message_format) do              \
{                                                                       \
    cut_test_register_result_va_list(status, system_message,            \
                                     user_message_format);              \
    cut_return();                                                       \
} while (0)

void        cut_assert_helper              (cut_boolean     result,
                                            const char     *expression,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_true_helper         (cut_boolean     result,
                                            const char     *expression,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_false_helper        (cut_boolean     result,
                                            const char     *expression,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_null_helper         (const void     *object,
                                            const char     *expression,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_null_string_helper  (const char     *string,
                                            const char     *expression,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_not_null_helper     (const void     *object,
                                            const char     *expression,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_int_helper    (long            expected,
                                            long            actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_uint_helper   (unsigned long   expected,
                                            unsigned long   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_size_helper   (size_t          expected,
                                            size_t          actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_double_helper (double          expected,
                                            double          error,
                                            double          actual,
                                            const char     *expression_expected,
                                            const char     *expression_error,
                                            const char     *expression_actual,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_string_helper (const char     *expected,
                                            const char     *actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_memory_helper (const void     *expected,
                                            size_t          expected_size,
                                            const void     *actual,
                                            size_t          actual_size,
                                            const char     *expression_expected,
                                            const char     *expression_expected_size,
                                            const char     *expression_actual,
                                            const char     *expression_actual_size,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_string_array_helper
                                           (char          **expected,
                                            char          **actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_operator_helper     (cut_boolean     result,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_operator_int_helper (cut_boolean     result,
                                            long            lhs,
                                            long            rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_operator_uint_helper(cut_boolean     result,
                                            unsigned long   lhs,
                                            unsigned long   rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_operator_size_helper(cut_boolean     result,
                                            size_t          lhs,
                                            size_t          rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_operator_double_helper
                                           (cut_boolean     result,
                                            double          lhs,
                                            double          rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs,
                                            const char     *user_message_format,
                                            ...);
void        cut_assert_equal_helper (cut_boolean     result,
                                     const char     *expression_function,
                                     const char     *expression_expected,
                                     const char     *expression_actual,
                                     const char     *user_message_format,
                                     ...);
void        cut_assert_errno_helper (void           *dummy_argument,
                                     const char     *user_message_format,
                                     ...);
void        cut_assert_path_exist_helper
                                    (const char     *path,
                                     const char     *expression_path,
                                     const char     *user_message_format,
                                     ...);
void        cut_assert_path_not_exist_helper
                                    (const char     *path,
                                     const char     *expression_path,
                                     const char     *user_message_format,
                                     ...);
void        cut_assert_match_helper (const char     *pattern,
                                     const char     *actual,
                                     const char     *expression_pattern,
                                     const char     *expression_actual,
                                     const char     *user_message_format,
                                     ...);
void        cut_assert_equal_pointer_helper
                                    (const void     *expected,
                                     const void     *actual,
                                     const char     *expression_expected,
                                     const char     *expression_actual,
                                     const char     *user_message_format,
                                     ...);
void        cut_error_errno_helper  (void           *dummy_argument,
                                     const char     *user_message_format,
                                     ...);


#ifdef __cplusplus
}
#endif

#endif /* __CUT_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
