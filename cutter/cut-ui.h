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

#ifndef __CUT_UI_H__
#define __CUT_UI_H__

#include <glib-object.h>
#include <cutter/cut-runner.h>

G_BEGIN_DECLS

#define CUT_TYPE_UI             (cut_ui_get_type ())
#define CUT_UI(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UI, CutUI))
#define CUT_UI_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), CUT_TYPE_UI, CutUIClass))
#define CUT_IS_UI(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UI))
#define CUT_IS_UI_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), CUT_TYPE_UI))
#define CUT_UI_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), CUT_TYPE_UI, CutUIClass))

typedef struct _CutUI       CutUI;         /* Dummy typedef */
typedef struct _CutUIClass  CutUIClass;

struct _CutUIClass
{
    GTypeInterface base_iface;

    gboolean (*run) (CutUI *ui, CutRunner *runner);
};

GType    cut_ui_get_type (void) G_GNUC_CONST;

void     cut_ui_init        (void);
void     cut_ui_quit        (void);
GObject *cut_ui_new         (const gchar *name,
                             const gchar *first_property,
                             ...);

gboolean cut_ui_run (CutUI *ui, CutRunner *runner);

G_END_DECLS

#endif /* __CUT_UI_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
