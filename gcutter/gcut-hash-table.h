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

#ifndef __GCUT_HASH_TABLE_H__
#define __GCUT_HASH_TABLE_H__

#include <glib-object.h>
#include <gcutter/gcut-list.h>

G_BEGIN_DECLS

gboolean         gcut_hash_table_equal          (GHashTable *hash1,
                                                 GHashTable *hash2,
                                                 GCompareDataFunc compare_func,
                                                 gpointer user_data);
gchar           *gcut_hash_table_inspect        (GHashTable *hash,
                                                 GCutInspectFunc key_inspect_func,
                                                 GCutInspectFunc value_inspect_func,
                                                 gpointer user_data);

gboolean         gcut_hash_table_string_equal   (GHashTable *hash1,
                                                 GHashTable *hash2);
gchar           *gcut_hash_table_string_string_inspect
                                                (GHashTable *hash);


G_END_DECLS

#endif /* __GCUT_HASH_TABLE_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
