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

#ifndef __CUT_DIFF_H__
#define __CUT_DIFF_H__

#include <glib-object.h>
#include <cutter/cut-public.h>

G_BEGIN_DECLS

#define CUT_TYPE_DIFFER            (cut_differ_get_type ())
#define CUT_DIFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_DIFFER, CutDiffer))
#define CUT_DIFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_DIFFER, CutDifferClass))
#define CUT_IS_DIFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_DIFFER))
#define CUT_IS_DIFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_DIFFER))
#define CUT_DIFFER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_DIFFER, CutDifferClass))

typedef struct _CutDiffer         CutDiffer;
typedef struct _CutDifferClass    CutDifferClass;

struct _CutDiffer
{
    GObject object;
};

struct _CutDifferClass
{
    GObjectClass parent_class;

    void (*diff) (CutDiffer *differ, GArray *result);
};

GType         cut_differ_get_type         (void) G_GNUC_CONST;

gchar        *cut_differ_diff             (CutDiffer   *differ);
gchar       **cut_differ_get_from         (CutDiffer   *differ);
gchar       **cut_differ_get_to           (CutDiffer   *differ);

void          cut_differ_util_append_with_tag
                                          (GArray      *result,
                                           const gchar *tag,
                                           gchar      **lines,
                                           guint        begin,
                                           guint        end);
gboolean      cut_differ_util_is_space_character
                                          (gpointer     data,
                                           gpointer     user_data);
guint         cut_differ_util_compute_width
                                          (const gchar *string,
                                           guint         begin,
                                           guint         end);
void          cut_differ_util_append_n_character
                                         (GString       *string,
                                          gchar          character,
                                          guint          n);

gboolean      cut_diff_is_interested      (const gchar *diff);
gboolean      cut_diff_need_fold          (const gchar *diff);

G_END_DECLS

#endif /* __CUT_DIFF_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
