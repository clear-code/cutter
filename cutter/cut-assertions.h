/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

#include <string.h>
#include <errno.h>
#include <cutter/cut-public.h>
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
 * cut_error:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Raises an error with message.
 */
#define cut_error(format, ...)                  \
    cut_test_fail(ERROR, NULL, format, ## __VA_ARGS__)

/**
 * cut_fail:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Raises a failure with message.
 */
#define cut_fail(format, ...)                    \
    cut_test_fail(FAILURE, NULL, format, ## __VA_ARGS__)

/**
 * cut_pend:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Marks the test is pending with message. The test is
 * stopped.
 */
#define cut_pend(format, ...)                               \
    cut_test_fail(PENDING, NULL, format, ## __VA_ARGS__)

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
#define cut_pending(format, ...) cut_pend(format, ## __VA_ARGS__)
#endif

/**
 * cut_notify:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Leaves a notification message. The test is continued.
 */
#define cut_notify(format, ...)                  \
    cut_test_register_result(NOTIFICATION, NULL, format, ## __VA_ARGS__)

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
#define cut_omit(format, ...)                           \
    cut_test_fail(OMISSION, NULL, format, ## __VA_ARGS__)

/**
 * cut_assert:
 * @expression: the expression to check.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expression is not 0 or NULL.
 */
#define cut_assert(expression, ...) do                          \
{                                                               \
    if (expression) {                                           \
        cut_test_pass();                                        \
    } else {                                                    \
        cut_test_fail(FAILURE,                                  \
                      cut_take_printf("expected: <%s>"          \
                                      " is not FALSE/NULL",     \
                                      #expression),             \
                      ## __VA_ARGS__);                          \
    }                                                           \
} while(0)

/**
 * cut_assert_true:
 * @expression: the expression to check.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expression is TRUE value (not 0 or NULL).
 *
 * Since: 0.9
 */
#define cut_assert_true(expression, ...) do                     \
{                                                               \
    if (expression) {                                           \
        cut_test_pass();                                        \
    } else {                                                    \
        cut_test_fail(FAILURE,                                  \
                      cut_take_printf("expected: <%s>"          \
                                      " is TRUE value",         \
                                      #expression),             \
                      ## __VA_ARGS__);                          \
    }                                                           \
} while(0)

/**
 * cut_assert_false:
 * @expression: the expression to check.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expression is 0 or NULL.
 *
 * Since: 0.9
 */
#define cut_assert_false(expression, ...) do                    \
{                                                               \
    if (expression) {                                           \
        cut_test_fail(FAILURE,                                  \
                      cut_take_printf("expected: <%s>"          \
                                      " is FALSE/NULL",         \
                                      #expression),             \
                      ## __VA_ARGS__);                          \
    } else {                                                    \
        cut_test_pass();                                        \
    }                                                           \
} while(0)

/**
 * cut_assert_null:
 * @expression: the expression to check.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expression is NULL.
 */
#define cut_assert_null(expression, ...) do                     \
{                                                               \
    if ((expression) == NULL) {                                 \
        cut_test_pass();                                        \
    } else {                                                    \
        cut_test_fail(FAILURE,                                  \
                      cut_take_printf("expected: <%s> is NULL", \
                                      #expression),             \
                      ## __VA_ARGS__);                          \
    }                                                           \
} while(0)                                                      \

/**
 * cut_assert_null_string:
 * @expression: the expression that returns string.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expression is NULL.
 *
 * Since: 0.3
 */
#define cut_assert_null_string(expression, ...) do                      \
{                                                                       \
    const char *_expression = (expression);                             \
    if (_expression == NULL) {                                          \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s> is NULL\n"        \
                                      " but was: <%s>",                 \
                                      #expression, _expression),        \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_not_null:
 * @expression: the expression to check.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expression is not NULL.
 */
#define cut_assert_not_null(expression, ...) do                         \
{                                                                       \
    if ((expression) != NULL) {                                         \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s> is not NULL",     \
                                      #expression),                     \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_int:
 * @expected: an expected integer value.
 * @actual: an actual integer value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_int(expected, actual, ...) do          \
{                                                               \
    long _expected = (long)(expected);                          \
    long _actual = (long)(actual);                              \
    if (_expected == _actual) {                                 \
        cut_test_pass();                                        \
    } else {                                                    \
        cut_test_fail(FAILURE,                                  \
                      cut_take_printf("<%s == %s>\n"            \
                                      "expected: <%ld>\n"       \
                                      " but was: <%ld>",        \
                                      #expected, #actual,       \
                                      _expected, _actual),      \
                      ## __VA_ARGS__);                          \
    }                                                           \
} while(0)

/**
 * cut_assert_equal_uint:
 * @expected: an expected unsigned integer value.
 * @actual: an actual unsigned integer value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_uint(expected, actual, ...) do     \
{                                                           \
    unsigned long _expected = (unsigned long)(expected);    \
    unsigned long _actual = (unsigned long)(actual);        \
    if (_expected == _actual) {                             \
        cut_test_pass();                                    \
    } else {                                                \
        cut_test_fail(FAILURE,                              \
                      cut_take_printf("<%s == %s>\n"        \
                                      "expected: <%lu>\n"   \
                                      " but was: <%lu>",    \
                                      #expected, #actual,   \
                                      _expected, _actual),  \
                      ## __VA_ARGS__);                      \
    }                                                       \
} while(0)

/**
 * cut_assert_equal_double:
 * @expected: an expected float value.
 * @error: an float value that specifies error range.
 * @actual: an actual float value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if (@expected - @error) <= @actual <= (@expected + @error).
 */
#define cut_assert_equal_double(expected, error, actual, ...) do        \
{                                                                       \
    double _expected, _error, _actual;                                  \
                                                                        \
    _expected = (expected);                                             \
    _error = (error);                                                   \
    _actual = (actual);                                                 \
    if (_expected - _error <= _actual &&                                \
        _actual <= _expected + _error) {                                \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("<%s-%s <= %s <= %s+%s>\n"        \
                                      "expected: <%g +/- %g>\n"         \
                                      " but was: <%g>",                 \
                                      #expected, #error,                \
                                      #actual, #expected, #error,       \
                                      _expected, _error, _actual),      \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_string:
 * @expected: an expected string value.
 * @actual: an actual string value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if both @expected and @actual are NULL or
 * strcmp(@expected, @actual) == 0.
 */
#define cut_assert_equal_string(expected, actual, ...) do               \
{                                                                       \
    const char *_expected = (expected);                                 \
    const char *_actual = (actual);                                     \
    if (_expected == NULL) {                                            \
        if (_actual == NULL) {                                          \
            cut_test_pass();                                            \
        } else {                                                        \
            cut_test_fail(FAILURE,                                      \
                          cut_take_printf("expected: <%s> is NULL\n"    \
                                          " but was: <%s>",             \
                                          #actual, _actual),            \
                          ## __VA_ARGS__);                              \
        }                                                               \
    } else {                                                            \
        if (_actual && strcmp(_expected, _actual) == 0) {               \
            cut_test_pass();                                            \
        } else {                                                        \
            const char *message;                                        \
                                                                        \
            message = cut_take_printf("<%s == %s>\n"                    \
                                      "expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      #expected, #actual,               \
                                      _expected, _actual);              \
            if (_expected && _actual) {                                 \
                const char *diff;                                       \
                                                                        \
                diff = cut_take_diff(_expected, _actual);               \
                if (cut_utils_is_interested_diff(diff)) {               \
                    message = cut_take_printf("%s\n"                    \
                                              "\n"                      \
                                              "diff:\n"                 \
                                              "%s",                     \
                                              message, diff);           \
                }                                                       \
            }                                                           \
            cut_test_fail(FAILURE, message, ## __VA_ARGS__);            \
        }                                                               \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_string_with_free:
 * @expected: an expected string value.
 * @actual: an actual string value that is freed.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if both @expected and @actual are NULL or
 * strcmp(@expected, @actual) == 0.
 *
 * Since: 0.3
 */
#define cut_assert_equal_string_with_free(expected, actual, ...)        \
    cut_assert_equal_string(expected,                                   \
                            cut_take_string(actual),                    \
                            ## __VA_ARGS__);

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_string_or_null:
 * @expected: an expected string value.
 * @actual: an actual string value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if both @expected and @actual are NULL or
 * strcmp(@expected, @actual) == 0.
 *
 *
 * Deprecated: 0.3: Use cut_assert_equal_string() instead.
 */
#define cut_assert_equal_string_or_null(expected, actual, ...) \
    cut_assert_equal_string(expected, actual, ## __VA_ARGS__)
#endif

/**
 * cut_assert_equal_memory:
 * @expected: an expected data.
 * @expected_size: a size of @expected.
 * @actual: an actual data.
 * @actual_size: a size of @actual.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected_size == @actual_size and
 * memcmp(@expected, @actual, @expected_size) == 0.
 */
#define cut_assert_equal_memory(expected, expected_size,                \
                                actual, actual_size, ...) do            \
{                                                                       \
    const void *_expected = (expected);                                 \
    size_t _expected_size = (expected_size);                            \
    const void *_actual = (actual);                                     \
    size_t _actual_size = (actual_size);                                \
    if (_expected_size == _actual_size &&                               \
        memcmp(_expected, _actual, _expected_size) == 0) {              \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("<%s[%s] == %s[%s]>\n"            \
                                      "expected: <%p[%ld]>\n"           \
                                      " but was: <%p[%ld]>",            \
                                      #expected, #expected_size,        \
                                      #actual, #actual_size,            \
                                      _expected, (long)_expected_size,  \
                                      _actual, (long)_actual_size),     \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_string_array:
 * @expected: an expected NULL-terminated array of strings.
 * @actual: an actual NULL-terminated array of strings that are freed.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if both @expected and @actual are not NULL and
 * have same content (strcmp() == 0) strings.
 */
#define cut_assert_equal_string_array(expected, actual, ...) do         \
{                                                                       \
    char **_expected = (expected);                                      \
    char **_actual = (actual);                                          \
    if (_expected && _actual &&                                         \
        cut_utils_compare_string_array(_expected, _actual)) {           \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("<%s == %s>\n"                    \
                                      "expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      #expected, #actual,               \
                                      cut_inspect_string_array(_expected), \
                                      cut_inspect_string_array(_actual)), \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_string_array_with_free:
 * @expected: an expected NULL-terminated array of strings.
 * @actual: an actual NULL-terminated array of strings.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if both @expected and @actual are not NULL and
 * have same content (strcmp() == 0) strings.
 *
 * Since: 0.9
 */
#define cut_assert_equal_string_array_with_free(expected, actual, ...)  \
    cut_assert_equal_string_array(expected,                             \
                                  cut_take_string_array(actual),        \
                                  ## __VA_ARGS__);

/**
 * cut_assert_operator:
 * @lhs: a left hand side value.
 * @operator: a binary operator.
 * @rhs: a right hand side value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if (@lhs @operator @rhs) is TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_operator(1, <, 2) -> (1 < 2);
 * ]|
 */
#define cut_assert_operator(lhs, operator, rhs, ...) do                 \
{                                                                       \
    if ((lhs) operator (rhs)) {                                         \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s %s %s> is TRUE",   \
                                      #lhs, #operator, #rhs),           \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_operator_int:
 * @lhs: a left hand side integer value.
 * @operator: a binary operator.
 * @rhs: a right hand side integer value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
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
    if (_lhs operator _rhs) {                                           \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s> %s <%s>\n"        \
                                      " but was: <%ld> %s <%ld>",       \
                                      #lhs, #operator, #rhs,            \
                                      _lhs, #operator, _rhs),           \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal:
 * @function: a function that compares @actual with @expected.
 * @expected: an expected value.
 * @actual: an actual value.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @function(@expected, @actual) returns TRUE.
 *
 * e.g.:
 * |[
 * cut_assert_equal(!strcmp, "abc", "abc"); -> Pass
 * ]|
 */
#define cut_assert_equal(function, expected, actual, ...) do            \
{                                                                       \
    if (function(expected, actual)) {                                   \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s(%s, %s)> is TRUE", \
                                      #function, #expected, #actual),   \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_errno:
 * @...: the parameters to insert into the format string.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
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
#define cut_assert_errno(...) do                                        \
{                                                                       \
    int _errno = errno;                                                 \
    if (_errno == 0) {                                                  \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <0> (errno)\n"         \
                                      " but was: <%d> (%s)",            \
                                      _errno, strerror(_errno)),        \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

#ifdef __cplusplus
}
#endif

#endif /* __CUT_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
