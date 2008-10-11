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

#ifndef __GCUT_LIST_H__
#define __GCUT_LIST_H__

#include <glib-object.h>

#include <gcutter/gcut-types.h>

G_BEGIN_DECLS

gboolean         gcut_list_equal                (const GList *list1,
                                                 const GList *list2,
                                                 GEqualFunc   equal_func);
gchar           *gcut_list_inspect              (const GList *list,
                                                 GCutInspectFunc inspect_func,
                                                 gpointer user_data);

gboolean         gcut_list_int_equal            (const GList *list1,
                                                 const GList *list2);
gchar           *gcut_list_int_inspect          (const GList *list);

gboolean         gcut_list_uint_equal           (const GList *list1,
                                                 const GList *list2);
gchar           *gcut_list_uint_inspect         (const GList *list);

gboolean         gcut_list_string_equal         (const GList *list1,
                                                 const GList *list2);
gchar           *gcut_list_string_inspect       (const GList *list);

gchar           *gcut_list_object_inspect       (const GList *list);


G_END_DECLS

#endif /* __GCUT_LIST_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
