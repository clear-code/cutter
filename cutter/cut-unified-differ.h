/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2010  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_UNIFIED_DIFFER_H__
#define __CUT_UNIFIED_DIFFER_H__

#include <glib-object.h>
#include <cutter/cut-differ.h>

G_BEGIN_DECLS

#define CUT_TYPE_UNIFIED_DIFFER            (cut_unified_differ_get_type ())
#define CUT_UNIFIED_DIFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UNIFIED_DIFFER, CutUnifiedDiffer))
#define CUT_UNIFIED_DIFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_UNIFIED_DIFFER, CutUnifiedDifferClass))
#define CUT_IS_UNIFIED_DIFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UNIFIED_DIFFER))
#define CUT_IS_UNIFIED_DIFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_UNIFIED_DIFFER))
#define CUT_UNIFIED_DIFFER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_UNIFIED_DIFFER, CutUnifiedDifferClass))

typedef struct _CutUnifiedDiffer         CutUnifiedDiffer;
typedef struct _CutUnifiedDifferClass    CutUnifiedDifferClass;

struct _CutUnifiedDiffer
{
    CutDiffer object;
};

struct _CutUnifiedDifferClass
{
    CutDifferClass parent_class;
};

GType        cut_unified_differ_get_type       (void) G_GNUC_CONST;

CutDiffer   *cut_unified_differ_new            (const gchar *from,
                                                const gchar *to);

void         cut_unified_differ_set_from_label (CutDiffer   *differ,
                                                const gchar *label);
const gchar *cut_unified_differ_get_from_label (CutDiffer   *differ);

void         cut_unified_differ_set_to_label   (CutDiffer   *differ,
                                                const gchar *label);
const gchar *cut_unified_differ_get_to_label   (CutDiffer   *differ);

G_END_DECLS

#endif /* __CUT_UNIFIED_DIFFER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
