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

#ifndef __CUT_MULTI_PROCESS_H__
#define __CUT_MULTI_PROCESS_H__

#include <cutter/cut-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION: cut-multi-process
 * @title: Multi Process
 * @short_description: Utilities to run your tests on multi
 * processes.
 *
 * If your test target supports multi-process and/or
 * multi-thread, you need to test them. You can use
 * #CutSubProcess and #CutSubProcessGroup to do it.
 *
 * #CutSubProcess runs your test on another process and
 * forwards results of it to a test on your main
 * process. You will get results of test on another process
 * as results of test on your main process.
 *
 * #CutSubProcessGroup is a convenience object to run some
 * #CutSubProcess.
 *
 * Since: 1.0.4
 */

/**
 * CutSubProcess:
 *
 * An object to represent sub cutter process. It contains
 * sub cutter process's parameters and results of sub cutter
 * process.
 *
 * e.g.:
 * |[
 * CutSubProcess *sub_process;
 *
 * sub_process = cut_take_new_sub_process("test-dir");
 * cut_sub_process_set_multi_thread(sub_process, TRUE);
 * cut_assert_true(cut_sub_process_run(sub_process));
 * ]|
 *
 * Since: 1.0.4
 */
typedef struct _CutSubProcess      CutSubProcess;

/**
 * CutSubProcessGroup:
 *
 * An object to run sub cutter processes. This is just for
 * convenience.
 *
 * e.g.:
 * |[
 * CutSubProcess *sub_process1, *sub_process2, *sub_process3;
 * CutSubProcessGroup *group;
 *
 * sub_process1 = cut_take_new_sub_process("test-dir1");
 * sub_process2 = cut_take_new_sub_process("test-dir2");
 * sub_process3 = cut_take_new_sub_process("test-dir3");
 *
 * group = cut_take_new_sub_process_group();
 * cut_sub_process_group_add(group, sub_process1);
 * cut_sub_process_group_add(group, sub_process2);
 * cut_sub_process_group_add(group, sub_process3);
 * cut_assert_true(cut_sub_process_group_run(group));
 * ]|
 *
 * Since: 1.0.4
 */
typedef struct _CutSubProcessGroup CutSubProcessGroup;

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
 * cut_sub_process_run:
 * @sub_process: a #CutSubProcess.
 *
 * Runs sub cutter process.
 *
 * Returns: %CUT_TRUE if @sub_process is completed
 * successfully, %CUT_FALSE otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_run                (CutSubProcess  *sub_process);

/**
 * cut_sub_process_run_async:
 * @sub_process: a #CutSubProcess.
 *
 * Runs sub cutter process asynchronously. The result of
 * @sub_process can be gotten by cut_sub_process_wait().
 *
 * Since: 1.0.4
 */
void           cut_sub_process_run_async          (CutSubProcess  *sub_process);

/**
 * cut_sub_process_wait:
 * @sub_process: a #CutSubProcess.
 *
 * Waits for sub cutter process that is ran asynchronously
 * to complete and returns the result.
 *
 * Returns: %CUT_TRUE if @sub_process is completed
 * successfully, %CUT_FALSE otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_wait               (CutSubProcess  *sub_process);

/**
 * cut_sub_process_is_success:
 * @sub_process: a #CutSubProcess.
 *
 * Returns whether @sub_process is completed successfully or
 * not.
 *
 * Returns: %CUT_TRUE if @sub_process is completed
 * successfully, %CUT_FALSE otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_is_success         (CutSubProcess  *sub_process);

/**
 * cut_sub_process_is_running:
 * @sub_process: a #CutSubProcess.
 *
 * Returns whether @sub_process is running or not.
 *
 * Returns: %CUT_TRUE if @sub_process is running, %CUT_FALSE
 * otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_is_running         (CutSubProcess  *sub_process);

/**
 * cut_sub_process_get_test_directory:
 * @sub_process: a #CutSubProcess.
 *
 * Returns a test directory that has tests to be ran.
 *
 * Returns: a test directory.
 *
 * Since: 1.0.4
 */
const char    *cut_sub_process_get_test_directory (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_test_directory:
 * @sub_process: a #CutSubProcess.
 * @test_directory: a test directory.
 *
 * Sets @test_directory as a test directory that has tests
 * to be ran. This is same as TEST_DIRECTORY required
 * command line argument.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_test_directory (CutSubProcess  *sub_process,
                                                   const char     *test_directory);

/**
 * cut_sub_process_get_source_directory:
 * @sub_process: a #CutSubProcess.
 *
 * Returns a source directory that has source files.
 *
 * Returns: a source directory.
 *
 * Since: 1.0.4
 */
const char    *cut_sub_process_get_source_directory
                                                  (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_source_directory:
 * @sub_process: a #CutSubProcess.
 * @source_directory: a source directory.
 *
 * Sets @source_directory as a source directory that has
 * source files. This is same as --source-directory command
 * line option.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_source_directory
                                                  (CutSubProcess  *sub_process,
                                                   const char     *source_directory);

/**
 * cut_sub_process_get_multi_thread:
 * @sub_process: a #CutSubProcess.
 *
 * Returns whether @sub_process is ran in multi-thread mode.
 *
 * Returns: %CUT_TRUE if @sub_process is ran in multi-thread
 * mode, %CUT_FALSE otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_get_multi_thread   (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_multi_thread:
 * @sub_process: a #CutSubProcess.
 * @multi_thread: %CUT_TRUE to be ran in multi-thread mode.
 *
 * Sets whether @sub_process is ran in multi-thread mode or
 * not. This is same as --multi-thread command line option.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_multi_thread   (CutSubProcess  *sub_process,
                                                   cut_boolean     multi_thread);

/**
 * cut_sub_process_get_exclude_files:
 * @sub_process: a #CutSubProcess.
 *
 * Returns file names that are excluded from target test
 * files.
 *
 * Returns: file names that are excluded from target test files.
 *
 * Since: 1.0.4
 */
const char   **cut_sub_process_get_exclude_files  (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_exclude_files:
 * @sub_process: a #CutSubProcess.
 * @files: file names that are excluded from target test
 * files.
 *
 * Sets file names that are excluded from target test files.
 * This is same as --exclude-file command line option.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_exclude_files  (CutSubProcess  *sub_process,
                                                   const char    **files);

/**
 * cut_sub_process_get_exclude_directories:
 * @sub_process: a #CutSubProcess.
 *
 * Returns directory names that are excluded from target
 * test directories.
 *
 * Returns: directory names that are excluded from target
 * test directories.
 *
 * Since: 1.0.4
 */
const char   **cut_sub_process_get_exclude_directories
                                                  (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_exclude_directories:
 * @sub_process: a #CutSubProcess.
 * @directories: directory names that are excluded from
 * target test directories.
 *
 * Sets directory names that are excluded from target test
 * directories. This is same as --exclude-directory command line
 * option.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_exclude_directories
                                                  (CutSubProcess  *sub_process,
                                                   const char    **directories);

/**
 * cut_sub_process_get_target_test_case_names:
 * @sub_process: a #CutSubProcess.
 *
 * Returns test case names that are ran.
 *
 * Returns: test case names that are ran.
 *
 * Since: 1.0.4
 */
const char   **cut_sub_process_get_target_test_case_names
                                                  (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_target_test_case_names:
 * @sub_process: a #CutSubProcess.
 * @names: test case names that are ran.
 *
 * Sets test case names that are ran. This is same as
 * --test-case command line option.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_target_test_case_names
                                                  (CutSubProcess  *sub_process,
                                                   const char    **names);

/**
 * cut_sub_process_get_target_test_names:
 * @sub_process: a #CutSubProcess.
 *
 * Returns test names that are ran.
 *
 * Returns: test names that are ran.
 *
 * Since: 1.0.4
 */
const char   **cut_sub_process_get_target_test_names
                                                  (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_target_test_names:
 * @sub_process: a #CutSubProcess.
 * @names: test names that are ran.
 *
 * Sets test names that are ran. This is same as --test
 * command line option.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_target_test_names
                                                  (CutSubProcess  *sub_process,
                                                   const char    **names);

/**
 * cut_sub_process_get_elapsed:
 * @sub_process: a #CutSubProcess.
 *
 * Gets the time while @sub_process was running.
 *
 * Returns: the time while @sub_process was running.
 *
 * Since: 1.0.4
 */
double         cut_sub_process_get_elapsed        (CutSubProcess  *sub_process);

/**
 * cut_sub_process_get_total_elapsed:
 * @sub_process: a #CutSubProcess.
 *
 * Gets the sum of times that are used by each test.
 *
 * Returns: the sum of times that are used by each test.
 *
 * Since: 1.0.4
 */
double         cut_sub_process_get_total_elapsed  (CutSubProcess  *sub_process);

/**
 * cut_sub_process_is_crashed:
 * @sub_process: a #CutSubProcess.
 *
 * Returns whether @sub_process was crashed or not.
 *
 * Returns: %CUT_TRUE if @sub_process was crashed, %CUT_FALSE
 * otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_is_crashed         (CutSubProcess  *sub_process);

/**
 * cut_sub_process_get_backtrace:
 * @sub_process: a #CutSubProcess.
 *
 * Returns the backtrace on @sub_process is crashed.
 *
 * Returns: the backtrace if @sub_process could get it, NULL
 * otherwise.
 *
 * Since: 1.0.4
 */
const char    *cut_sub_process_get_backtrace      (CutSubProcess  *sub_process);

/**
 * cut_sub_process_get_fatal_failures:
 * @sub_process: a #CutSubProcess.
 *
 * Returns whether @sub_process is ran in fatal-failures
 * mode. See cut_sub_process_set_fatal_failures() more
 * details of fatal-failures mode.
 *
 * Returns: %CUT_TRUE if @sub_process is ran in fatal-failures
 * mode, %CUT_FALSE otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_get_fatal_failures (CutSubProcess  *sub_process);

/**
 * cut_sub_process_set_fatal_failures:
 * @sub_process: a #CutSubProcess.
 * @fatal_failures: %CUT_TRUE to be ran in fatal-failures mode.
 *
 * Sets whether @sub_process is ran in fatal-failures mode or
 * not. In this mode, all failures are treated as fatal
 * problems. It means that test is aborted on failure. On
 * some environments, breakpoint is set.
 *
 * This is same as --fatal-failures command line option.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_set_fatal_failures (CutSubProcess  *sub_process,
                                                   cut_boolean     fatal_failures);

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

/**
 * cut_sub_process_group_add:
 * @group: a #CutSubProcessGroup.
 * @sub_process: a #CutSubProcess.
 *
 * Adds @sub_process to @group.
 *
 * Since: 1.0.4
 */
void           cut_sub_process_group_add          (CutSubProcessGroup  *group,
                                                   CutSubProcess       *sub_process);

/**
 * cut_sub_process_group_run:
 * @group: a #CutSubProcessGroup.
 *
 * Runs all sub cutter processes of @group and returns the
 * result of them.
 *
 * Returns: %CUT_TRUE if all sub cutter processes of @group
 * are completed successfully, %CUT_FALSE otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_group_run          (CutSubProcessGroup  *group);

/**
 * cut_sub_process_group_run_async:
 * @group: a #CutSubProcessGroup.
 *
 * Runs all sub cutter processes of @group
 * asynchronously. The result of them can be gotten by
 * cut_sub_process_group_wait().
 *
 * Since: 1.0.4
 */
void           cut_sub_process_group_run_async    (CutSubProcessGroup  *group);

/**
 * cut_sub_process_group_wait:
 * @group: a #CutSubProcessGroup.
 *
 * Waits for all sub cutter processes of @group that are ran
 * asynchronously to complete and returns the result.
 *
 * Returns: %CUT_TRUE if all sub cutter processes of @group
 * are completed successfully, %CUT_FALSE otherwise.
 *
 * Since: 1.0.4
 */
cut_boolean    cut_sub_process_group_wait         (CutSubProcessGroup  *group);


#ifdef __cplusplus
}
#endif

#endif /* __CUT_MULTI_PROCESS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
