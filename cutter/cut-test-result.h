/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef __CUT_TEST_RESULT_H__
#define __CUT_TEST_RESULT_H__

#include <glib-object.h>

#include <cutter/cut-assertions.h>

G_BEGIN_DECLS

#define CUT_TYPE_TEST_RESULT            (cut_test_result_get_type ())
#define CUT_TEST_RESULT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_RESULT, CutTestResult))
#define CUT_TEST_RESULT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_RESULT, CutTestResultClass))
#define CUT_IS_TEST_RESULT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_RESULT))
#define CUT_IS_TEST_RESULT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_RESULT))
#define CUT_TEST_RESULT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_RESULT, CutTestResultClass))

typedef struct _CutTestResult         CutTestResult;
typedef struct _CutTestResultClass    CutTestResultClass;

struct _CutTestResult
{
    GObject object;
};

struct _CutTestResultClass
{
    GObjectClass parent_class;
};

GType          cut_test_result_get_type   (void) G_GNUC_CONST;

CutTestResult *cut_test_result_new        (CutTestResultStatus status,
                                           const gchar *test_name,
                                           const gchar *test_case_name,
                                           const gchar *test_suite_name,
                                           const gchar *user_message,
                                           const gchar *system_message,
                                           const gchar *function_name,
                                           const gchar *filename,
                                           guint line);

CutTestResultStatus  cut_test_result_get_status        (CutTestResult *result);
const gchar         *cut_test_result_get_test_name     (CutTestResult *result);
const gchar         *cut_test_result_get_test_case_name(CutTestResult *result);
const gchar         *cut_test_result_get_test_suite_name(CutTestResult *result);
const gchar         *cut_test_result_get_message       (CutTestResult *result);
const gchar         *cut_test_result_get_user_message  (CutTestResult *result);
const gchar         *cut_test_result_get_system_message(CutTestResult *result);
const gchar         *cut_test_result_get_function_name (CutTestResult *result);
const gchar         *cut_test_result_get_filename      (CutTestResult *result);
guint                cut_test_result_get_line          (CutTestResult *result);
gdouble              cut_test_result_get_elapsed       (CutTestResult *result);
gchar               *cut_test_result_to_xml            (CutTestResult *result);

G_END_DECLS

#endif /* __CUT_TEST_RESULT_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
