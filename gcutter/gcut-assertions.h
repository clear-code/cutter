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
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_type(expected, actual, ...) do                \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_type_helper((expected), (actual),             \
                                      #expected, #actual),              \
        gcut_assert_equal_type(expected, actual, __VA_ARGS__));         \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_type:
 * @expected: an expected GType.
 * @actual: an actual GType.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_type() instead.
 */
#define cut_assert_equal_g_type(expected, actual, ...) do               \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
                                                                        \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_type(expected, actual),                       \
        cut_assert_equal_g_type(expected, actual, __VA_ARGS__));        \
} while (0)
#endif

/**
 * gcut_assert_equal_value:
 * @expected: an expected GValue *.
 * @actual: an actual GValue *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_value(expected, actual, ...) do               \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_value_helper(expected, actual,                \
                                       #expected, #actual),             \
        gcut_assert_equal_value(expected, actual, __VA_ARGS__));        \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_value:
 * @expected: an expected GValue *.
 * @actual: an actual GValue *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_value() instead.
 */
#define cut_assert_equal_g_value(expected, actual, ...) do              \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_value(expected, actual),                      \
        cut_assert_equal_g_value(expected, actual, __VA_ARGS__));       \
} while (0)
#endif

/**
 * gcut_assert_equal_list:
 * @expected: an expected list.
 * @actual: an actual list.
 * @equal_function: a function that compares each elements of
 *                  @expected and @actual list.
 * @inspect_function: a function that inspected @expected
 *                    and @actual list.
 * @inspect_user_data: a data to be passed to @inspect_function.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @equal_function(@expected, @actual) == %CUT_TRUE.
 *
 * Since: 1.0.6
 */
#define gcut_assert_equal_list(expected, actual, equal_function,        \
                               inspect_function, inspect_user_data,     \
                               ...)  do                                 \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
                                                                        \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_helper(expected, actual,                 \
                                      equal_function,                   \
                                      inspect_function,                 \
                                      inspect_user_data,                \
                                      #expected, #actual,               \
                                      #equal_function),                 \
        gcut_assert_equal_list(expected, actual, equal_function,        \
                               inspect_function, inspect_user_data,     \
                               __VA_ARGS__));                           \
} while (0)

/**
 * gcut_assert_equal_list_int:
 * @expected: an expected GList * of integer.
 * @actual: an actual GList * of integer.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_list_int(expected, actual, ...) do            \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_int_helper(expected, actual,             \
                                          #expected, #actual),          \
        gcut_assert_equal_list_int(expected, actual, __VA_ARGS__));     \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_list_int:
 * @expected: an expected GList * of integer.
 * @actual: an actual GList * of integer.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 0.8
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_list_int() instead.
 */
#define cut_assert_equal_g_list_int(expected, actual, ...) do           \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_int_helper(expected, actual,             \
                                          #expected, #actual),          \
        cut_assert_equal_g_list_int(expected, actual, __VA_ARGS__));    \
} while (0)
#endif

/**
 * gcut_assert_equal_list_uint:
 * @expected: an expected GList * of unsigned integer.
 * @actual: an actual GList * of unsigned integer.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_list_uint(expected, actual, ...) do           \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_uint_helper(expected, actual,            \
                                           #expected, #actual),         \
        gcut_assert_equal_list_uint(expected, actual, __VA_ARGS__));    \
} while (0)

/**
 * gcut_assert_equal_list_string:
 * @expected: an expected GList * of string.
 * @actual: an actual GList * of string.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.3
 */
#define gcut_assert_equal_list_string(expected, actual, ...) do         \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_string_helper(expected, actual,          \
                                             #expected, #actual),       \
        gcut_assert_equal_list_string(expected, actual, __VA_ARGS__));  \
} while (0)


#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_equal_g_list_string:
 * @expected: an expected GList * of string.
 * @actual: an actual GList * of string.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 0.8
 *
 * Deprecated: 1.0.3: Use gcut_assert_equal_list_string() instead.
 */
#define cut_assert_equal_g_list_string(expected, actual, ...) do        \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_string_helper(expected, actual,          \
                                             #expected, #actual),       \
        cut_assert_equal_g_list_string(expected, actual, __VA_ARGS__)); \
} while (0)
#endif

/**
 * gcut_assert_equal_list_object:
 * @expected: an expected GList * of GObject *.
 * @actual: an actual GList * of GObject *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected and @actual has same GObject * in
 * same order.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_object(expected, actual, ...) do         \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_object_helper(expected, actual,          \
                                             g_direct_equal,            \
                                             #expected, #actual),       \
        gcut_assert_equal_list_object(expected, actual, __VA_ARGS__));  \
} while (0)

/**
 * gcut_assert_equal_list_object_custom:
 * @expected: an expected GList * of GObject *.
 * @actual: an actual GList * of GObject *.
 * @equal_function: a function that compares two GObject *.
 *                  (#GEqualFunc)
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected and @actual has same GObject * in
 * same order. Each comparison of GObject * uses
 * @equal_function.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_object_custom(expected, actual,          \
                                             equal_function, ...) do    \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_object_helper(expected, actual,          \
                                             equal_function,            \
                                             #expected, #actual),       \
        gcut_assert_equal_list_object_custom(expected, actual,          \
                                              __VA_ARGS__));            \
} while (0)

/**
 * gcut_assert_equal_list_enum:
 * @type: a GEnum type.
 * @expected: an expected GList * of enum value.
 * @actual: an actual GList * of enum value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected and @actual has same enum values in
 * same order.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_enum(type, expected, actual, ...) do     \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_enum_helper(type, expected, actual,      \
                                           #type, #expected, #actual),  \
        gcut_assert_equal_list_enum(type, expected, actual,             \
                                    __VA_ARGS__));                      \
} while (0)

/**
 * gcut_assert_equal_list_flags:
 * @type: a GFlags type.
 * @expected: an expected GList * of flags value.
 * @actual: an actual GList * of flags value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected and @actual has same flags values in
 * same order.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_list_flags(type, expected, actual, ...) do    \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_list_flags_helper(type, expected, actual,     \
                                            #type, #expected, #actual), \
        gcut_assert_equal_list_flags(type, expected, actual,            \
                                     __VA_ARGS__));                     \
} while (0)

/**
 * gcut_assert_equal_hash_table:
 * @expected: an expected #GHashTable of string.
 * @actual: an actual #GHashTable of string.
 * @equal_function: a function that compares each values of
 *                  @expected and @actual hash table.
 * @key_inspect_function: a function that inspected a key of
 *                        @expected and @actual hash table.
 * @value_inspect_function: a function that inspected a value of
 *                          @expected and @actual hash table.
 * @inspect_user_data: a data to be passed to @inspect_function.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.6
 */
#define gcut_assert_equal_hash_table(expected, actual,                  \
                                     equal_function,                    \
                                     key_inspect_function,              \
                                     value_inspect_function,            \
                                     inspect_user_data) do              \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_hash_table_helper(expected, actual,           \
                                            equal_function,             \
                                            key_inspect_function,       \
                                            value_inspect_function,       \
                                            inspect_user_data,          \
                                            #expected, #actual,         \
                                            #equal_function),           \
        gcut_assert_equal_hash_table(expected, actual));                \
} while (0)

/**
 * gcut_assert_equal_hash_table_string_string:
 * @expected: an expected #GHashTable of string.
 * @actual: an actual #GHashTable of string.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.4
 */
#define gcut_assert_equal_hash_table_string_string(expected, actual,    \
                                                   ...) do              \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_hash_table_string_string_helper(              \
            expected, actual, #expected, #actual),                      \
        gcut_assert_equal_hash_table_string_string(expected, actual,    \
                                                   __VA_ARGS__));       \
} while (0)

/**
 * gcut_assert_error:
 * @error: a target GError *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @error == NULL.
 *
 * Since: 1.0.3
 */
#define gcut_assert_error(error, ...) do                        \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        gcut_assert_error_helper(error, #error),                \
        gcut_assert_error(error, __VA_ARGS__));                 \
} while (0)

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_g_error:
 * @error: a target GError *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @error == NULL.
 *
 * Since: 1.0
 *
 * Deprecated: 1.0.3: Use gcut_assert_error() instead.
 */
#define cut_assert_g_error(error, ...) do                       \
{                                                               \
    cut_set_message_backward_compatibility(__VA_ARGS__);        \
    cut_trace_with_info_expression(                             \
        gcut_assert_error_helper(error, #error),                \
        cut_assert_g_error(error, __VA_ARGS__));                \
} while (0)
#endif

/**
 * gcut_assert_equal_error:
 * @expected: an expected GError *.
 * @actual: an actual GError *.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_error(expected, actual, ...) do               \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_error_helper(expected, actual,                \
                                       #expected, #actual),             \
        gcut_assert_equal_error(expected, actual, __VA_ARGS__));        \
} while (0)

/**
 * gcut_assert_remove_path:
 * @path: a first element of the path to the removed path.
 * @...: remaining elements in path. NULL terminated.
 *
 * Passes if cut_utils_build_path(@path, ...) is removed successfully.
 *
 * Since: 1.0.3
 */
#define gcut_assert_remove_path(path, ...)                      \
    cut_trace_with_info_expression(                             \
        gcut_assert_remove_path_helper(path, __VA_ARGS__),      \
        gcut_assert_remove_path(path, __VA_ARGS__))

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * cut_assert_remove_path:
 * @path: a first element of the path to the removed path.
 * @...: remaining elements in path. NULL terminated.
 *
 * Passes if cut_utils_build_path(@path, ...) is removed successfully.
 *
 * Since: 1.0.2
 *
 * Deprecated: 1.0.3: Use gcut_assert_remove_path() instead.
 */
#define cut_assert_remove_path(path, ...)                       \
    cut_trace_with_info_expression(                             \
        gcut_assert_remove_path_helper(path, __VA_ARGS__),      \
        cut_assert_remove_path(path, __VA_ARGS__))
#endif

/**
 * gcut_assert_equal_time_val:
 * @expected: an expected GTimeVal.
 * @actual: an actual GTimeVal.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.4
 */
#define gcut_assert_equal_time_val(expected, actual, ...) do            \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_time_val_helper(expected, actual,             \
                                          #expected, #actual),          \
        gcut_assert_equal_time_val(expected, actual,  __VA_ARGS__));    \
} while (0)

/**
 * gcut_assert_equal_enum:
 * @enum_type: a GEnum type.
 * @expected: an expected enum value.
 * @actual: an actual enum value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
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
#define gcut_assert_equal_enum(enum_type, expected, actual, ...) do     \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_enum_helper(enum_type, expected, actual,      \
                                      #enum_type, #expected, #actual),  \
        gcut_assert_equal_enum(enum_type, expected, actual,             \
                               __VA_ARGS__));                           \
} while (0)

/**
 * gcut_assert_equal_flags:
 * @flags_type: a GFlags type.
 * @expected: an expected flags value.
 * @actual: an actual flags value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
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
#define gcut_assert_equal_flags(flags_type, expected, actual, ...) do   \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_flags_helper(flags_type,                      \
                                       expected, actual,                \
                                       #flags_type,                     \
                                       #expected, #actual),             \
        gcut_assert_equal_flags(flags_type, expected, actual,           \
                                __VA_ARGS__));                          \
} while (0)

/**
 * gcut_assert_equal_object:
 * @expected: an expected object. (#GObject *)
 * @actual: an actual object. (#GObject *)
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
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
#define gcut_assert_equal_object(expected, actual, ...) do              \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_object_helper(G_OBJECT(expected),             \
                                        G_OBJECT(actual),               \
                                        NULL,                           \
                                        #expected, #actual,             \
                                        NULL),                          \
        gcut_assert_equal_object(expected, actual, __VA_ARGS__));       \
} while (0)

/**
 * gcut_assert_equal_object_custom:
 * @expected: an expected object. (#GObject *)
 * @actual: an actual object. (#GObject *)
 * @equal_function: a function that compare two object. (#GEqualFunc)
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if equal_function(@expected, @actual) == %CUT_TRUE.
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
                                        equal_function, ...) do         \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_object_helper(G_OBJECT(expected),             \
                                        G_OBJECT(actual),               \
                                        equal_function,                 \
                                        #expected, #actual,             \
                                        #equal_function),               \
        gcut_assert_equal_object_custom(expected, actual,               \
                                        equal_function, __VA_ARGS__));  \
} while (0)


/**
 * gcut_assert_equal_int64:
 * @expected: an expected 64 bit integer value. (gint64)
 * @actual: an actual 64 bit integer value. (gint64)
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_int64(expected, actual, ...) do               \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_int64_helper((expected), (actual),            \
                                       #expected, #actual),             \
        gcut_assert_equal_int64(expected, actual, __VA_ARGS__));        \
} while (0)

/**
 * gcut_assert_equal_uint64:
 * @expected: an expected unsigned integer value. (guint64)
 * @actual: an actual unsigned integer value. (guint64)
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.5
 */
#define gcut_assert_equal_uint64(expected, actual, ...) do              \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_uint64_helper((expected), (actual),           \
                                        #expected, #actual),            \
        gcut_assert_equal_uint64(expected, actual, __VA_ARGS__));       \
} while (0)

/**
 * gcut_assert_equal_g_pid:
 * @expected: an expected GPid value.
 * @actual: an actual GPid value.
 * @...: optional format string, followed by parameters to insert
 *       into the format string. (as with printf()) This is
 *       deprecated since 0.1.6. Use cut_set_message() instead.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.6
 */
#define gcut_assert_equal_g_pid(expected, actual, ...) do               \
{                                                                       \
    cut_set_message_backward_compatibility(__VA_ARGS__);                \
    cut_trace_with_info_expression(                                     \
        gcut_assert_equal_g_pid_helper((expected), (actual),            \
                                        #expected, #actual),            \
        gcut_assert_equal_g_pid(expected, actual, __VA_ARGS__));        \
} while (0)

G_END_DECLS

#endif /* __GCUT_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
