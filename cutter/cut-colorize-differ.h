/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_COLORIZE_DIFFER_H__
#define __CUT_COLORIZE_DIFFER_H__

#include <glib-object.h>
#include <cutter/cut-readable-differ.h>

G_BEGIN_DECLS

#define CUT_TYPE_COLORIZE_DIFFER            (cut_colorize_differ_get_type ())
#define CUT_COLORIZE_DIFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_COLORIZE_DIFFER, CutColorizeDiffer))
#define CUT_COLORIZE_DIFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_COLORIZE_DIFFER, CutColorizeDifferClass))
#define CUT_IS_COLORIZE_DIFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_COLORIZE_DIFFER))
#define CUT_IS_COLORIZE_DIFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_COLORIZE_DIFFER))
#define CUT_COLORIZE_DIFFER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_COLORIZE_DIFFER, CutColorizeDifferClass))

typedef struct _CutColorizeDiffer         CutColorizeDiffer;
typedef struct _CutColorizeDifferClass    CutColorizeDifferClass;

struct _CutColorizeDiffer
{
    CutReadableDiffer object;
};

struct _CutColorizeDifferClass
{
    CutReadableDifferClass parent_class;
};

GType      cut_colorize_differ_get_type  (void) G_GNUC_CONST;

CutDiffer *cut_colorize_differ_new     (const gchar *from,
                                        const gchar *to);
G_END_DECLS

#endif /* __CUT_COLORIZE_DIFFER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
