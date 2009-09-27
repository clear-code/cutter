/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
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

#include <cutter/cut-test-utils-helper.h>

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
 * Passes ownership of @object to Cutter and returns @object
 * itself. @object is destroyed by @destroy_func.
 *
 * Returns: @object owned by Cutter. Don't free it.
 *
 * Since: 1.0.5
 */
#define cut_take(object, destroy_function)                      \
    cut_test_context_take(cut_get_current_test_context(),       \
                          (object), destroy_function)

/**
 * cut_take_memory:
 * @memory: the memory to be owned by Cutter.
 *
 * Passes ownership of @memory to Cutter and returns @memory
 * itself. @memory is destroyed by free().
 *
 * Returns: @memory owned by Cutter. Don't free it.
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
 * Passes ownership of @string to Cutter and returns @string
 * itself.
 *
 * Returns: @string owned by Cutter. Don't free it.
 */
#define cut_take_string(string)                                         \
    cut_test_context_take_string(cut_get_current_test_context(),        \
                                 (string))

/**
 * cut_take_strdup:
 * @string: the string to be duplicated.
 *
 * Duplicates @string, passes ownership of the duplicated
 * string to Cutter and returns the duplicated string.
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
 * @string: the string to be duplicated.
 * @size: the number of bytes to duplicate.
 *
 * Duplicates the first @size bytes of @string, passes
 * ownership of the duplicated string to Cutter and returns
 * the duplicated string. The duplicated string is always
 * %NULL-terminated.
 *
 * Returns: a duplicated string owned by Cutter. Don't free it.
 *
 * Since: 1.0.5
 */
#define cut_take_strndup(string, size)                                  \
    cut_test_context_take_strndup(cut_get_current_test_context(),       \
                                  (string), (size))

/**
 * cut_take_memdup:
 * @memory: the memory to be duplicated.
 * @size: the number of bytes to duplicate.
 *
 * Duplicates @size bytes of @memory, passes ownership of
 * the duplicated memory to Cutter and returns the
 * duplicated memory.
 *
 * Returns: a duplicated memory owned by Cutter. Don't free it.
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
#define cut_take_printf(...)                                            \
    cut_test_context_take_printf(cut_get_current_test_context(),        \
                                 __VA_ARGS__)

/**
 * cut_take_string_array:
 * @strings: the array of strings to be owned by Cutter.
 *
 * Passes ownership of the array of strings to Cutter and
 * returns @strings itself.
 *
 * Returns: @strings owned by Cutter. Don't free it.
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
 * Returns: a diff between @from and @to owned by Cutter. Don't free it.
 */
#define cut_take_diff(from, to)                                         \
    cut_take_string(cut_diff_readable(from, to))

/**
 * cut_take_replace:
 * @target: the replace target string.
 * @pattern: the regular expression pattern as string.
 * @replacement: text to replace each match with
 *
 * Replaces all occurrences of the @pattern with the
 * @replacement in the @target string.
 *
 * Returns: a replaced string owned by Cutter. Don't free it.
 *
 * Since: 1.0.6
 */
#define cut_take_replace(target, pattern, replacement)                  \
    cut_take_replace_helper(                                            \
        target, pattern, replacement,                                   \
        (cut_push_backtrace(cut_take_replace(target,                    \
                                             pattern,                   \
                                             replacement)),             \
         cut_pop_backtrace))

/**
 * cut_set_fixture_data_dir:
 * @path: a first element of the path to the fixture data directory.
 * @...: remaining elements in path.
 *       %NULL-terminate is required since 1.0.7.
 *
 * Set fixture data directory that is used by
 * cut_get_fixture_data_string() and so on.
 *
 * Since: 1.0.2
 */
#define cut_set_fixture_data_dir(...)                   \
    cut_test_context_set_fixture_data_dir(              \
        cut_get_current_test_context(), __VA_ARGS__)

/**
 * cut_build_fixture_data_path:
 * @path: a first element of the path to the fixture data.
 * @...: remaining elements in path.
 *       %NULL-terminate is required since 1.0.7.
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
#define cut_build_fixture_data_path(...)                \
    cut_test_context_build_fixture_data_path(           \
        cut_get_current_test_context(), __VA_ARGS__)

/**
 * cut_get_fixture_data_string:
 * @path: a first element of the path to the fixture data.
 * @...: remaining elements in path.
 *       %NULL-terminate is required since 1.0.7.
 *
 * Reads the fixture data at "@path/..." and returns it as a
 * string that is owned by Cutter. The description of
 * cut_build_fixture_data_path() shows how the fixture data
 * path is determined.
 *
 * Returns: a content of the fixture data as string.
 * Don't free it.
 *
 * Since: 1.0.2
 */
#define cut_get_fixture_data_string(...)                                \
    cut_utils_get_fixture_data_string(cut_get_current_test_context(),   \
                                      NULL, __VA_ARGS__)

/**
 * cut_remove_path:
 * @path: a first element of the path to be removed.
 * @...: remaining elements in path.
 *       %NULL-terminate is required since 1.0.7.
 *
 * Removes @path and it's children recursively. It doesn't
 * report any errors.
 *
 * Since: 1.0.2
 */
#define cut_remove_path(...)                                            \
    cut_utils_remove_path_recursive_force(cut_build_path(__VA_ARGS__))

/**
 * cut_build_path:
 * @path: a first element of the path to be removed.
 * @...: remaining elements in path. %NULL terminated.
 *
 * Builds path from @path and the following elements.
 *
 * Returns: built path owned by Cutter. Don't free it.
 *
 * Since: 1.0.7
 */
#define cut_build_path(...)                                            \
    cut_take_string(cut_utils_build_path(__VA_ARGS__))

#ifdef __cplusplus
}
#endif

#endif /* __CUT_TEST_UTILS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
