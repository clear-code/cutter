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
 * gcut_assert_equal_list_object:
 * @expected: an expected GList * of GObject *.
 * @actual: an actual GList * of GObject *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected and @actual has same GObject * in
 * same order.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_object(expected, actual, ...)            \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_object_helper(                           \
            get_current_test_context(),                                 \
            expected, actual, g_direct_equal, #expected, #actual,       \
            ## __VA_ARGS__, NULL),                                      \
        gcut_assert_equal_list_object(expected, actual, ## __VA_ARGS__))

/**
 * gcut_assert_equal_list_object_custom:
 * @expected: an expected GList * of GObject *.
 * @actual: an actual GList * of GObject *.
 * @equal_function: a function that compares two GObject *.
 *                  (#GEqualFunc)
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected and @actual has same GObject * in
 * same order. Each comparison of GObject * uses
 * @equal_function.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_object_custom(expected, actual,          \
                                             equal_function, ...)       \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_object_helper(                           \
            get_current_test_context(),                                 \
            expected, actual, equal_function, #expected, #actual,       \
            ## __VA_ARGS__, NULL),                                      \
        gcut_assert_equal_list_object_custom(expected, actual,          \
                                             ## __VA_ARGS__))

/**
 * gcut_assert_equal_list_enum:
 * @type: a GEnum type.
 * @expected: an expected GList * of enum value.
 * @actual: an actual GList * of enum value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected and @actual has same enum values in
 * same order.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_enum(type, expected, actual, ...)        \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_enum_helper(                             \
            get_current_test_context(),                                 \
            type, expected, actual, #type, #expected, #actual,          \
            ## __VA_ARGS__, NULL),                                      \
        gcut_assert_equal_list_enum(type, expected, actual,             \
                                    ## __VA_ARGS__))

/**
 * gcut_assert_equal_list_flags:
 * @type: a GFlags type.
 * @expected: an expected GList * of flags value.
 * @actual: an actual GList * of flags value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected and @actual has same flags values in
 * same order.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_flags(type, expected, actual, ...)       \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_flags_helper(                            \
            get_current_test_context(),                                 \
            type, expected, actual, #type, #expected, #actual,          \
            ## __VA_ARGS__, NULL),                                      \
        gcut_assert_equal_list_flags(type, expected, actual,            \
                                    ## __VA_ARGS__))

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
 * @enum_type: a GEnum type.
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
 * @flags_type: a GFlags type.
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

/**
 * gcut_assert_equal_object:
 * @expected: an expected object. (#GObject *)
 * @actual: an actual object. (#GObject *)
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual. See
 * gcut_assert_equal_object_custom() when you need to customize
 * equality check.
 *
 * e.g.:
 * |[
 * gcut_assert_equal_object(object, object);   -> Pass
 * gcut_assert_equal_object(object1, object2); -> Fail
 * gcut_assert_equal_object(NULL, NULL);       -> Pass
 * gcut_assert_equal_object(object1, NULL);    -> Fail
 * ]|
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_object(expected, actual, ...)                 \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_object_helper(get_current_test_context(),     \
                                        G_OBJECT(expected),             \
                                        G_OBJECT(actual),               \
                                        NULL,                           \
                                        #expected, #actual,             \
                                        NULL,                           \
                                        ## __VA_ARGS__, NULL),          \
        gcut_assert_equal_object(expected, actual, ## __VA_ARGS__))

/**
 * gcut_assert_equal_object_custom:
 * @expected: an expected object. (#GObject *)
 * @actual: an actual object. (#GObject *)
 * @equal_function: a function that compare two object. (#GEqualFunc)
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if equal_function(@expected, @actual) == TRUE.
 *
 * e.g.:
 * |[
 * static gboolean
 * equal_name (gconstpointer data1, gconstpointer data2)
 * {
 *     return g_str_equal(my_object_get_name(MY_OBJECT(data1)),
 *                        my_object_get_name(MY_OBJECT(data2)));
 * }
 *
 * gcut_assert_equal_object_custom(object, object, equal_name);   -> Pass
 * gcut_assert_equal_object_custom(same_name_object1,
 *                                 same_name_object2,
 *                                 equal_name);                   -> Pass
 * gcut_assert_equal_object_custom(different_name_object1,
 *                                 different_name_object2,
 *                                 equal_name);                   -> Fail
 * ]|
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_object_custom(expected, actual,               \
                                        equal_function, ...)            \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_object_helper(get_current_test_context(),     \
                                        G_OBJECT(expected),             \
                                        G_OBJECT(actual),               \
                                        equal_function,                 \
                                        #expected, #actual,             \
                                        #equal_function,                \
                                        ## __VA_ARGS__, NULL),          \
        gcut_assert_equal_object(expected, actual, equal_function,      \
                                 ## __VA_ARGS__))


/**
 * gcut_assert_equal_int64:
 * @expected: an expected 64 bit integer value. (gint64)
 * @actual: an actual 64 bit integer value. (gint64)
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_int64(expected, actual, ...)                  \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_int64_helper(get_current_test_context(),      \
                                       (expected), (actual),            \
                                       #expected, #actual,              \
                                       ## __VA_ARGS__, NULL),           \
        gcut_assert_equal_int64(expected, actual, ## __VA_ARGS__))

/**
 * gcut_assert_equal_uint64:
 * @expected: an expected unsigned integer value. (guint64)
 * @actual: an actual unsigned integer value. (guint64)
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_uint64(expected, actual, ...)                 \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_uint64_helper(get_current_test_context(),     \
                                        (expected), (actual),           \
                                        #expected, #actual,             \
                                        ## __VA_ARGS__, NULL),          \
        gcut_assert_equal_uint64(expected, actual, ## __VA_ARGS__))

G_END_DECLS

#endif /* __GCUT_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
