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

#ifndef __CUTTEST_UTILS_H__
#define __CUTTEST_UTILS_H__

#include <glib.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>

#define ISO8601_PATTERN_WITHOUT_YEAR                    \
    "\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}(?:\\.\\d+)?Z"
#define ISO8601_PATTERN "\\d{4}-" ISO8601_PATTERN_WITHOUT_YEAR

#define CUTTEST_TEST_DIR_KEY "CUTTEST_TEST_DIR"

const gchar *cuttest_get_base_dir (void);
void         cuttest_add_test     (CutTestCase *test_case,
                                   const gchar *test_name,
                                   CutTestFunction test_function);
GList       *cuttest_result_summary_list_new
                                  (guint n_tests,
                                   guint n_assertions,
                                   guint n_successes,
                                   guint n_failures,
                                   guint n_errors,
                                   guint n_pendings,
                                   guint n_notifications,
                                   guint n_omissions);
GList       *cuttest_result_summary_list_new_from_run_context
                                  (CutRunContext *run_context);

GList       *cuttest_result_string_list_new_va_list
                                  (const gchar *test_name,
                                   const gchar *user_message,
                                   const gchar *system_message,
                                   const gchar *message,
                                   const gchar *backtrace,
                                   va_list args);
GList       *cuttest_result_string_list_new
                                  (const gchar *test_name,
                                   const gchar *user_message,
                                   const gchar *system_message,
                                   const gchar *message,
                                   const gchar *backtrace,
                                   ...);
GList       *cuttest_result_string_list_new_from_result
                                  (CutTestResult *result);

#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
