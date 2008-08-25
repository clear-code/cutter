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
 * If your test target support multi-process and/or
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

typedef struct _CutSubProcess      CutSubProcess;
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

cut_boolean    cut_sub_process_run                (CutSubProcess  *sub_process);
void           cut_sub_process_run_async          (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_wait               (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_is_success         (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_is_running         (CutSubProcess  *sub_process);

const char    *cut_sub_process_get_test_directory (CutSubProcess  *sub_process);
void           cut_sub_process_set_test_directory (CutSubProcess  *sub_process,
                                                   const char     *test_directory);
const char    *cut_sub_process_get_source_directory
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_source_directory
                                                  (CutSubProcess  *sub_process,
                                                   const char     *source_directory);

cut_boolean    cut_sub_process_is_multi_thread    (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_get_multi_thread   (CutSubProcess  *sub_process);
void           cut_sub_process_set_multi_thread   (CutSubProcess  *sub_process,
                                                   cut_boolean     multi_thread);

const char   **cut_sub_process_get_exclude_files  (CutSubProcess  *sub_process);
void           cut_sub_process_set_exclude_files  (CutSubProcess  *sub_process,
                                                   const char    **files);

const char   **cut_sub_process_get_exclude_directories
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_exclude_directories
                                                  (CutSubProcess  *sub_process,
                                                   const char    **directories);

const char   **cut_sub_process_get_target_test_case_names
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_target_test_case_names
                                                  (CutSubProcess  *sub_process,
                                                   const char    **names);

const char   **cut_sub_process_get_target_test_names
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_target_test_names
                                                  (CutSubProcess  *sub_process,
                                                   const char    **names);

double         cut_sub_process_get_elapsed        (CutSubProcess  *sub_process);
double         cut_sub_process_get_total_elapsed  (CutSubProcess  *sub_process);

cut_boolean    cut_sub_process_is_crashed         (CutSubProcess  *sub_process);

const char    *cut_sub_process_get_backtrace      (CutSubProcess  *sub_process);

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

void           cut_sub_process_group_add          (CutSubProcessGroup  *group,
                                                   CutSubProcess       *sub_process);
cut_boolean    cut_sub_process_group_run          (CutSubProcessGroup  *group);
void           cut_sub_process_group_run_async    (CutSubProcessGroup  *group);
cut_boolean    cut_sub_process_group_wait         (CutSubProcessGroup  *group);


#ifdef __cplusplus
}
#endif

#endif /* __CUT_MULTI_PROCESS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
