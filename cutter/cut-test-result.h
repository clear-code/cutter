/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2009  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_TEST_RESULT_H__
#define __CUT_TEST_RESULT_H__

#include <glib-object.h>

#include <cutter/cut-test.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-test-suite.h>

G_BEGIN_DECLS

#define CUT_TYPE_TEST_RESULT            (cut_test_result_get_type ())
#define CUT_TEST_RESULT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_RESULT, CutTestResult))
#define CUT_TEST_RESULT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_RESULT, CutTestResultClass))
#define CUT_IS_TEST_RESULT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_RESULT))
#define CUT_IS_TEST_RESULT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_RESULT))
#define CUT_TEST_RESULT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_RESULT, CutTestResultClass))

struct _CutTestResult
{
    GObject object;
};

struct _CutTestResultClass
{
    GObjectClass parent_class;
};

GType          cut_test_result_get_type   (void) G_GNUC_CONST;

CutTestResult *cut_test_result_new        (CutTestResultStatus  status,
                                           CutTest             *test,
                                           CutTestIterator     *test_iterator,
                                           CutTestCase         *test_case,
                                           CutTestSuite        *test_suite,
                                           CutTestData         *test_data,
                                           const gchar         *user_message,
                                           const gchar         *system_message,
                                           const GList         *backtrace);
CutTestResult *cut_test_result_new_empty  (void);
CutTestResult *cut_test_result_new_from_xml
                                          (const gchar *xml,
                                           gssize       len,
                                           GError     **error);

CutTestResultStatus  cut_test_result_get_status        (CutTestResult *result);
CutTest             *cut_test_result_get_test          (CutTestResult *result);
const gchar         *cut_test_result_get_test_name     (CutTestResult *result);
CutTestIterator     *cut_test_result_get_test_iterator (CutTestResult *result);
const gchar         *cut_test_result_get_test_iterator_name
                                                       (CutTestResult *result);
CutTestCase         *cut_test_result_get_test_case     (CutTestResult *result);
const gchar         *cut_test_result_get_test_case_name(CutTestResult *result);
CutTestSuite        *cut_test_result_get_test_suite    (CutTestResult *result);
const gchar         *cut_test_result_get_test_suite_name(CutTestResult *result);
CutTestData         *cut_test_result_get_test_data     (CutTestResult *result);
const gchar         *cut_test_result_get_message       (CutTestResult *result);
const gchar         *cut_test_result_get_user_message  (CutTestResult *result);
const gchar         *cut_test_result_get_system_message(CutTestResult *result);
const GList         *cut_test_result_get_backtrace    (CutTestResult *result);
void                 cut_test_result_get_start_time    (CutTestResult *result,
                                                        GTimeVal *start_time);
gdouble              cut_test_result_get_elapsed       (CutTestResult *result);
const gchar         *cut_test_result_get_expected      (CutTestResult *result);
const gchar         *cut_test_result_get_actual        (CutTestResult *result);
const gchar         *cut_test_result_get_diff          (CutTestResult *result);
const gchar         *cut_test_result_get_folded_diff   (CutTestResult *result);

void cut_test_result_set_status          (CutTestResult *result,
                                          CutTestResultStatus status);
void cut_test_result_set_test            (CutTestResult *result,
                                          CutTest *test);
void cut_test_result_set_test_iterator   (CutTestResult *result,
                                          CutTestIterator *test_iterator);
void cut_test_result_set_test_case       (CutTestResult *result,
                                          CutTestCase *test_case);
void cut_test_result_set_test_suite      (CutTestResult *result,
                                          CutTestSuite  *test_suite);
void cut_test_result_set_test_data       (CutTestResult *result,
                                          CutTestData   *test_data);
void cut_test_result_set_message         (CutTestResult *result,
                                          const gchar *message);
void cut_test_result_set_user_message    (CutTestResult *result,
                                          const gchar *user_message);
void cut_test_result_set_system_message  (CutTestResult *result,
                                          const gchar *system_message);
void cut_test_result_set_backtrace       (CutTestResult *result,
                                          const GList   *backtrace);
void cut_test_result_set_start_time      (CutTestResult *result,
                                          GTimeVal *start_time);
void cut_test_result_set_elapsed         (CutTestResult *result,
                                          gdouble elapsed);
void cut_test_result_set_expected        (CutTestResult *result,
                                          const gchar   *expected);
void cut_test_result_set_actual          (CutTestResult *result,
                                          const gchar   *actual);
void cut_test_result_set_diff            (CutTestResult *result,
                                          const gchar   *diff);
void cut_test_result_set_folded_diff     (CutTestResult *result,
                                          const gchar   *folded_diff);

gchar       *cut_test_result_to_xml                (CutTestResult *result);
void         cut_test_result_to_xml_string         (CutTestResult *result,
                                                    GString       *string,
                                                    guint          indent);

const gchar *cut_test_result_status_to_signal_name (CutTestResultStatus status);
gboolean     cut_test_result_status_is_critical    (CutTestResultStatus status);

G_END_DECLS

#endif /* __CUT_TEST_RESULT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
