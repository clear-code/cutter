/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifndef __CUT_PUBLIC_H__
#define __CUT_PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CutTestContext     CutTestContext;

typedef enum {
    CUT_TEST_RESULT_SUCCESS,
    CUT_TEST_RESULT_FAILURE,
    CUT_TEST_RESULT_ERROR,
    CUT_TEST_RESULT_PENDING,
    CUT_TEST_RESULT_NOTIFICATION
} CutTestResultStatus;

void  cut_test_context_pass_assertion       (CutTestContext *context);
void  cut_test_context_register_result      (CutTestContext *context,
                                             CutTestResultStatus status,
                                             const char *function_name,
                                             const char *filename,
                                             unsigned int line,
                                             const char *message,
                                             ...);
void  cut_test_context_long_jump            (CutTestContext *context) __attribute__((__noreturn__));

gboolean cut_utils_compare_string_array     (const gchar **strings1,
                                             const gchar **strings2);


#ifdef __cplusplus
}
#endif

#endif /* __CUT_PUBLIC_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
