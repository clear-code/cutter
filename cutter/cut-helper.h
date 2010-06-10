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

#ifndef __CUT_HELPER_H__
#define __CUT_HELPER_H__

#include <cutter.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION: cut-helper
 * @title: Assertion writing helper
 * @short_description: Symbols in this section help you
 *                     writing your own assertions.
 *
 * You will need to write your own assertions for writing
 * easy to read test. Symbols in this section help you
 * writing your own assertions.
 *
 * e.g.:
 *
 * my-assertions.h:
 * |[
 * #ifndef __MY_ASSERTIONS_H__
 * #define __MY_ASSERTIONS_H__
 *
 * #include <cutter.h>
 *
 * #ifdef __cplusplus
 * extern "C" {
 * #endif
 *
 * #define my_assert_equal_int(expected, actual)                    \
 *     cut_trace_with_info_expression(                              \
 *         my_assert_equal_int_helper((expected), (actual),         \
 *                                    # expected, # actual),        \
 *         my_assert_equal_int(expected, actual, __VA_ARGS__))
 *
 * void my_assert_equal_int_help (long expected,
 *                                long actual,
 *                                const char *expression_expected,
 *                                const char *expression_actual);
 *
 * #ifdef __cplusplus
 * }
 * #endif
 *
 * #endif
 * ]|
 *
 * my-assertions.c:
 * |[
 * #include "my-assertions.h"
 *
 * void
 * my_assert_equal_int_helper (long expected,
 *                             long actual,
 *                             const char *expression_expected,
 *                             const char *expression_actual)
 * {
 *     if (expected == actual) {
 *         cut_test_pass();
 *     } else {
 *         cut_test_fail(cut_take_printf("<%s == %s>\n"
 *                                       "expected: <%ld>\n"
 *                                       "  actual: <%ld>",
 *                                       expression_expected,
 *                                       expression_actual,
 *                                       expected, actual));
 *     }
 * }
 * ]|
 *
 * Makefile.am:
 * |[
 * AM_CFLAGS = $(CUTTER_CFLAGS)
 * LIBS = $(CUTTER_LIBS)
 * noinst_LTLIBRARIES = libmy-assertions.la
 * libmy_assertions_la_SOURCES = my-assertions.c my-assertions.h
 * AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined
 * ]|
 */


/**
 * cut_test_pass:
 *
 * Call cut_test_pass() if an assertion is
 * passed. cut_test_pass() counts up n-assertions.
 *
 * Since: 1.0.5
 */
#define cut_test_pass()                                                 \
    cut_test_context_pass_assertion(cut_get_current_test_context())

/**
 * cut_test_fail:
 * @system_message: a failure message from testing system.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Call cut_test_fail() if an assertion is failed.
 * cut_test_fail() counts up n-failures and terminate the
 * current test.
 *
 * Since: 1.0.5
 */
#define cut_test_fail(system_message)           \
    cut_test_terminate(FAILURE, system_message)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_test_fail_va_list:
 * @system_message: a failure message from testing
 *                  system.
 * @user_message_format: a failure message from user.
 *
 * See cut_test_fail() for cut_test_fail_va_list()'s
 * behavior. @user_message_format is the prior variable of
 * variable length arguments.
 *
 * e.g.:
 * |[
 * void
 * my_assert(cut_boolean result,
 *           const gchar *user_message_format,
 *           ...)
 * {
 *     if (result) {
 *         cut_test_pass();
 *     } else {
 *         cut_test_fail_va_list("Fail!", user_message_format);
 *     }
 * }
 * ]|
 *
 * Since: 1.0.5
 * Deprecated: 1.0.6: Use cut_test_fail() instead.
 */
#define cut_test_fail_va_list(system_message, ...) do           \
{                                                               \
    cut_test_with_user_message(                                 \
        cut_test_terminate(FAILURE, system_message),            \
        __VA_ARGS__);                                           \
} while (0)
#endif


/**
 * CUT_RELATIVE_PATH:
 *
 * Define this macro in a source code or build option
 * (e.g. -DCUT_RELATIVE_PATH=\""sub/dir/"\") if the source
 * code is built as shared library and used it as helper
 * library of your test. If this path isn't set, you can't
 * get correct path from cut_trace() and
 * cut_trace_with_info_expression().
 *
 * Here is an example structure for explain:
 *
 * |[
 * --- core-lib/ --- XXX.c # Your core library
 *  |             +- ...
 *  |             +- YYY.c
 *  +- util-lib/ --- AAA.c # Your utility library
 *  |             +- ...
 *  |             +- BBB.c
 *  |
 *  +- test/ --- core/ --- test-XXX.c # Tests for your core library
 *            |         +- ...
 *            |         +- test-YYY.c
 *            +- util/ --- test-AAA.c # Tests for your utility library
 *            |         +- ...
 *            |         +- test-BBB.c
 *            +- lib/  --- my-assertions.c # Your library of tests.
 *                      +- my-assertions.h # This library will be used
 *                      |                  # as shared library of your
 *                      |                  # tests (test/core/test-*.so
 *                      |                  # and test/util/test-*.so)
 *                      +- ...
 *
 *   % cutter --source-directory=test test
 * ]|
 *
 * In the above example structure, you need to define
 * %CUT_RELATIVE_PATH as "lib" in test/lib/my-assertions.c
 * because my-assertions.c is in lib/ directory from source
 * directory "test" specified by command line option
 * --source-directory.
 *
 * Here are example code and build option:
 *
 * |[
 * test/lib/my-assertions.c:
 *   #define CUT_RELATIVE_PATH "lib"
 *   #include <cutter.h>
 *
 * build option:
 *   % gcc -DCUT_RELATIVE_PATH="\"lib\"" ...
 * ]|
 *
 * Since: 1.0.6
 */
#ifndef CUT_RELATIVE_PATH
#  define CUT_RELATIVE_PATH NULL
#endif

/**
 * cut_push_backtrace:
 * @expression: an expression to be traced.
 *
 * Pushes @expression and the current source place to the
 * backtrace stack.
 *
 * Normally, you don't need to use it directory. cut_trace()
 * is enough.
 *
 * Since: 1.0.6
 */
#define cut_push_backtrace(expression)                                  \
    cut_test_context_push_backtrace(cut_get_current_test_context(),     \
                                    CUT_RELATIVE_PATH,                  \
                                    __FILE__, __LINE__,                 \
                                    CUT_FUNCTION,                       \
                                    #expression)

/**
 * cut_pop_backtrace:
 *
 * Pops a backtrace from the backtrace stack.
 *
 * Normally, you don't need to use it directory. cut_trace()
 * is enough.
 *
 * Since: 1.0.6
 */
void cut_pop_backtrace(void);

/**
 * cut_trace:
 * @expression: an expression to be traced.
 *
 * Mark the current file, line, function and @expression and
 * show it when assertion is failed in
 * @expression. Most of @expression will be function call.
 *
 * Note that you can't get return value of @expression.
 *
 * Here is an example of cut_trace(). If
 * cut_assert_not_null(object) is failed, you will get a
 * backtrace that contains two line;
 * cut_assert_not_null(object) and create_my_object("my-name").
 *
 * e.g.:
 * |[
 * static MyObject *object;
 *
 * static void
 * create_my_object(const char *name)
 * {
 *     object = my_object_new(name);
 *     cut_assert_not_null(object);
 * }
 *
 * void
 * test_my_object_name(void)
 * {
 *     cut_trace(create_my_object("my-name"));
 *     cut_assert_equal_string("my-name",
 *                             my_object_get_name(object));
 * }
 * ]|
 *
 * You will use cut_trace() with macro for test readability:
 * |[
 * static MyObject *object;
 *
 * static void
 * create_my_object_helper(const char *name)
 * {
 *     object = my_object_new(name);
 *     cut_assert_not_null(object);
 * }
 *
 * #define create_my_object(...)                        \
 *     cut_trace(create_my_object_helper(__VA_ARGS__))
 *
 * void
 * test_my_object_name(void)
 * {
 *     create_my_object("my-name");
 *     cut_assert_equal_string("my-name",
 *                             my_object_get_name(object));
 * }
 * ]|
 *
 * Since: 1.0.5
 */
#define cut_trace(expression) do                                        \
{                                                                       \
    cut_push_backtrace(expression);                                     \
    do {                                                                \
        expression;                                                     \
    } while (0);                                                        \
    cut_pop_backtrace();                                                \
} while (0)

/**
 * cut_trace_with_info_expression:
 * @expression: an expression to be traced.
 * @info_expression: a traced expression.
 *
 * It's difference between cut_trace() and
 * cut_trace_with_info_expression() that traced expression
 * is the same expression as @expression or
 * not. cut_trace_with_info_expression() is useful when you
 * want to hide some information in @expression for
 * backtrace readability.
 *
 * Here is an example of
 * cut_trace_with_info_expression(). If
 * cut_assert_not_null(object) is failed, you will get a
 * backtrace that contains two line:
 * <itemizedlist>
 *   <listitem><para>cut_assert_not_null(object)</para></listitem>
 *   <listitem><para>create_my_object("my-name") not
 * create_my_object_helper("my-name")</para></listitem>
 * </itemizedlist>
 * If you use cut_trace() instead of
 * cut_trace_with_info_expression(), you will get
 * create_my_object_helper("my-name"). You may be confused
 * about 'Where is create_my_object_helper("my-name") from?
 * test_my_object_name() uses create_my_object("my-name")
 * but does not use create_my_object_helper("my-name").'.
 *
 * e.g.:
 * |[
 * static MyObject *object;
 *
 * static void
 * create_my_object_helper(const char *name)
 * {
 *     object = my_object_new(name);
 *     cut_assert_not_null(object);
 * }
 *
 * #define create_my_object(...)                        \
 *     cut_trace_with_info_expression(                  \
 *         create_my_object_helper(__VA_ARGS__),        \
 *         create_my_object(__VA_ARGS__))
 *
 * void
 * test_my_object_name(void)
 * {
 *     create_my_object("my-name");
 *     cut_assert_equal_string("my-name",
 *                             my_object_get_name(object));
 * }
 * ]|
 *
 * Since: 1.0.5
 */
#define cut_trace_with_info_expression(expression, info_expression) do  \
{                                                                       \
    CutTestContext *_cut_test_context;                                  \
                                                                        \
    _cut_test_context = cut_get_current_test_context();                 \
    cut_test_context_push_backtrace(_cut_test_context,                  \
                                    CUT_RELATIVE_PATH,                  \
                                    __FILE__, __LINE__,                 \
                                    CUT_FUNCTION,                       \
                                    #info_expression);                  \
    do {                                                                \
        expression;                                                     \
    } while (0);                                                        \
    cut_test_context_pop_backtrace(_cut_test_context);                  \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_append_diff:
 * @message: the string to be appended diff.
 * @from: the original string.
 * @to: the modified string.
 *
 * Computes diff between @from and @to and append the diff
 * to @message. Returned string is owned by Cutter.
 *
 * Returns: @message with diff between @from and @to or same
 * as @message if the diff not interested. Don't free it.
 *
 * Since: 1.0.3
 *
 * Deprecated: 1.0.9: Use cut_set_expected() and
 * cut_set_actual() instead.
 */
#define cut_append_diff(message, from, to)                      \
    cut_take_string(cut_utils_append_diff(message, from, to))
#endif

/**
 * cut_set_expected:
 * @expected: the inspected expected object.
 *
 * Sets an inspected expected object to be used by the next
 * assertion.
 *
 * If both of expected and actual object are set and diff of
 * them is needed, the diff is generated automatically.
 *
 * See also cut_set_actual().
 *
 * Since: 1.0.9
 */
#define cut_set_expected(expected)                                      \
    cut_test_context_set_expected(cut_get_current_test_context(),       \
                                      expected)

/**
 * cut_set_actual:
 * @actual: the inspected actual object.
 *
 * Sets an inspected actual object to be used by the next
 * assertion.
 *
 * If both of expected and actual object are set and diff of
 * them is needed, the diff is generated automatically.
 *
 * See also cut_set_expected().
 *
 * Since: 1.0.9
 */
#define cut_set_actual(actual)                                  \
    cut_test_context_set_actual(cut_get_current_test_context(), \
                                actual)

/**
 * cut_inspect_string_array:
 * @strings: the array of strings to be inspected.
 *
 * Formats @strings as human readable string that is owned by Cutter.
 *
 * Returns: a inspected string owned by Cutter. Don't free it.
 */
#define cut_inspect_string_array(strings)                               \
    cut_take_string(cut_utils_inspect_string_array(strings))

/**
 * cut_equal_string:
 * @string1: a string.
 * @string2: a string.
 *
 * Compare @string1 to @string2. @string1 and/or @string2
 * maybe %NULL.
 *
 * Returns: %CUT_TRUE if both @string1 and @string2 are %NULL or
 *          have the same contents; %CUT_FALSE otherwise.
 *
 * Since: 1.0.5
 */
#define cut_equal_string(string1, string2)                              \
    cut_utils_equal_string(string1, string2)

/**
 * cut_equal_double:
 * @double1: a double value.
 * @double2: a double value.
 * @error: a double value that specifies error range.
 *
 * Compare @double1 to @double2 with @error range.
 *
 * Returns: %CUT_TRUE if |@double1 - @double2| <= @error;
 *          %CUT_FALSE otherwise.
 *
 * Since: 1.0.5
 */
#define cut_equal_double(double1, double2, error)       \
    cut_utils_equal_double(double1, double2, error)

/**
 * cut_equal_sockaddr:
 * @address1: a socket address.
 * @address2: a socket address.
 *
 * Compare @address1 to @address2.
 *
 * This function can be disabled by defining CUT_DISABLE_SOCKET_SUPPORT.
 *
 * Returns: %CUT_TRUE if @address1 == @address2,
 *          %CUT_FALSE otherwise.
 *
 * Since: 1.1.1
 */
#ifdef CUT_DISABLE_SOCKET_SUPPORT
#define cut_equal_sockaddr(address1, address2) CUT_FALSE
#else
#define cut_equal_sockaddr(address1, address2)                  \
    cut_utils_equal_sockaddr(address1, address2)
#endif

/**
 * cut_inspect_sockaddr:
 * @address: the socket address to be inspected.
 *
 * Formats @address as human readable string that is owned
 * by Cutter.
 *
 * This function can be disabled by defining CUT_DISABLE_SOCKET_SUPPORT.
 *
 * Returns: a inspected socket address owned by Cutter. Don't free it.
 *
 * Since: 1.1.1
 */
#ifdef CUT_DISABLE_SOCKET_SUPPORT
#define cut_inspect_sockaddr(address)                               \
    cut_take_strdup("don't define CUT_DISABLE_SOCKET_SUPPORT "      \
                    "to use cut_inspect_sockaddr().")
#else
#define cut_inspect_sockaddr(address)                               \
    cut_take_string(cut_utils_inspect_sockaddr(address))
#endif

/**
 * cut_get_test_directory:
 *
 * Gets the directory name which is specified.
 *
 * Returns: a string owned by Cutter that must not be modified or freed.
 *
 * Since: 1.1.4
 */
const char *cut_get_test_directory(void);

#ifdef __cplusplus
}
#endif

#endif /* __CUT_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
