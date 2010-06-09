/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
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

#define cut_test_with_user_message(assertion, set_user_message) do      \
{                                                                       \
    jmp_buf *cut_previous_jump_buffer;                                  \
    jmp_buf cut_jump_buffer;                                            \
                                                                        \
    cut_test_context_start_user_message_jump(cut_get_current_test_context()); \
    cut_previous_jump_buffer =                                          \
        cut_test_context_get_jump(cut_get_current_test_context());      \
    cut_test_context_set_jump(cut_get_current_test_context(),           \
                              &cut_jump_buffer);                        \
    if (setjmp(cut_jump_buffer) == 0) {                                 \
        assertion;                                                      \
    }                                                                   \
    cut_test_context_set_jump(cut_get_current_test_context(),           \
                              cut_previous_jump_buffer);                \
    cut_test_context_finish_user_message_jump(cut_get_current_test_context()); \
                                                                        \
    do {                                                                \
        set_user_message;                                               \
    } while (0);                                                        \
                                                                        \
    if (cut_test_context_get_have_current_result(cut_get_current_test_context())) { \
        if (!cut_test_context_in_user_message_jump(cut_get_current_test_context())) { \
            cut_test_context_process_current_result(cut_get_current_test_context()); \
        }                                                               \
        cut_return();                                                   \
    }                                                                   \
} while (0)

#define cut_test_register_result(status, system_message, ...) do        \
{                                                                       \
    cut_test_context_set_current_result(cut_get_current_test_context(), \
                                        CUT_TEST_RESULT_ ## status,     \
                                        system_message);                \
    do {                                                                \
        __VA_ARGS__;                                                    \
    } while (0);                                                        \
    cut_test_context_process_current_result(cut_get_current_test_context()); \
} while (0)

#define cut_test_terminate(status, system_message, ...) do              \
{                                                                       \
    cut_test_context_set_current_result(cut_get_current_test_context(), \
                                        CUT_TEST_RESULT_ ## status,     \
                                        system_message);                \
    do {                                                                \
        __VA_ARGS__;                                                    \
    } while (0);                                                        \
    if (!cut_test_context_in_user_message_jump(cut_get_current_test_context())) { \
        cut_test_context_process_current_result(cut_get_current_test_context()); \
    }                                                                   \
    cut_return();                                                       \
} while (0)

void        cut_assert_helper              (cut_boolean     result,
                                            const char     *expression);
void        cut_assert_true_helper         (cut_boolean     result,
                                            const char     *expression);
void        cut_assert_false_helper        (cut_boolean     result,
                                            const char     *expression);
void        cut_assert_equal_boolean_helper(cut_boolean     expected,
                                            cut_boolean     actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_boolean_helper
                                           (cut_boolean     expected,
                                            cut_boolean     actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_null_helper         (const void     *object,
                                            const char     *expression);
void        cut_assert_null_string_helper  (const char     *string,
                                            const char     *expression);
void        cut_assert_not_null_helper     (const void     *object,
                                            const char     *expression);
void        cut_assert_equal_int_helper    (long            expected,
                                            long            actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_helper(long            expected,
                                            long            actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
#ifdef HAVE_STDINT_H
void        cut_assert_equal_int_least8_helper
                                           (int_least8_t    expected,
                                            int_least8_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_least8_helper
                                           (int_least8_t    expected,
                                            int_least8_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_int_least16_helper
                                           (int_least16_t   expected,
                                            int_least16_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_least16_helper
                                           (int_least16_t   expected,
                                            int_least16_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_int_least32_helper
                                           (int_least32_t   expected,
                                            int_least32_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_least32_helper
                                           (int_least32_t   expected,
                                            int_least32_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_int_least64_helper
                                           (int_least64_t   expected,
                                            int_least64_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_least64_helper
                                           (int_least64_t   expected,
                                            int_least64_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_int_fast8_helper
                                           (int_fast8_t     expected,
                                            int_fast8_t     actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_fast8_helper
                                           (int_fast8_t     expected,
                                            int_fast8_t     actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_int_fast16_helper
                                           (int_fast16_t    expected,
                                            int_fast16_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_fast16_helper
                                           (int_fast16_t    expected,
                                            int_fast16_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_int_fast32_helper
                                           (int_fast32_t    expected,
                                            int_fast32_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_fast32_helper
                                           (int_fast32_t    expected,
                                            int_fast32_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_int_fast64_helper
                                           (int_fast64_t    expected,
                                            int_fast64_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_int_fast64_helper
                                           (int_fast64_t    expected,
                                            int_fast64_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_intptr_helper (intptr_t        expected,
                                            intptr_t        actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_intptr_helper
                                           (intptr_t        expected,
                                            intptr_t        actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_intmax_helper (intmax_t        expected,
                                            intmax_t        actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_intmax_helper
                                           (intmax_t        expected,
                                            intmax_t        actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
#endif
void        cut_assert_equal_uint_helper   (unsigned long   expected,
                                            unsigned long   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_helper
                                           (unsigned long   expected,
                                            unsigned long   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
#ifdef HAVE_STDINT_H
void        cut_assert_equal_uint_least8_helper
                                           (uint_least8_t   expected,
                                            uint_least8_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_least8_helper
                                           (uint_least8_t   expected,
                                            uint_least8_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uint_least16_helper
                                           (uint_least16_t  expected,
                                            uint_least16_t  actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_least16_helper
                                           (uint_least16_t  expected,
                                            uint_least16_t  actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uint_least32_helper
                                           (uint_least32_t  expected,
                                            uint_least32_t  actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_least32_helper
                                           (uint_least32_t  expected,
                                            uint_least32_t  actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uint_least64_helper
                                           (uint_least64_t  expected,
                                            uint_least64_t  actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_least64_helper
                                           (uint_least64_t  expected,
                                            uint_least64_t  actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uint_fast8_helper
                                           (uint_fast8_t    expected,
                                            uint_fast8_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_fast8_helper
                                           (uint_fast8_t    expected,
                                            uint_fast8_t    actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uint_fast16_helper
                                           (uint_fast16_t   expected,
                                            uint_fast16_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_fast16_helper
                                           (uint_fast16_t   expected,
                                            uint_fast16_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uint_fast32_helper
                                           (uint_fast32_t   expected,
                                            uint_fast32_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_fast32_helper
                                           (uint_fast32_t   expected,
                                            uint_fast32_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uint_fast64_helper
                                           (uint_fast64_t   expected,
                                            uint_fast64_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uint_fast64_helper
                                           (uint_fast64_t   expected,
                                            uint_fast64_t   actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uintptr_helper(uintptr_t       expected,
                                            uintptr_t       actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uintptr_helper
                                           (uintptr_t       expected,
                                            uintptr_t       actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_uintmax_helper(uintmax_t       expected,
                                            uintmax_t       actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_uintmax_helper
                                           (uintmax_t       expected,
                                            uintmax_t       actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
#endif
void        cut_assert_equal_size_helper   (size_t          expected,
                                            size_t          actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_size_helper
                                           (size_t          expected,
                                            size_t          actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_double_helper (double          expected,
                                            double          error,
                                            double          actual,
                                            const char     *expression_expected,
                                            const char     *expression_error,
                                            const char     *expression_actual);
void        cut_assert_not_equal_double_helper
                                           (double          expected,
                                            double          error,
                                            double          actual,
                                            const char     *expression_expected,
                                            const char     *expression_error,
                                            const char     *expression_actual);
void        cut_assert_equal_char_helper   (char            expected,
                                            char            actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_char_helper
                                           (char            expected,
                                            char            actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_string_helper (const char     *expected,
                                            const char     *actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_not_equal_string_helper
                                           (const char     *expected,
                                            const char     *actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_equal_substring_helper
                                           (const char     *expected,
                                            const char     *actual,
                                            size_t          length,
                                            const char     *expression_expected,
                                            const char     *expression_actual,
                                            const char     *expression_length);
void        cut_assert_not_equal_substring_helper
                                           (const char     *expected,
                                            const char     *actual,
                                            size_t          length,
                                            const char     *expression_expected,
                                            const char     *expression_actual,
                                            const char     *expression_length);
void        cut_assert_equal_memory_helper (const void     *expected,
                                            size_t          expected_size,
                                            const void     *actual,
                                            size_t          actual_size,
                                            const char     *expression_expected,
                                            const char     *expression_expected_size,
                                            const char     *expression_actual,
                                            const char     *expression_actual_size);
void        cut_assert_not_equal_memory_helper
                                           (const void     *expected,
                                            size_t          expected_size,
                                            const void     *actual,
                                            size_t          actual_size,
                                            const char     *expression_expected,
                                            const char     *expression_expected_size,
                                            const char     *expression_actual,
                                            const char     *expression_actual_size);
void        cut_assert_equal_string_array_helper
                                           (char          **expected,
                                            char          **actual,
                                            const char     *expression_expected,
                                            const char     *expression_actual);
void        cut_assert_operator_helper     (cut_boolean     result,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs);
void        cut_assert_operator_int_helper (cut_boolean     result,
                                            long            lhs,
                                            long            rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs);
void        cut_assert_operator_uint_helper(cut_boolean     result,
                                            unsigned long   lhs,
                                            unsigned long   rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs);
void        cut_assert_operator_size_helper(cut_boolean     result,
                                            size_t          lhs,
                                            size_t          rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs);
void        cut_assert_operator_double_helper
                                           (cut_boolean     result,
                                            double          lhs,
                                            double          rhs,
                                            const char     *expression_lhs,
                                            const char     *expression_operator,
                                            const char     *expression_rhs);
void        cut_assert_equal_helper (cut_boolean     result,
                                     const char     *expression_function,
                                     const char     *expression_expected,
                                     const char     *expression_actual);
void        cut_assert_errno_helper (void);
void        cut_assert_path_exist_helper
                                    (const char     *path,
                                     const char     *expression_path);
void        cut_assert_path_not_exist_helper
                                    (const char     *path,
                                     const char     *expression_path);
void        cut_assert_match_helper (const char     *pattern,
                                     const char     *actual,
                                     const char     *expression_pattern,
                                     const char     *expression_actual);
void        cut_assert_equal_pointer_helper
                                    (const void     *expected,
                                     const void     *actual,
                                     const char     *expression_expected,
                                     const char     *expression_actual);
void        cut_assert_equal_fixture_data_string_helper
                                    (const char     *expected,
                                     const char     *expression_expected,
                                     const char     *path,
                                     ...) CUT_GNUC_NULL_TERMINATED;
void        cut_assert_equal_file_raw_helper
                                    (const char     *expected,
                                     const char     *actual,
                                     const char     *expression_expected,
                                     const char     *expression_actual);
void        cut_assert_not_equal_file_raw_helper
                                    (const char     *expected,
                                     const char     *actual,
                                     const char     *expression_expected,
                                     const char     *expression_actual);
#ifndef CUT_DISABLE_SOCKET_SUPPORT
void        cut_assert_equal_sockaddr_helper
                                    (const struct sockaddr *expected,
                                     const struct sockaddr *actual,
                                     const char     *expression_expected,
                                     const char     *expression_actual);
#endif
void        cut_error_errno_helper  (void);

#ifdef __cplusplus
}
#endif

#endif /* __CUT_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
