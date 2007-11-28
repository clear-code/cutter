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

#ifndef __CUT_UI_H__
#define __CUT_UI_H__

#include <glib-object.h>

#include <cutter/cut-private.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-verbose-level.h>

G_BEGIN_DECLS

#define CUT_TYPE_UI            (cut_ui_get_type ())
#define CUT_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UI, CutUI))
#define CUT_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_UI, CutUIClass))
#define CUT_IS_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UI))
#define CUT_IS_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_UI))
#define CUT_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_UI, CutUIClass))

typedef struct _CutUI         CutUI;
typedef struct _CutUIClass    CutUIClass;

struct _CutUI
{
    GObject object;
};

struct _CutUIClass
{
    GObjectClass parent_class;

    gboolean (*run)                (CutUI    *ui,
                                    CutTestSuite *test_suite,
                                    CutRunner   *runner);
};

GType           cut_ui_get_type  (void) G_GNUC_CONST;

void            cut_ui_init        (void);
void            cut_ui_quit        (void);

const gchar    *cut_ui_get_default_module_dir   (void);
void            cut_ui_set_default_module_dir   (const gchar *dir);

void            cut_ui_load        (const gchar *base_dir);
void            cut_ui_unload      (void);
GList          *cut_ui_get_registered_types (void);
GList          *cut_ui_get_log_domains      (void);

CutUI          *cut_ui_new         (const gchar *name,
                                    const gchar *first_property,
                                    ...);

gboolean        cut_ui_run         (CutUI        *ui,
                                    CutTestSuite *test_suite,
                                    CutRunner    *runner);
G_END_DECLS

#endif /* __CUT_UI_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
