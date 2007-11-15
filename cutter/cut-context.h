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

#ifndef __CUT_CONTEXT_H__
#define __CUT_CONTEXT_H__

#include <glib-object.h>

#include "cut-output.h"

G_BEGIN_DECLS

#define CUT_TYPE_CONTEXT            (cut_context_get_type ())
#define CUT_CONTEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_CONTEXT, CutContext))
#define CUT_CONTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_CONTEXT, CutContextClass))
#define CUT_IS_CONTEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_CONTEXT))
#define CUT_IS_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_CONTEXT))
#define CUT_CONTEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_CONTEXT, CutContextClass))

typedef struct _CutContextClass    CutContextClass;

struct _CutContext
{
    GObject object;
};

struct _CutContextClass
{
    GObjectClass parent_class;
};

extern GPrivate *cut_context_private;

GType        cut_context_get_type  (void) G_GNUC_CONST;

CutContext  *cut_context_new (void);

CutVerboseLevel cut_context_parse_verbose_level (CutContext *context,
                                                 const gchar *name);
void  cut_context_set_verbose_level         (CutContext *context,
                                             CutVerboseLevel level);
void  cut_context_set_verbose_level_by_name (CutContext *context,
                                             const gchar *name);
void  cut_context_set_source_directory      (CutContext *context,
                                             const gchar *directory);
const gchar *cut_context_get_source_directory (CutContext *context);
void  cut_context_set_use_color             (CutContext *context,
                                             gboolean    use_color);

void     cut_context_set_multi_thread       (CutContext *context,
                                             gboolean    use_multi_thread);
gboolean cut_context_get_multi_thread       (CutContext *context);

void  cut_context_start_test                (CutContext *context,
                                             CutTest    *test);
void  cut_context_start_test_case           (CutContext *context,
                                             CutTestCase *test_case);
void  cut_context_start_test_suite          (CutContext *context,
                                             CutTestSuite *test_suite);

guint cut_context_get_n_tests               (CutContext *context);
guint cut_context_get_n_assertions          (CutContext *context);
guint cut_context_get_n_failures            (CutContext *context);
guint cut_context_get_n_errors              (CutContext *context);
guint cut_context_get_n_pendings            (CutContext *context);
guint cut_context_get_n_notifications       (CutContext *context);

const GList *cut_context_get_results        (CutContext *context);

gboolean cut_context_is_crashed             (CutContext *context);
const gchar *cut_context_get_stack_trace    (CutContext *context);


G_END_DECLS

#endif /* __CUT_CONTEXT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
