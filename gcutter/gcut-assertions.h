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
#define gcut_assert_equal_type(expected, actual, ...)                   \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_type_helper(get_current_test_context(),       \
                                      (expected), (actual),             \
                                      #expected, #actual,               \
                                      ## __VA_ARGS__, NULL),            \
        gcut_assert_equal_type(expected, actual, ## __VA_ARGS__))

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
#define cut_assert_equal_g_type(expected, actual, ...)                  \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_type(expected, actual, ## __VA_ARGS__),       \
        cut_assert_equal_g_type(expected, actual, ## __VA_ARGS__))
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
#define gcut_assert_equal_value(expected, actual, ...)                  \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_value_helper(get_current_test_context(),      \
                                       expected, actual,                \
                                       #expected, #actual,              \
                                       ## __VA_ARGS__, NULL),           \
        gcut_assert_equal_value(expected, actual, ## __VA_ARGS__))

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
#define cut_assert_equal_g_value(expected, actual, ...)                 \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_value(expected, actual, ## __VA_ARGS__),      \
        cut_assert_equal_g_value(expected, actual, ## __VA_ARGS__))
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
#define gcut_assert_equal_list_int(expected, actual, ...)               \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_int_helper(get_current_test_context(),   \
                                          expected, actual,             \
                                          #expected, #actual,           \
                                          ## __VA_ARGS__, NULL),        \
        gcut_assert_equal_list_int(expected, actual, ## __VA_ARGS__))
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
#define cut_assert_equal_g_list_int(expected, actual, ...)              \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_int(expected, actual, ## __VA_ARGS__),   \
        cut_assert_equal_g_list_int(expected, actual, ## __VA_ARGS__))
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
#define gcut_assert_equal_list_uint(expected, actual, ...)              \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_uint_helper(get_current_test_context(),  \
                                           expected, actual,            \
                                           #expected, #actual,          \
                                           ## __VA_ARGS__, NULL),       \
        gcut_assert_equal_list_uint(expected, actual, ## __VA_ARGS__))

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
#define gcut_assert_equal_list_string(expected, actual, ...)            \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_string_helper(                           \
            get_current_test_context(),                                 \
            expected, actual, #expected, #actual,                       \
            ## __VA_ARGS__, NULL),                                      \
        gcut_assert_equal_list_string(expected, actual, ## __VA_ARGS__))


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
#define cut_assert_equal_g_list_string(expected, actual, ...)   \
    cut_trace_with_info_expression(                             \
        gcut_assert_equal_list_string(expected, actual,         \
                                      ## __VA_ARGS__),          \
        cut_assert_equal_g_list_string(expected, actual,        \
                                       ## __VA_ARGS__))
#endif

/**
 * gcut_assert_equal_hash_table_string_string:
 * @expected: an expected GHashTable * of string.
 * @actual: an actual GHashTable * of string.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.4
 */
#define gcut_assert_equal_hash_table_string_string(expected, actual,    \
                                                   ...)                 \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_hash_table_string_string_helper(              \
            get_current_test_context(),                                 \
            expected, actual, #expected, #actual,                       \
            ## __VA_ARGS__, NULL),                                      \
        gcut_assert_equal_hash_table_string_string(expected, actual,    \
                                                   ## __VA_ARGS__))

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
#define gcut_assert_error(error, ...)                                   \
    cut_trace_with_info_expression(                                     \
        gcut_assert_error_helper(get_current_test_context(),            \
                                 error, #error,                         \
                                 ## __VA_ARGS__, NULL),                 \
        gcut_assert_error(error, ## __VA_ARGS__))

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
#define cut_assert_g_error(error, ...)                  \
    cut_trace_with_info_expression(                     \
        gcut_assert_error(error, ## __VA_ARGS__),       \
        cut_assert_g_error(error, ## __VA_ARGS__))
#endif

/**
 * gcut_assert_equal_error:
 * @expected: an expected GError *.
 * @actual: an actual GError *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_error(expected, actual, ...)                  \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_error_helper(get_current_test_context(),      \
                                       expected, actual,                \
                                       #expected, #actual,              \
                                       ## __VA_ARGS__, NULL),           \
        gcut_assert_equal_error(expected, actual, ## __VA_ARGS__))

/**
 * gcut_assert_remove_path:
 * @path: a first element of the path to the removed path.
 * @...: remaining elements in path.
 *
 * Passes if cut_utils_build_path(@path, ...) is removed successfully.
 *
 * Since: 1.0.3
 */
#define gcut_assert_remove_path(path, ...)                              \
    cut_trace_with_info_expression(                                     \
        gcut_assert_remove_path_helper(path, ## __VA_ARGS__),           \
        gcut_assert_remove_path(path, ## __VA_ARGS__))

#define gcut_assert_remove_path_helper(path, ...) do                    \
{                                                                       \
    GError *_remove_path_g_error = NULL;                                \
    gchar *_full_path;                                                  \
    const gchar *_taken_full_path;                                      \
                                                                        \
    _full_path = cut_utils_build_path(path, ## __VA_ARGS__, NULL);      \
    cut_utils_remove_path_recursive(_full_path, &_remove_path_g_error); \
                                                                        \
    _taken_full_path = cut_take_string(_full_path);                     \
    gcut_assert_error(&_remove_path_g_error,                            \
                      "Remove: %s", _taken_full_path);                  \
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
    cut_trace_with_info_expression(                     \
        gcut_assert_remove_path(path, ## __VA_ARGS__),  \
        cut_assert_remove_path(path, ## __VA_ARGS__))
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
#define gcut_assert_equal_time_val(expected, actual, ...)       \
    cut_trace_with_info_expression(                             \
        gcut_assert_equal_time_val_helper(expected, actual,     \
                                          ## __VA_ARGS__),      \
        gcut_assert_equal_time_val(expected, actual,            \
                                   ## __VA_ARGS__))

#define gcut_assert_equal_time_val_helper(expected, actual, ...) do     \
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

/**
 * gcut_assert_equal_enum:
 * @type: an GEnum type.
 * @expected: an expected enum value.
 * @actual: an actual enum value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * e.g.:
 * |[
 * gcut_assert_equal_enum(GTK_TYPE_DIRECTION_TYPE,
                          GTK_DIR_LEFT, GTK_DIR_LEFT); -> Pass
 * gcut_assert_equal_enum(GTK_TYPE_DIRECTION_TYPE,
                          GTK_DIR_DOWN, GTK_DIR_LEFT); -> Fail
 * ]|
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_enum(enum_type, expected, actual, ...)        \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_enum_helper(get_current_test_context(),       \
                                      enum_type, expected, actual,      \
                                      #enum_type, #expected, #actual,   \
                                      ## __VA_ARGS__, NULL),            \
        gcut_assert_equal_enum(enum_type, expected, actual,             \
                               ## __VA_ARGS__))

/**
 * gcut_assert_equal_flags:
 * @type: an GFlags type.
 * @expected: an expected flags value.
 * @actual: an actual flags value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * e.g.:
 * |[
 * gcut_assert_equal_flags(GTK_TYPE_DIALOG_FLAGS,
 *                         GTK_DIALOG_DESTROY_MODAL |
 *                           GTK_DIALOG_DESTROY_WITH_PARENT,
 *                         GTK_DIALOG_DESTROY_MODAL |
 *                           GTK_DIALOG_DESTROY_WITH_PARENT); -> Pass
 * gcut_assert_equal_flags(GTK_TYPE_DIALOG_FLAGS,
 *                         GTK_DIALOG_DESTROY_MODAL |
 *                           GTK_DIALOG_DESTROY_WITH_PARENT,
 *                         GTK_DIALOG_DESTROY_MODAL); -> Fail
 * ]|
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_flags(flags_type, expected, actual, ...)      \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_flags_helper(get_current_test_context(),      \
                                       flags_type, expected, actual,    \
                                       #flags_type, #expected, #actual, \
                                       ## __VA_ARGS__, NULL),           \
        gcut_assert_equal_flags(flags_type, expected, actual,           \
                               ## __VA_ARGS__))

G_END_DECLS

#endif /* __GCUT_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
