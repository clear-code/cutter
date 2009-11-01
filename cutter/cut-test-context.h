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

#ifndef __CUT_TEST_CONTEXT_H__
#define __CUT_TEST_CONTEXT_H__

#include <glib-object.h>

#include <cutter/cut-private.h>
#include <cutter/cut-public.h>
#include <cutter/cut-test-data.h>
#include <gcutter/gcut-public.h>

G_BEGIN_DECLS

#define CUT_TEST_CONTEXT_ERROR                (cut_test_context_error_quark())

#define CUT_TYPE_TEST_CONTEXT                 (cut_test_context_get_type())
#define CUT_TEST_CONTEXT(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_CONTEXT, CutTestContext))
#define CUT_TEST_TEST_CONTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_CONTEXT, CutTestContextClass))
#define CUT_IS_TEST_CONTEXT(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_CONTEXT))
#define CUT_IS_TEST_TEST_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_CONTEXT))
#define CUT_TEST_TEST_CONTEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_CONTEXT, CutTestContextClass))

typedef struct _CutTestContextClass    CutTestContextClass;

struct _CutTestContext
{
    GObject object;
};

struct _CutTestContextClass
{
    GObjectClass parent_class;
};

typedef enum
{
    CUT_TEST_CONTEXT_ERROR_FATAL
} CutTestContextError;

GQuark           cut_test_context_error_quark  (void);

GType            cut_test_context_get_type     (void) G_GNUC_CONST;

void             cut_test_context_current_init (void);
void             cut_test_context_current_quit (void);

CutTestContext  *cut_test_context_new (CutRunContext   *run_context,
                                       CutTestSuite    *test_suite,
                                       CutTestCase     *test_case,
                                       CutTestIterator *test_iterator,
                                       CutTest         *test);
CutTestContext  *cut_test_context_new_empty (void);

CutRunContext   *cut_test_context_get_run_context (CutTestContext *context);
void             cut_test_context_set_run_context (CutTestContext *context,
                                                   CutRunContext  *run_context);
CutTestSuite *cut_test_context_get_test_suite (CutTestContext *context);
void          cut_test_context_set_test_suite (CutTestContext *context,
                                               CutTestSuite   *test_suite);
CutTestCase  *cut_test_context_get_test_case  (CutTestContext *context);
void          cut_test_context_set_test_case  (CutTestContext *context,
                                               CutTestCase    *test_case);
CutTestIterator *cut_test_context_get_test_iterator
                                              (CutTestContext *context);
void          cut_test_context_set_test_iterator
                                              (CutTestContext *context,
                                               CutTestIterator *test_iterator);
CutTest      *cut_test_context_get_test       (CutTestContext *context);
void          cut_test_context_set_test       (CutTestContext *context,
                                               CutTest        *test);

void          cut_test_context_set_multi_thread
                                              (CutTestContext *context,
                                               gboolean        use_multi_thread);
gboolean      cut_test_context_is_multi_thread
                                              (CutTestContext *context);

void          cut_test_context_set_data       (CutTestContext *context,
                                               CutTestData    *test_data);
CutTestData  *cut_test_context_get_data       (CutTestContext *context,
                                               guint           index);
void          cut_test_context_shift_data     (CutTestContext *context);
gboolean      cut_test_context_have_data      (CutTestContext *context);
CutTestData  *cut_test_context_get_current_data
                                              (CutTestContext *context);
guint         cut_test_context_get_n_data     (CutTestContext *context);

void          cut_test_context_set_failed     (CutTestContext *context,
                                               gboolean        failed);
gboolean      cut_test_context_is_failed      (CutTestContext *context);

void          cut_test_context_emit_signal    (CutTestContext *context,
                                               CutTestResult  *result);

gchar        *cut_test_context_to_xml         (CutTestContext *context);
void          cut_test_context_to_xml_string  (CutTestContext *context,
                                               GString        *string,
                                               guint           indent);

gchar        *cut_test_context_build_fixture_data_path_va_list
                                              (CutTestContext *context,
                                               const gchar    *path,
                                               va_list         args);
const gchar  *cut_test_context_get_fixture_data_string
                                              (CutTestContext *context,
                                               GError        **error,
                                               gchar         **full_path,
                                               const gchar    *path,
                                               ...) G_GNUC_NULL_TERMINATED;
const gchar  *cut_test_context_get_fixture_data_string_va_list
                                              (CutTestContext *context,
                                               GError        **error,
                                               gchar         **full_path,
                                               const gchar    *path,
                                               va_list         args);

gchar        *cut_test_context_build_source_filename
                                              (CutTestContext *context,
                                               const gchar    *filename);

G_END_DECLS

#endif /* __CUT_TEST_CONTEXT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
