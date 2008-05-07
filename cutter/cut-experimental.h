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
 * cut_user_data:
 *
 * This is useful only if you make a custom test runner like
 * cutter command. You can pass a data from your custom test
 * runner to test programs. They will receive your data
 * through this.
 *
 * Returns: a data from a program who invokes your test.
 */
#define cut_user_data                                               \
    (cut_test_context_get_user_data(get_current_test_context()))

typedef struct _CutTestAttribute {
    const char *name;
    const char *value;
} CutTestAttribute;

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
 * Returns: pid.
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
 * @uesc_timeout: Timeout in usec.
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
 * @pid: Process ID to be read from stdout.
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
 * @pid: Process ID to be read from stderr.
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

#ifdef __cplusplus
}
#endif

#endif /* __CUT_EXPERIMENTAL_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
