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

#ifndef __CUT_OUTPUT_H__
#define __CUT_OUTPUT_H__

#include <glib-object.h>

#include "cut-module.h"
#include "cut-private.h"
#include "cut-test-result.h"
#include "cut-verbose-level.h"

G_BEGIN_DECLS

#define CUT_TYPE_OUTPUT            (cut_output_get_type ())
#define CUT_OUTPUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_OUTPUT, CutOutput))
#define CUT_OUTPUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_OUTPUT, CutOutputClass))
#define CUT_IS_OUTPUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_OUTPUT))
#define CUT_IS_OUTPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_OUTPUT))
#define CUT_OUTPUT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_OUTPUT, CutOutputClass))

typedef struct _CutOutput         CutOutput;
typedef struct _CutOutputClass    CutOutputClass;

struct _CutOutput
{
    GObject object;
};

struct _CutOutputClass
{
    GObjectClass parent_class;

    void (*on_start_test_suite)    (CutOutput *output,
                                    CutTestSuite *test_suite);
    void (*on_start_test_case)     (CutOutput *output,
                                    CutTestCase *test_case);
    void (*on_start_test)          (CutOutput *output,
                                    CutTestCase *test_case,
                                    CutTest *test);
    void (*on_success)             (CutOutput *output,
                                    CutTest *test);
    void (*on_failure)             (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
    void (*on_error)               (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
    void (*on_pending)             (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
    void (*on_notification)        (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
    void (*on_complete_test)       (CutOutput *output,
                                    CutTestCase *test_case,
                                    CutTest *test,
                                    CutTestResult *result);
    void (*on_complete_test_case)  (CutOutput *output,
                                    CutTestCase *test_case);
    void (*on_complete_test_suite) (CutOutput *output,
                                    CutContext *context,
                                    CutTestSuite *test_suite);
};

GType        cut_output_get_type  (void) G_GNUC_CONST;

void            cut_output_init        (void);
void            cut_output_quit        (void);

const gchar    *cut_output_get_default_module_dir   (void);
void            cut_output_set_default_module_dir   (const gchar *dir);

void            cut_output_load        (const gchar *base_dir);
CutModule      *cut_output_load_module (const gchar *name);
void            cut_output_unload      (void);
GList          *cut_output_get_registered_types (void);
GList          *cut_output_get_log_domains      (void);

CutOutput   *cut_output_new (const gchar *name,
                             const gchar *first_property,
                             ...);

void  cut_output_set_verbose_level         (CutOutput *output,
                                            CutVerboseLevel level);
CutVerboseLevel
      cut_output_get_verbose_level         (CutOutput *output);
void  cut_output_set_verbose_level_by_name (CutOutput *output,
                                            const gchar *name);
void  cut_output_set_source_directory      (CutOutput *output,
                                            const gchar *directory);
const gchar *
      cut_output_get_source_directory      (CutOutput *output);
void  cut_output_set_use_color             (CutOutput *output,
                                            gboolean   use_color);

void  cut_output_on_start_test_suite       (CutOutput *output,
                                            CutTestSuite *test_suite);
void  cut_output_on_start_test_case        (CutOutput *output,
                                            CutTestCase *test_case);
void  cut_output_on_start_test             (CutOutput *output,
                                            CutTestCase *test_case,
                                            CutTest *test);
void  cut_output_on_success                (CutOutput *output, CutTest *test);
void  cut_output_on_failure                (CutOutput *output,
                                            CutTest *test,
                                            CutTestResult *result);
void  cut_output_on_error                  (CutOutput *output,
                                            CutTest *test,
                                            CutTestResult *result);
void  cut_output_on_pending                (CutOutput *output,
                                            CutTest *test,
                                            CutTestResult *result);
void  cut_output_on_notification           (CutOutput *output,
                                            CutTest *test,
                                            CutTestResult *result);
void  cut_output_on_complete_test          (CutOutput *output,
                                            CutTestCase *test_case,
                                            CutTest *test,
                                            CutTestResult *result);
void  cut_output_on_complete_test_case     (CutOutput *output,
                                            CutTestCase *test_case);
void  cut_output_on_complete_test_suite    (CutOutput *output,
                                            CutContext *context,
                                            CutTestSuite *test_suite);

G_END_DECLS

#endif /* __CUT_OUTPUT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
