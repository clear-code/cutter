/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_TEST_UTILS_H__
#define __CUT_TEST_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION: cut-test-utils
 * @title: Test Utilities
 * @short_description: Utilities to write tests more easily.
 *
 * To write tests, you need to write codes that
 * set up/tear down test environment, prepare expected and
 * actual values and so on. Cutter provides test utilities
 * to you write your tests more easily.
 *
 * The utilities work without GLib support.
 */

/**
 * cut_take:
 * @object: the object to be owned by Cutter.
 * @destroy_function: the destroy function for the object.
 *
 * Passes ownership of the object to Cutter and returns the
 * object itself. @object is destroyed by @destroy_func.
 *
 * Returns: the object owned by Cutter. Don't free it.
 *
 * Since: 1.0.5
 */
#define cut_take(object, destroy_function)                      \
    cut_test_context_take(cut_get_current_test_context(),       \
                          (object), destroy_function)

/**
 * cut_take_memory:
 * @memory: the memory to be owned by Cutter. (void *)
 *
 * Passes ownership of the memory to Cutter and returns the
 * memory itself. @memory is destroyed by free().
 *
 * Returns: the memory owned by Cutter. Don't free it.
 *
 * Since: 1.0.5
 */
#define cut_take_memory(memory)                                         \
    cut_test_context_take_memory(cut_get_current_test_context(),        \
                                 (memory))

/**
 * cut_take_string:
 * @string: the string to be owned by Cutter.
 *
 * Passes ownership of the string to Cutter and returns the
 * string itself.
 *
 * Returns: a string owned by Cutter. Don't free it.
 */
#define cut_take_string(string)                                         \
    cut_test_context_take_string(cut_get_current_test_context(),        \
                                 (string))

/**
 * cut_take_strdup:
 * @string: the string to be duplicated. (const char *)
 *
 * Duplicates the string, passes ownership of the
 * duplicated string to Cutter and returns the duplicated
 * string.
 *
 * Returns: a duplicated string owned by Cutter. Don't free it.
 *
 * Since: 1.0.5
 */
#define cut_take_strdup(string)                                         \
    cut_test_context_take_strdup(cut_get_current_test_context(),        \
                                 (string))

/**
 * cut_take_strndup:
 * @string: the string to be duplicated. (const char *)
 * @size: the number of bytes to duplicate. (size_t)
 *
 * Duplicates the first @size bytes of the string, passes
 * ownership of the duplicated string to Cutter and returns
 * the duplicated string. The duplicated string is always
 * nul-terminated.
 *
 * Returns: the duplicated string owned by Cutter. Don't free it.
 *
 * Since: 1.0.5
 */
#define cut_take_strndup(string, size)                                  \
    cut_test_context_take_strndup(cut_get_current_test_context(),       \
                                  (string), (size))

/**
 * cut_take_memdup:
 * @memory: the memory to be duplicated. (void *)
 * @size: the number of bytes to duplicate. (size_t)
 *
 * Duplicates @size bytes of the memory, passes ownership of
 * the duplicated memory to Cutter and returns the
 * duplicated memory.
 *
 * Returns: the duplicated memory owned by Cutter. Don't free it.
 *
 * Since: 1.0.5
 */
#define cut_take_memdup(memory, size)                                   \
    cut_test_context_take_memdup(cut_get_current_test_context(),        \
                                 (memory), (size))

/**
 * cut_take_printf:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Formats a string like printf() but the formatted string
 * is owned by Cutter.
 *
 * Returns: a formatted string owned by Cutter. Don't free it.
 */
#define cut_take_printf(format, ...)                                    \
    cut_test_context_take_printf(cut_get_current_test_context(),        \
                                 format, __VA_ARGS__)

/**
 * cut_take_string_array:
 * @strings: the array of strings to be owned by Cutter.
 *
 * Passes ownership of the array of strings to Cutter and
 * returns @strings itself.
 *
 * Returns: @strings strings owned by Cutter. Don't free it.
 */
#define cut_take_string_array(strings)                                  \
    cut_test_context_take_string_array(cut_get_current_test_context(),  \
                                       (strings))

/**
 * cut_take_diff:
 * @from: the original string.
 * @to: the modified string.
 *
 * Computes diff between @from and @to that is owned by Cutter.
 *
 * Returns: a diff between @from and @to. Don't free it.
 */
#define cut_take_diff(from, to)                                         \
    cut_take_string(cut_diff_readable(from, to))

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
 */
#define cut_append_diff(message, from, to)                      \
    cut_take_string(cut_utils_append_diff(message, from, to))

/**
 * cut_inspect_string_array:
 * @strings: the array of strings to be inspected.
 *
 * Formats @strings as human readable string that is owned by Cutter.
 *
 * Returns: a inspected string. Don't free it.
 */
#define cut_inspect_string_array(strings)                               \
    cut_take_string(cut_utils_inspect_string_array(strings))

/**
 * cut_set_fixture_data_dir:
 * @path: a first element of the path to the fixture data directory.
 * @...: remaining elements in path.
 *
 * Set fixture data directory that is used by
 * cut_get_fixture_data_string() and so on.
 *
 * Since: 1.0.2
 */
#define cut_set_fixture_data_dir(path, ...) do  \
{                                               \
    cut_test_context_set_fixture_data_dir(      \
        cut_get_current_test_context(),         \
        path, ## __VA_ARGS__, NULL);            \
} while (0)

/**
 * cut_build_fixture_data_path:
 * @path: a first element of the path to the fixture data.
 * @...: remaining elements in path.
 *
 * Builds a path to the fixture data. If @path is relative
 * path, the path is handled as a relative path from a
 * directory that is specified by cut_set_fixture_data_dir()
 * or the current directory.
 *
 * Returns: a path to the fixture data. It should be freed
 * when no longer needed.
 *
 * Since: 1.0.2
 */
#define cut_build_fixture_data_path(path, ...)  \
    cut_test_context_build_fixture_data_path(   \
        cut_get_current_test_context(),         \
        path, ## __VA_ARGS__, NULL)

/**
 * cut_get_fixture_data_string:
 * @path: a first element of the path to the fixture data.
 * @...: remaining elements in path.
 *
 * Reads the fixture data at "@path/..." and returns it as a
 * string that is owned by Cutter. The description of
 * cut_build_fixture_data_path() shows how the fixture data
 * path is determined.
 *
 * Returns: a content of the fixture data as string. (char
 * *) It should not be freed.
 *
 * Since: 1.0.2
 */
#define cut_get_fixture_data_string(path, ...)                          \
    cut_utils_get_fixture_data_string(cut_get_current_test_context(),   \
                                      NULL, path, ## __VA_ARGS__, NULL)

/**
 * cut_remove_path:
 * @path: a first element of the path to be removed.
 * @...: remaining elements in path.
 *
 * Removes @path and it's children recursively. It doesn't
 * report any errors.
 *
 * Since: 1.0.2
 */
#define cut_remove_path(path, ...)                                      \
    cut_utils_remove_path_recursive_force(                              \
        cut_take_string(                                                \
            cut_utils_build_path(path, ## __VA_ARGS__, NULL)))

/**
 * cut_equal_string:
 * @string1: a string. (char *)
 * @string2: a string. (char *)
 *
 * Compare @string1 to @string2. @string1 and/or @string2
 * maybe NULL.
 *
 * Returns: TRUE if both @string1 and @string2 are NULL or
 *          have the same contents; FALSE otherwise.
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
 * Returns: TRUE if |@double1 - @double2| <= @error; FALSE otherwise.
 *
 * Since: 1.0.5
 */
#define cut_equal_double(double1, double2, error)       \
    cut_utils_equal_double(double1, double2, error)

#ifdef __cplusplus
}
#endif

#endif /* __CUT_TEST_UTILS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
