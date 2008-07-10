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

#ifndef __CUT_EXPERIMENTAL_H__
#define __CUT_EXPERIMENTAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION: cut-experimental
 * @title: Experimental
 * @short_description: Experimental API.
 *
 * This API may be changed in the future.
 */

/**
 * cut_fork:
 *
 * Makes child process.
 *
 * e.g.:
 * |[
 * int pid;
 * pid = cut_fork();
 * cut_assert_errno();
 *
 * if (pid == 0) {
 *   do_something_for_child_process();
 *   _exit(EXIT_SUCCESS);
 * }
 *
 * do_something_for_parent_process();
 * ]|
 *
 * Returns: Process ID.
 *
 * Since: 0.8
 */
#define cut_fork()                                              \
    cut_test_context_trap_fork(get_current_test_context(),      \
                               __PRETTY_FUNCTION__,             \
                               __FILE__,                        \
                               __LINE__)

/**
 * cut_wait_process:
 * @pid: Process ID to wait.
 * @usec_timeout: Timeout in usec.
 *
 * Waits a process specified by @pid.
 *
 * e.g.:
 * |[
 * int pid;
 * pid = cut_fork();
 * cut_assert_errno();
 *
 * if (pid == 0) {
 *   do_something_for_child_process();
 *   _exit(EXIT_SUCCESS);
 * }
 *
 * do_something_for_parent_process();
 * cut_assert_equal_int(EXIT_SUCCESS, cut_wait_process(pid, 100));
 * ]|
 *
 * Since: 0.8
 */
#define cut_wait_process(pid, usec_timeout)                     \
    cut_test_context_wait_process(get_current_test_context(),   \
                                  pid, usec_timeout)

/**
 * cut_fork_get_stdout_message:
 * @pid: Process ID.
 *
 * Reads a message from a process specified by @pid.
 *
 * e.g.:
 * |[
 * int pid;
 * pid = cut_fork();
 * cut_assert_errno();
 *
 * if (pid == 0) {
 *   g_print("I'm a child.");
 *   _exit(EXIT_SUCCESS);
 * }
 *
 * cut_assert_equal_string("I'm a child.", cut_fork_get_stdout_message(pid));
 * cut_assert_equal_int(EXIT_SUCCESS, cut_wait_process(pid, 100));
 * ]|
 *
 * Returns: a message read from stdout of a process specified by @pid.
 *
 * Since: 0.8
 */
#define cut_fork_get_stdout_message(pid)                            \
    cut_test_context_get_forked_stdout_message(get_current_test_context(), pid)

/**
 * cut_fork_get_stderr_message:
 * @pid: Process ID.
 *
 * Reads a message from a process specified by @pid.
 *
 * e.g.:
 * |[
 * int pid;
 * pid = cut_fork();
 * cut_assert_errno();
 *
 * if (pid == 0) {
 *   g_print("I'm a child.");
 *   _exit(EXIT_SUCCESS);
 * }
 *
 * cut_assert_equal_string("I'm a child.", cut_fork_get_stderr_message(pid));
 * cut_assert_equal_int(EXIT_SUCCESS, cut_wait_process(pid, 100));
 * ]|
 *
 * Returns: a message read from stderr of a process specified by @pid.
 *
 * Since: 0.8
 */
#define cut_fork_get_stderr_message(pid)                            \
    cut_test_context_get_forked_stderr_message(get_current_test_context(), pid)


/**
 * cut_add_data:
 * @first_data_name: The first data name.
 * @...: The data and destroy function of the first data,
 *       followed optionally by more
 *       name/data/destroy_function triples
 *
 * Adds data to use iterated test.
 *
 * e.g.:
 * |[
 * typedef struct _MyTestData
 * {
 *     char *expected;
 *     int integer;
 * } MyTestData;
 *
 * static MyTestData *
 * new_my_test_data (char *expected, int integer)
 * {
 *     MyTestData *data;
 *
 *     data = malloc(sizeof(MyTestData));
 *     data->expected = strdup(expected);
 *     data->integer = integer;
 *
 *     return data;
 * }
 *
 * static void
 * free_my_test_data (MyTestData *data)
 * {
 *     free(data->expected);
 *     free(data);
 * }
 *
 * void
 * data_my_translate(void)
 * {
 *     cut_add_data("simple data", \/\* the first data \*\/
 *                  new_my_test_data("first", 1)),
 *                  free_my_test_data,
 *                  "complex data", \/\* the second data \*\/,
 *                  new_my_test_data("a hundred eleven", 111),
 *                  free_my_test_data);
 * }
 *
 * void
 * test_my_translate(void *data)
 * {
 *      MyTestData *test_data = data;
 *      cut_assert_equal_string(data->expected,
 *                              my_translate(data->integer));
 * }
 * ]|
 *
 * Since: 1.0.3
 */
#define cut_add_data(first_data_name, ...)                      \
    cut_test_context_add_data(get_current_test_context(),       \
                              first_data_name, ## __VA_ARGS__,  \
                              NULL)

#ifdef __cplusplus
}
#endif

#endif /* __CUT_EXPERIMENTAL_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
