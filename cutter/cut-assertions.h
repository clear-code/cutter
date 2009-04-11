/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_ASSERTIONS_H__
#define __CUT_ASSERTIONS_H__

#include <cutter/cut-assertions-helper.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION: cut-assertions
 * @title: Assertions
 * @short_description: Checks that your program works as you expect.
 *
 * To check that your program works as you expect, you use
 * cut_assert_XXX() where you want to check expected value
 * is got.
 *
 * e.g.:
 * |[
 * cut_assert_equal_int(3, 1 + 2);
 * ]|
 */

/**
 * cut_assert:
 * @expression: the expression to be checked.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expression is not 0 or %NULL.
 */
#define cut_assert(expression, ...) do                          \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_helper((expression) ? CUT_TRUE : CUT_FALSE,  \
                          #expression),                         \
        cut_assert(expression, __VA_ARGS__));                   \
} while (0)

/**
 * cut_assert_true:
 * @expression: the expression to be checked.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expression is %CUT_TRUE value (not 0 or %NULL).
 *
 * Since: 0.9
 */
#define cut_assert_true(expression, ...)  do                            \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_true_helper((expression) ? CUT_TRUE : CUT_FALSE,     \
                               #expression),                            \
        cut_assert_true(expression, __VA_ARGS__));                      \
} while (0)

/**
 * cut_assert_false:
 * @expression: the expression to be checked.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expression is 0 or %NULL.
 *
 * Since: 0.9
 */
#define cut_assert_false(expression, ...) do                            \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_false_helper((expression) ? CUT_TRUE : CUT_FALSE,    \
                                #expression),                           \
        cut_assert_false(expression, __VA_ARGS__));                     \
} while (0)

/**
 * cut_assert_null:
 * @expression: the expression to be checked.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expression is %NULL.
 */
#define cut_assert_null(expression, ...)  do                            \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_null_helper((expression), #expression),              \
        cut_assert_null(expression, __VA_ARGS__));                      \
} while (0)

/**
 * cut_assert_null_string:
 * @string: the string to be checked.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @string is %NULL.
 *
 * Since: 0.3
 */
#define cut_assert_null_string(string, ...)  do                         \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_null_string_helper((string), #string),               \
        cut_assert_null_string(string, __VA_ARGS__));                   \
} while (0)

/**
 * cut_assert_not_null:
 * @expression: the expression to be checked.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expression is not %NULL.
 */
#define cut_assert_not_null(expression, ...)  do                \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_not_null_helper((expression), #expression),  \
        cut_assert_not_null(expression, __VA_ARGS__));          \
} while (0)

/**
 * cut_assert_equal_int:
 * @expected: an expected integer value.
 * @actual: an actual integer value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_int(expected, actual, ...)  do         \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_equal_int_helper((expected), (actual),       \
                                    #expected, #actual),        \
        cut_assert_equal_int(expected, actual, __VA_ARGS__));   \
} while (0)

/**
 * cut_assert_equal_uint:
 * @expected: an expected unsigned integer value.
 * @actual: an actual unsigned integer value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_uint(expected, actual, ...)  do        \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_equal_uint_helper((expected), (actual),      \
                                     #expected, #actual),       \
        cut_assert_equal_uint(expected, actual, __VA_ARGS__));  \
} while (0)

/**
 * cut_assert_equal_size:
 * @expected: an expected size_t value.
 * @actual: an actual size_t value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.6
 */
#define cut_assert_equal_size(expected, actual, ...)  do                \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_size_helper((expected), (actual),              \
                                     #expected, #actual),               \
        cut_assert_equal_size(expected, actual, __VA_ARGS__));          \
} while (0)

/**
 * cut_assert_equal_double:
 * @expected: an expected float value.
 * @error: a float value that specifies error range.
 * @actual: an actual float value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if (@expected - @error) <= @actual <= (@expected + @error).
 */
#define cut_assert_equal_double(expected, error, actual, ...) do        \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_double_helper((expected), (error), (actual),   \
                                       #expected, #error, #actual),     \
        cut_assert_equal_double(expected, error, actual, __VA_ARGS__)); \
} while (0)

/**
 * cut_assert_equal_string:
 * @expected: an expected string value.
 * @actual: an actual string value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if both @expected and @actual are %NULL or
 * strcmp(@expected, @actual) == 0.
 */
#define cut_assert_equal_string(expected, actual, ...) do               \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_string_helper(expected, actual,                \
                                       #expected, #actual),             \
        cut_assert_equal_string(expected, actual, __VA_ARGS__));        \
} while (0)

/**
 * cut_assert_equal_string_with_free:
 * @expected: an expected string value.
 * @actual: an actual string value that is freed.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if both @expected and @actual are %NULL or
 * strcmp(@expected, @actual) == 0.
 *
 * Since: 0.3
 */
#define cut_assert_equal_string_with_free(expected, actual, ...) do     \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_string_helper(expected,                        \
                                       cut_take_string(actual),         \
                                       #expected, #actual),             \
        cut_assert_equal_string_with_free(expected, actual,             \
                                          __VA_ARGS__));                \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_string_or_null:
 * @expected: an expected string value.
 * @actual: an actual string value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if both @expected and @actual are %NULL or
 * strcmp(@expected, @actual) == 0.
 *
 * Deprecated: 0.3: Use cut_assert_equal_string() instead.
 */
#define cut_assert_equal_string_or_null(expected, actual, ...) do       \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_string(expected, actual),                      \
        cut_assert_equal_string_or_null(expected, actual,               \
                                        __VA_ARGS__));                  \
} while (0)
#endif

/**
 * cut_assert_equal_memory:
 * @expected: an expected data.
 * @expected_size: a size of @expected.
 * @actual: an actual data.
 * @actual_size: a size of @actual.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected_size == @actual_size and
 * memcmp(@expected, @actual, @expected_size) == 0.
 */
#define cut_assert_equal_memory(expected, expected_size,                \
                                actual, actual_size, ...) do            \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_memory_helper(expected, expected_size,         \
                                       actual, actual_size,             \
                                       #expected, #expected_size,       \
                                       #actual, #actual_size),          \
        cut_assert_equal_memory(expected, expected_size,                \
                                actual, actual_size, __VA_ARGS__));     \
} while (0)

/**
 * cut_assert_equal_string_array:
 * @expected: an expected %NULL-terminated array of strings.
 * @actual: an actual %NULL-terminated array of strings.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if both @expected and @actual are not %NULL and
 * have same content (strcmp() == 0) strings.
 */
#define cut_assert_equal_string_array(expected, actual, ...) do         \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_string_array_helper(expected, actual,          \
                                             #expected, #actual),       \
        cut_assert_equal_string_array(expected, actual, __VA_ARGS__));  \
} while (0)

/**
 * cut_assert_equal_string_array_with_free:
 * @expected: an expected %NULL-terminated array of strings.
 * @actual: an actual %NULL-terminated array of strings that are freed.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if both @expected and @actual are not %NULL and
 * have same content (strcmp() == 0) strings.
 *
 * Since: 0.9
 */
#define cut_assert_equal_string_array_with_free(expected, actual,       \
                                                ...) do                 \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_string_array_helper(                           \
            expected, (char **)cut_take_string_array(actual),           \
            #expected, #actual),                                        \
        cut_assert_equal_string_array_with_free(expected, actual,       \
                                                __VA_ARGS__));          \
} while (0)

/**
 * cut_assert_operator:
 * @lhs: a left hand side value.
 * @operator: a binary operator.
 * @rhs: a right hand side value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if (@lhs @operator @rhs) is TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_operator(1, <, 2) -> (1 < 2);
 * ]|
 */
#define cut_assert_operator(lhs, operator, rhs, ...) do         \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_operator_helper(((lhs) operator (rhs)),      \
                                   #lhs, #operator, #rhs),      \
        cut_assert_operator(lhs, operator, rhs, __VA_ARGS__));  \
} while (0)

/**
 * cut_assert_operator_int:
 * @lhs: a left hand side integer value.
 * @operator: a binary operator.
 * @rhs: a right hand side integer value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if (@lhs @operator @rhs) is TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_operator_int(1, <, 2) -> (1 < 2);
 * ]|
 */
#define cut_assert_operator_int(lhs, operator, rhs, ...) do             \
{                                                                       \
    long _lhs = (lhs);                                                  \
    long _rhs = (rhs);                                                  \
                                                                        \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_operator_int_helper((_lhs operator _rhs),            \
                                       _lhs, _rhs,                      \
                                       #lhs, #operator, #rhs),          \
        cut_assert_operator_int(lhs, operator, rhs, __VA_ARGS__));      \
} while(0)

/**
 * cut_assert_operator_uint:
 * @lhs: a left hand side unsigned integer value.
 * @operator: a binary operator.
 * @rhs: a right hand side unsigned integer value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if (@lhs @operator @rhs) is TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_operator_uint(1, <, 2) -> (1 < 2);
 * ]|
 *
 * Since: 1.0.5
 */
#define cut_assert_operator_uint(lhs, operator, rhs, ...) do            \
{                                                                   	\
    unsigned long _lhs = (lhs);                                     	\
    unsigned long _rhs = (rhs);                                     	\
                                                                        \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_operator_uint_helper((_lhs operator _rhs),           \
                                        _lhs, _rhs,                     \
                                        #lhs, #operator, #rhs),         \
        cut_assert_operator_uint(lhs, operator, rhs, __VA_ARGS__));     \
} while(0)

/**
 * cut_assert_operator_size:
 * @lhs: a left hand side size_t value.
 * @operator: a binary operator.
 * @rhs: a right hand side size_t value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if (@lhs @operator @rhs) is TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_operator_size(1, <, 2) -> (1 < 2);
 * ]|
 *
 * Since: 1.0.5
 */
#define cut_assert_operator_size(lhs, operator, rhs, ...) do            \
{                                                                   	\
    size_t _lhs = (lhs);                                     		\
    size_t _rhs = (rhs);                                     		\
                                                                        \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_operator_size_helper((_lhs operator _rhs),           \
                                        _lhs, _rhs,                     \
                                        #lhs, #operator, #rhs),         \
        cut_assert_operator_size(lhs, operator, rhs, __VA_ARGS__));     \
} while(0)

/**
 * cut_assert_operator_double:
 * @lhs: a left hand side double value.
 * @operator: a binary operator.
 * @rhs: a right hand side double value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if (@lhs @operator @rhs) is TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_operator_double(1.1, <, 2.2) -> (1.1 < 2.2);
 * ]|
 *
 * Since: 1.0.5
 */
#define cut_assert_operator_double(lhs, operator, rhs, ...) do          \
{                                                                       \
    double _lhs = (lhs);                                                \
    double _rhs = (rhs);                                                \
                                                                        \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_operator_double_helper((_lhs operator _rhs),         \
                                          _lhs, _rhs,                   \
                                          #lhs, #operator, #rhs),       \
        cut_assert_operator_double(lhs, operator, rhs, __VA_ARGS__));   \
} while(0)

/**
 * cut_assert_equal:
 * @function: a function that compares @actual with @expected.
 * @expected: an expected value.
 * @actual: an actual value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @function(@expected, @actual) returns %CUT_TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_equal(!strcmp, "abc", "abc"); -> Pass
 * ]|
 */
#define cut_assert_equal(function, expected, actual, ...) do            \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_helper(function(expected, actual),             \
                                #function, #expected, #actual),         \
        cut_assert_equal(function, expected, actual, __VA_ARGS__));     \
} while (0)

/**
 * cut_assert_errno:
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if errno is 0.
 *
 * e.g.:
 * |[
 * count = write(stdout, buffer, strlen(buffer));
 * cut_assert_errno("Failed to write");            -> Pass when count != -1
 * ]|
 *
 * Since: 0.8
 */
#define cut_assert_errno(...) do                                \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_errno_helper(),                              \
        cut_assert_errno(__VA_ARGS__));                         \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_file_exist:
 * @path: the path to test.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @path exists. It may or may not be a regular file.
 *
 * e.g.:
 * |[
 * cut_assert_file_exist("/tmp");             -> Pass on many environment
 * cut_assert_file_exist("/non-existent");    -> Fail
 * ]|
 *
 * Since: 0.9
 * Deprecated: 1.0.2: Use cut_assert_path_exist() instead.
 */
#define cut_assert_file_exist(path, ...) do                     \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_path_exist(path),                            \
        cut_assert_file_exist(path, __VA_ARGS__));              \
} while (0)
#endif

/**
 * cut_assert_path_exist:
 * @path: the path to test.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @path exists. It may or may not be a regular file.
 *
 * e.g.:
 * |[
 * cut_assert_path_exist("/tmp");             -> Pass on many environment
 * cut_assert_path_exist("/non-existent");    -> Fail
 * ]|
 *
 * Since: 1.0.2
 */
#define cut_assert_path_exist(path, ...) do                     \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_path_exist_helper(path, #path),              \
        cut_assert_path_exist(path, __VA_ARGS__));              \
} while (0)

/**
 * cut_assert_path_not_exist:
 * @path: the path to test.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @path doesn't exist.
 *
 * e.g.:
 * |[
 * cut_assert_path_not_exist("/non-existent");    -> Pass on many environment
 * cut_assert_path_not_exist("/tmp");             -> Fail
 * ]|
 *
 * Since: 1.0.2
 */
#define cut_assert_path_not_exist(path, ...) do                 \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        cut_assert_path_not_exist_helper(path, #path),          \
        cut_assert_path_not_exist(path, __VA_ARGS__));          \
} while (0)

/**
 * cut_assert_match:
 * @pattern: the regular expression pattern as string.
 * @actual: the string to be matched.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @pattern matches @string.
 *
 * e.g.:
 * |[
 * cut_assert_match("^abc", "abc");            -> Pass
 * cut_assert_match("^abc", " abc");           -> Fail
 * ]|
 *
 * Since: 1.0
 */
#define cut_assert_match(pattern, actual, ...) do                       \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_match_helper(pattern, actual, #pattern, #actual),    \
        cut_assert_match(path, actual, __VA_ARGS__));                   \
} while (0)

/**
 * cut_assert_match_with_free:
 * @pattern: the regular expression as string.
 * @actual: the string to be matched that is freed.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @pattern matches @string. See cut_assert_match()
 * for detail.
 *
 * Since: 1.0
 */
#define cut_assert_match_with_free(pattern, actual, ...) do             \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_match_helper(pattern, cut_take_string(actual),       \
                                #pattern, #actual),                     \
        cut_assert_match_with_free(pattern, actual, __VA_ARGS__));      \
} while (0)

/**
 * cut_assert_equal_pointer:
 * @expected: an expected pointer.
 * @actual: an actual pointer.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0
 */
#define cut_assert_equal_pointer(expected, actual, ...) do              \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_pointer_helper(expected, actual,               \
                                        #expected, #actual),            \
        cut_assert_equal_pointer(expected, actual, __VA_ARGS__));       \
} while (0)

/**
 * cut_assert_equal_fixture_data_string:
 * @expected: an expected string.
 * @path: a first element of the path to the fixture data.
 * @...: remaining elements in path. %NULL terminated.
 *
 * Passes if @expected == cut_get_fixture_data_string(@path, ...).
 *
 * Since: 1.0.2
 */
#define cut_assert_equal_fixture_data_string(expected, path, ...)       \
    cut_trace_with_info_expression(                                     \
        cut_assert_equal_fixture_data_string_helper(expected,           \
                                                    #expected,          \
                                                    path, __VA_ARGS__), \
        cut_assert_equal_fixture_data_string(expected, path,            \
                                             __VA_ARGS__))

/**
 * cut_error:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Raises an error with message.
 */
#define cut_error(...) do                                               \
{                                                                       \
    cut_set_message(__VA_ARGS__);                                       \
    cut_trace_with_info_expression(cut_test_terminate(ERROR, NULL),     \
                                   cut_error(__VA_ARGS__));             \
} while (0)

/**
 * cut_error_errno:
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * e.g.:
 * |[
 * void
 * setup (void)
 * {
 *     mkdir("tmp", 0700);
 *     cut_error_errno("Failed to make tmp directory");
 *       -> Error when tmp directory isn't made successfully.
 * }
 * ]|
 *
 * Since: 1.0.2
 */
#define cut_error_errno(...) do                                         \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(cut_error_errno_helper(),            \
                                   cut_error_errno(__VA_ARGS__));       \
} while (0)

/**
 * cut_fail:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Raises a failure with message.
 */
#define cut_fail(...) do                                                \
{                                                                       \
    cut_set_message(__VA_ARGS__);                                       \
    cut_trace_with_info_expression(cut_test_terminate(FAILURE, NULL),   \
                                   cut_fail(__VA_ARGS__));             \
} while (0)

/**
 * cut_pend:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Marks the test is pending with message. The test is
 * stopped.
 */
#define cut_pend(...) do                                                \
{                                                                       \
    cut_set_message(__VA_ARGS__);                                       \
    cut_trace_with_info_expression(cut_test_terminate(PENDING, NULL),   \
                                   cut_pend(__VA_ARGS__));              \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_pending:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Marks the test is pending with message. The test is
 * stopped.
 *
 * Deprecated: 0.4: Use cut_pend() instead.
 */
#define cut_pending(...) do                                             \
{                                                                       \
    cut_set_message(__VA_ARGS__);                                       \
    cut_trace_with_info_expression(cut_test_terminate(PENDING, NULL),   \
                                   cut_pend(__VA_ARGS__));              \
} while (0)
#endif

/**
 * cut_notify:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Leaves a notification message. The test is continued.
 */
#define cut_notify(...) do                              \
{                                                       \
    cut_set_message(__VA_ARGS__);                       \
    cut_trace_with_info_expression(                     \
        cut_test_register_result(NOTIFICATION, NULL),   \
        cut_notify(__VA_ARGS__));                       \
} while (0)

/**
 * cut_omit:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Omit the test.
 *
 * e.g.:
 * |[
 * if (version < 2.0)
 *   cut_omit("Require >= 2.0");
 * ]|
 *
 * Since: 0.8
 */
#define cut_omit(...) do                                                \
{                                                                       \
    cut_set_message(__VA_ARGS__);                                       \
    cut_trace_with_info_expression(cut_test_terminate(OMISSION, NULL),  \
                                   cut_omit(__VA_ARGS__));              \
} while (0)

/**
 * cut_return:
 *
 * Finish the test.
 *
 * e.g.:
 * |[
 * static void
 * sub_xxx (void)
 * {
 *   some_assertions();
 *   if (no_need_more_test)
 *     cut_return();
 *   some_assertions();
 * }
 *
 * void
 * test_xxx (void)
 * {
 *    some_assertions();
 *    cut_trace(sub_xxx());
 *    some_assertions();
 * }
 * ]|
 *
 * Since: 1.0.6
 */
#define cut_return()                                                    \
    cut_test_context_long_jump(cut_get_current_test_context())

#ifdef __cplusplus
}
#endif

#endif /* __CUT_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
