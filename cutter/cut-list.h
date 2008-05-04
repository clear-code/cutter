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

#ifndef __CUT_LIST_H__
#define __CUT_LIST_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef void   (*CutInspectFunc) (GString *string,
                                  gconstpointer data,
                                  gpointer user_data);

gboolean         cut_list_equal                (const GList *list1,
                                                const GList *list2,
                                                GCompareDataFunc compare_func,
                                                gpointer user_data);
gchar           *cut_list_inspect              (const GList *list,
                                                CutInspectFunc inspect_func,
                                                gpointer user_data);

gboolean         cut_list_equal_int            (const GList *list1,
                                                const GList *list2);
gchar           *cut_list_inspect_int          (const GList *list);

gboolean         cut_list_equal_string         (const GList *list1,
                                                const GList *list2);
gchar           *cut_list_inspect_string       (const GList *list);


G_END_DECLS

#endif /* __CUT_LIST_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
