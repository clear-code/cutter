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

#include <cutter/cut-public.h>

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
 * cut_take_string:
 * @string: the string to be owned by Cutter.
 *
 * Passes ownership of the string to Cutter and returns a
 * string that has same contents of @string.
 *
 * Returns: a string owned by Cutter. Don't free it.
 */
#define cut_take_string(string)                                         \
    cut_test_context_take_string(get_current_test_context(), (string))

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
    cut_test_context_take_printf(get_current_test_context(),            \
                                 (format), __VA_ARGS__)

/**
 * cut_take_string_array:
 * @strings: the array of strings to be owned by Cutter.
 *
 * Passes ownership of the array of strings (char **) to
 * Cutter and returns an array of strings that has same
 * contents of @strings.
 *
 * Returns: an array of strings owned by Cutter. Don't free it.
 */
#define cut_take_string_array(strings)                                  \
    cut_test_context_take_string_array(get_current_test_context(), (strings))

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
        get_current_test_context(),             \
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
 * Returns: a path to the fixture data.
 *
 * Since: 1.0.2
 */
#define cut_build_fixture_data_path(path, ...)                          \
    cut_test_context_build_fixture_data_path(get_current_test_context(), \
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
 * Returns: a content of the fixture data as string. (char *)
 *
 * Since: 1.0.2
 */
#define cut_get_fixture_data_string(path, ...)                          \
    cut_utils_get_fixture_data_string(get_current_test_context(),       \
                                      __PRETTY_FUNCTION__,              \
                                      __FILE__,                         \
                                      __LINE__,                         \
                                      path, ## __VA_ARGS__, NULL)

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
 * cut_take_new_sub_process:
 * @test_directory: a directory includes sub process test.
 *
 * Creates sub cutter process that runs tests under
 * @test_directory and returns it. A created sub process
 * is owned by Cutter.
 *
 * Returns: a #CutSubProcess.
 *
 * Since: 1.0.4
 */
#define cut_take_new_sub_process(test_directory)                       \
    cut_utils_take_new_sub_process(test_directory,                     \
                                   get_current_test_context())

/**
 * cut_take_new_sub_process_group:
 *
 * Creates a group of sub cutter process. A created group
 * is owned by Cutter.
 *
 * Returns: a #CutSubProcessGroup.
 *
 * Since: 1.0.4
 */
#define cut_take_new_sub_process_group()                                \
    cut_utils_take_new_sub_process_group(get_current_test_context())


#ifdef __cplusplus
}
#endif

#endif /* __CUT_TEST_UTILS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
