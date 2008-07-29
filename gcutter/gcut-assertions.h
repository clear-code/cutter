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

#ifndef __GCUT_ASSERTIONS_H__
#define __GCUT_ASSERTIONS_H__

#include <glib.h>

#include <gcutter/gcut-value-equal.h>
#include <gcutter/gcut-list.h>
#include <gcutter/gcut-assertions.h>
#include <gcutter/gcut-public.h>
#include <gcutter/gcut-test-utils.h>
#include <gcutter/gcut-assertions-helper.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-assertions
 * @title: Assertions with GLib support
 * @short_description: Checks that your program works as you
 * expect with GLib support.
 *
 */

/**
 * gcut_assert_equal_type:
 * @expected: an expected GType.
 * @actual: an actual GType.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_type(expected, actual, ...) do        \
{                                                               \
    GType _expected = (expected);                               \
    GType _actual = (actual);                                   \
    if (_expected == _actual) {                                 \
        cut_test_pass();                                        \
    } else {                                                    \
        cut_test_fail(FAILURE,                                  \
                      cut_take_printf("<%s == %s>\n"            \
                                      "expected: <%s>\n"        \
                                      " but was: <%s>",         \
                                      #expected, #actual,       \
                                      g_type_name(_expected),   \
                                      g_type_name(_actual)),    \
                      ## __VA_ARGS__);                          \
    }                                                           \
} while(0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_type:
 * @expected: an expected GType.
 * @actual: an actual GType.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_type() instead.
 */
#define cut_assert_equal_g_type(expected, actual, ...)          \
    gcut_assert_equal_type(expected, actual, ## __VA_ARGS__)
#endif

/**
 * gcut_assert_equal_value:
 * @expected: an expected GValue *.
 * @actual: an actual GValue *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_value(expected, actual, ...) do               \
{                                                                       \
    GValue *_expected = (expected);                                     \
    GValue *_actual = (actual);                                         \
    if (gcut_value_equal(expected, actual)) {                           \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *message;                                           \
        const gchar *inspected_expected, *inspected_actual;             \
        const gchar *expected_type_name, *actual_type_name;             \
                                                                        \
        inspected_expected =                                            \
            cut_take_string(g_strdup_value_contents(_expected));        \
        inspected_actual =                                              \
            cut_take_string(g_strdup_value_contents(_actual));          \
        expected_type_name = g_type_name(G_VALUE_TYPE(_expected));      \
        actual_type_name = g_type_name(G_VALUE_TYPE(_actual));          \
                                                                        \
        message = cut_take_printf("<%s == %s>\n"                        \
                                  "expected: <%s> (%s)\n"               \
                                  " but was: <%s> (%s)",                \
                                  #expected, #actual,                   \
                                  inspected_expected,                   \
                                  expected_type_name,                   \
                                  inspected_actual,                     \
                                  actual_type_name);                    \
        message = cut_append_diff(message,                              \
                                  inspected_expected,                   \
                                  inspected_actual);                    \
                                                                        \
        cut_test_fail(FAILURE, message, ## __VA_ARGS__);                \
    }                                                                   \
} while(0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_value:
 * @expected: an expected GValue *.
 * @actual: an actual GValue *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_value() instead.
 */
#define cut_assert_equal_g_value(expected, actual, ...)         \
    gcut_assert_equal_value(expected, actual, ## __VA_ARGS__)
#endif

/**
 * gcut_assert_equal_list_int:
 * @expected: an expected GList * of integer.
 * @actual: an actual GList * of integer.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_list_int(expected, actual, ...) do            \
{                                                                       \
    const GList *_expected, *_actual;                                   \
                                                                        \
    _expected = (expected);                                             \
    _actual = (actual);                                                 \
    if (gcut_list_int_equal(_expected, _actual)) {                      \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *message;                                           \
        const gchar *inspected_expected, *inspected_actual;             \
                                                                        \
        inspected_expected =                                            \
            cut_take_string(gcut_list_int_inspect(_expected));          \
        inspected_actual =                                              \
            cut_take_string(gcut_list_int_inspect(_actual));            \
                                                                        \
        message = cut_take_printf("<%s == %s>\n"                        \
                                  "expected: <%s>\n"                    \
                                  " but was: <%s>",                     \
                                  #expected, #actual,                   \
                                  inspected_expected,                   \
                                  inspected_actual),                    \
        message = cut_append_diff(message,                              \
                                  inspected_expected,                   \
                                  inspected_actual);                    \
        cut_test_fail(FAILURE, message, ## __VA_ARGS__);                \
    }                                                                   \
} while(0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_list_int:
 * @expected: an expected GList * of integer.
 * @actual: an actual GList * of integer.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 0.8
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_list_int() instead.
 */
#define cut_assert_equal_g_list_int(expected, actual, ...)      \
    gcut_assert_equal_list_int(expected, actual, ## __VA_ARGS__)
#endif

/**
 * gcut_assert_equal_list_uint:
 * @expected: an expected GList * of unsigned integer.
 * @actual: an actual GList * of unsigned integer.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_list_uint(expected, actual, ...) do           \
{                                                                       \
    const GList *_expected, *_actual;                                   \
                                                                        \
    _expected = (expected);                                             \
    _actual = (actual);                                                 \
    if (gcut_list_uint_equal(_expected, _actual)) {                     \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *message;                                           \
        const gchar *inspected_expected, *inspected_actual;             \
                                                                        \
        inspected_expected =                                            \
            cut_take_string(gcut_list_uint_inspect(_expected));         \
        inspected_actual =                                              \
            cut_take_string(gcut_list_uint_inspect(_actual));           \
        message = cut_take_printf("<%s == %s>\n"                        \
                                  "expected: <%s>\n"                    \
                                  " but was: <%s>",                     \
                                  #expected, #actual,                   \
                                  inspected_expected,                   \
                                  inspected_actual),                    \
        message = cut_append_diff(message,                              \
                                  inspected_expected,                   \
                                  inspected_actual);                    \
        cut_test_fail(FAILURE, message, ## __VA_ARGS__);                \
    }                                                                   \
} while(0)

/**
 * gcut_assert_equal_list_string:
 * @expected: an expected GList * of string.
 * @actual: an actual GList * of string.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_list_string(expected, actual, ...) do         \
{                                                                       \
    const GList *_expected = (expected);                                \
    const GList *_actual = (actual);                                    \
    if (gcut_list_string_equal(_expected, _actual)) {                   \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *message;                                           \
        const gchar *inspected_expected, *inspected_actual;             \
                                                                        \
        inspected_expected =                                            \
            cut_take_string(gcut_list_string_inspect(_expected));       \
        inspected_actual =                                              \
            cut_take_string(gcut_list_string_inspect(_actual));         \
        message = cut_take_printf("<%s == %s>\n"                        \
                                  "expected: <%s>\n"                    \
                                  " but was: <%s>",                     \
                                  #expected, #actual,                   \
                                  inspected_expected,                   \
                                  inspected_actual);                    \
        message = cut_append_diff(message,                              \
                                  inspected_expected,                   \
                                  inspected_actual);                    \
        cut_test_fail(FAILURE, message, ## __VA_ARGS__);                \
    }                                                                   \
} while(0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_list_string:
 * @expected: an expected GList * of string.
 * @actual: an actual GList * of string.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 0.8
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_list_string() instead.
 */
#define cut_assert_equal_g_list_string(expected, actual, ...)           \
    gcut_assert_equal_list_string(expected, actual, ## __VA_ARGS__)
#endif

/**
 * gcut_assert_error:
 * @error: a target GError *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @error == NULL.
 *
 * Since: 1.0.3
 */
#define gcut_assert_error(error, ...) do                                \
{                                                                       \
    GError *_error;                                                     \
                                                                        \
    _error = (error);                                                   \
    if (_error == NULL) {                                               \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected;                                         \
        inspected =                                                     \
            cut_take_string(cut_utils_inspect_g_error(_error));         \
        g_error_free(_error);                                           \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s> is NULL\n"        \
                                      " but was: <%s>",                 \
                                      #error, inspected),               \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_g_error:
 * @error: a target GError *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @error == NULL.
 *
 * Since: 1.0
 *
 * Deprecated: 1.0.3: Use gcut_assert_error() instead.
 */
#define cut_assert_g_error(error, ...)          \
    gcut_assert_error(error, ## __VA_ARGS__)
#endif

/**
 * gcut_assert_remove_path:
 * @path: a first element of the path to the removed path.
 * @...: remaining elements in path.
 *
 * Passes if cut_utils_build_path(@path, ...) is removed successfully.
 *
 * Since: 1.0.3
 */
#define gcut_assert_remove_path(path, ...) do                           \
{                                                                       \
    GError *_remove_path_g_error = NULL;                                \
    gchar *_full_path;                                                  \
    const gchar *_taken_full_path;                                      \
                                                                        \
    _full_path = cut_utils_build_path(path, ## __VA_ARGS__, NULL);      \
    cut_utils_remove_path_recursive(_full_path, &_remove_path_g_error); \
                                                                        \
    _taken_full_path = cut_take_string(_full_path);                     \
    cut_assert_g_error(&_remove_path_g_error,                           \
                       "Remove: %s", _taken_full_path);                 \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_remove_path:
 * @path: a first element of the path to the removed path.
 * @...: remaining elements in path.
 *
 * Passes if cut_utils_build_path(@path, ...) is removed successfully.
 *
 * Since: 1.0.2
 *
 * Deprecated: 1.0.3: Use gcut_assert_remove_path() instead.
 */
#define cut_assert_remove_path(path, ...)               \
    gcut_assert_remove_path(path, ## __VA_ARGS__)
#endif

/**
 * gcut_assert_equal_time_val:
 * @expected: an expected GTimeVal.
 * @actual: an actual GTimeVal.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.4
 */
#define gcut_assert_equal_time_val(expected, actual, ...) do            \
{                                                                       \
    GTimeVal *_expected_time_val;                                       \
    GTimeVal *_actual_time_val;                                         \
    const gchar *_expected_time_val_string;                             \
    const gchar *_actual_time_val_string;                               \
                                                                        \
    _expected_time_val = (expected);                                    \
    _actual_time_val = (actual);                                        \
    _expected_time_val_string =                                         \
        cut_take_string(g_time_val_to_iso8601(_expected_time_val));     \
    _actual_time_val_string =                                           \
        cut_take_string(g_time_val_to_iso8601(_actual_time_val));       \
    cut_assert_equal_string(_expected_time_val_string,                  \
                            _actual_time_val_string,                    \
                            ## __VA_ARGS__);                            \
} while(0)

G_END_DECLS

#endif /* __GCUT_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
