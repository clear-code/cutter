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

#ifndef __CUT_PUBLIC_H__
#define __CUT_PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#  define CUT_GNUC_PRINTF(format_index, arg_index)                      \
    __attribute__((__format__ (__printf__, format_index, arg_index)))
#  define CUT_GNUC_NORETURN                     \
    __attribute__((__noreturn__))
#else
#  define CUT_GNUC_PRINTF(format_index, arg_index)
#  define CUT_GNUC_NORETURN
#endif

#if __GNUC__ >= 4
#  define CUT_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#else
#  define CUT_GNUC_NULL_TERMINATED
#endif


typedef struct _CutTestContext     CutTestContext;

typedef struct _CutTestAttribute CutTestAttribute;
struct _CutTestAttribute {
    const char *name;
    const char *value;
};

typedef enum {
    CUT_TEST_RESULT_INVALID = -1,
    CUT_TEST_RESULT_SUCCESS,
    CUT_TEST_RESULT_NOTIFICATION,
    CUT_TEST_RESULT_OMISSION,
    CUT_TEST_RESULT_PENDING,
    CUT_TEST_RESULT_FAILURE,
    CUT_TEST_RESULT_ERROR
} CutTestResultStatus;

void  cut_test_context_pass_assertion       (CutTestContext *context);
void  cut_test_context_register_result      (CutTestContext *context,
                                             CutTestResultStatus status,
                                             const char *function_name,
                                             const char *filename,
                                             unsigned int line,
                                             const char *message,
                                             ...) /* CUT_GNUC_PRINTF(7, 8) */;
void  cut_test_context_long_jump            (CutTestContext *context) CUT_GNUC_NORETURN;

const char *cut_test_context_take_string    (CutTestContext *context,
                                             char           *string);
const char *cut_test_context_take_printf    (CutTestContext *context,
                                             const char     *format,
                                             ...) CUT_GNUC_PRINTF(2, 3);
const char **cut_test_context_take_string_array
                                            (CutTestContext *context,
                                             char          **strings);


int   cut_utils_compare_string_array        (char **strings1,
                                             char **strings2);
char *cut_utils_inspect_string_array        (char **strings);
const char *cut_utils_inspect_string        (const char *string);

int   cut_utils_is_interested_diff          (const char *diff);

int   cut_utils_file_exist                  (const char *path);
char *cut_utils_build_path                  (const char *path,
                                             ...) CUT_GNUC_NULL_TERMINATED;
void  cut_utils_remove_path_recursive_force (const char *path);

int   cut_utils_regex_match                 (const char *pattern,
                                             const char *string);

char *cut_test_context_build_fixture_path   (CutTestContext *context,
                                             const char     *path,
                                             ...) CUT_GNUC_NULL_TERMINATED;

const char *cut_utils_get_fixture_data_string(CutTestContext *context,
                                              const char *function,
                                              const char *file,
                                              unsigned int line,
                                              const char *path,
                                              ...) CUT_GNUC_NULL_TERMINATED;

void  cut_utils_get_fixture_data_string_and_path(CutTestContext *context,
                                                 const char *function,
                                                 const char *file,
                                                 unsigned int line,
                                                 char **data,
                                                 char **fixture_path,
                                                 const char *path,
                                                 ...) CUT_GNUC_NULL_TERMINATED;

void *cut_test_context_get_user_data        (CutTestContext *context);
int   cut_test_context_trap_fork            (CutTestContext *context,
                                             const char     *function_name,
                                             const char     *filename,
                                             unsigned int    line);
int   cut_test_context_wait_process         (CutTestContext *context,
                                             int             pid,
                                             unsigned int    usec_timeout);
const char *cut_test_context_get_forked_stdout_message
                                            (CutTestContext *context,
                                             int             pid);
const char *cut_test_context_get_forked_stderr_message
                                            (CutTestContext *context,
                                             int             pid);

void  cut_test_context_set_fixture_data_dir (CutTestContext *context,
                                             const char     *path,
                                             ...) CUT_GNUC_NULL_TERMINATED;

char       *cut_diff_readable               (const char     *from,
                                             const char     *to);


#ifdef __cplusplus
}
#endif

#endif /* __CUT_PUBLIC_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
