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

#ifndef __CUT_GASSERTIONS_H__
#define __CUT_GASSERTIONS_H__

#include <glib.h>

#include <cutter/cut-value-equal.h>
#include <cutter/cut-list.h>
#include <cutter/cut-assertions.h>
#include <cutter/cut-gpublic.h>
#include <cutter/cut-gassertions-helper.h>

G_BEGIN_DECLS

/**
 * SECTION: cut-gassertions
 * @title: Assertions with GLib support
 * @short_description: Checks that your program works as you
 * expect with GLib support.
 *
 */

/**
 * cut_assert_equal_g_type:
 * @expected: an expected GType.
 * @actual: an actual GType.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_g_type(expected, actual, ...) do       \
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

/**
 * cut_assert_equal_g_value:
 * @expected: an expected GValue *.
 * @actual: an actual GValue *.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_g_value(expected, actual, ...) do              \
{                                                                       \
    GValue *_expected = (expected);                                     \
    GValue *_actual = (actual);                                         \
    if (cut_value_equal(expected, actual)) {                            \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
        const gchar *expected_type_name, *actual_type_name;             \
        inspected_expected =                                            \
            cut_take_string(g_strdup_value_contents(_expected));        \
        inspected_actual =                                              \
            cut_take_string(g_strdup_value_contents(_actual));          \
        expected_type_name = g_type_name(G_VALUE_TYPE(_expected));      \
        actual_type_name = g_type_name(G_VALUE_TYPE(_actual));          \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("<%s == %s>\n"                    \
                                      "expected: <%s> (%s)\n"           \
                                      " but was: <%s> (%s)",            \
                                      #expected, #actual,               \
                                      inspected_expected,               \
                                      expected_type_name,               \
                                      inspected_actual,                 \
                                      actual_type_name),                \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_g_list_int:
 * @expected: an expected GList * of integer.
 * @actual: an actual GList * of integer.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 0.8
 */
#define cut_assert_equal_g_list_int(expected, actual, ...) do           \
{                                                                       \
    const GList *_expected, *_actual;                                   \
                                                                        \
    _expected = (expected);                                             \
    _actual = (actual);                                                 \
    if (cut_list_equal_int(_expected, _actual)) {                       \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
        inspected_expected =                                            \
            cut_take_string(cut_list_inspect_int(_expected));           \
        inspected_actual =                                              \
            cut_take_string(cut_list_inspect_int(_actual));             \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("<%s == %s>\n"                    \
                                      "expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      #expected, #actual,               \
                                      inspected_expected,               \
                                      inspected_actual),                \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_g_list_uint:
 * @expected: an expected GList * of unsigned integer.
 * @actual: an actual GList * of unsigned integer.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define cut_assert_equal_g_list_uint(expected, actual, ...) do          \
{                                                                       \
    const GList *_expected, *_actual;                                   \
                                                                        \
    _expected = (expected);                                             \
    _actual = (actual);                                                 \
    if (cut_list_equal_uint(_expected, _actual)) {                      \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
        inspected_expected =                                            \
            cut_take_string(cut_list_inspect_uint(_expected));          \
        inspected_actual =                                              \
            cut_take_string(cut_list_inspect_uint(_actual));            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("<%s == %s>\n"                    \
                                      "expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      #expected, #actual,               \
                                      inspected_expected,               \
                                      inspected_actual),                \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_equal_g_list_string:
 * @expected: an expected GList * of string.
 * @actual: an actual GList * of string.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 0.8
 */
#define cut_assert_equal_g_list_string(expected, actual, ...) do        \
{                                                                       \
    const GList *_expected = (expected);                                \
    const GList *_actual = (actual);                                    \
    if (cut_list_equal_string(_expected, _actual)) {                    \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
        inspected_expected =                                            \
            cut_take_string(cut_list_inspect_string(_expected));        \
        inspected_actual =                                              \
            cut_take_string(cut_list_inspect_string(_actual));          \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("<%s == %s>\n"                    \
                                      "expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      #expected, #actual,               \
                                      inspected_expected,               \
                                      inspected_actual),                \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_g_error:
 * @error: a GError *.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @error == NULL.
 *
 * Since: 1.0
 */
#define cut_assert_g_error(g_error, ...) do                             \
{                                                                       \
    GError *_g_error = (g_error);                                       \
    if (_g_error == NULL) {                                             \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected;                                         \
        inspected =                                                     \
            cut_take_string(cut_utils_inspect_g_error(_g_error));       \
        g_error_free(_g_error);                                         \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s> is NULL\n"        \
                                      " but was: <%s>",                 \
                                      #g_error, inspected),             \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while(0)

/**
 * cut_assert_remove_path:
 * @path: a first element of the path to the removed path.
 * @...: remaining elements in path.
 *
 * Passes if cut_utils_build_path(@path, ...) is removed successfully.
 *
 * Since: 1.0.2
 */
#define cut_assert_remove_path(path, ...) do                            \
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

G_END_DECLS

#endif /* __CUT_GASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
