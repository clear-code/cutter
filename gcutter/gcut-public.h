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

#ifndef __GCUT_PUBLIC_H__
#define __GCUT_PUBLIC_H__

#include <glib-object.h>

#include <cutter/cut-public.h>

G_BEGIN_DECLS

GObject      *cut_test_context_take_g_object (CutTestContext     *context,
                                              GObject            *object);

const GError *cut_test_context_take_g_error  (CutTestContext     *context,
                                              GError             *error);

const GList  *cut_test_context_take_g_list   (CutTestContext     *context,
                                              GList              *list,
                                              CutDestroyFunction  destroy);
GHashTable   *cut_test_context_take_g_hash_table
                                             (CutTestContext     *context,
                                              GHashTable         *hash_table);
GString      *cut_test_context_take_g_string (CutTestContext     *context,
                                              GString            *string);

GString      *gcut_utils_get_fixture_data    (CutTestContext *context,
                                              const gchar   **full_path,
                                              const gchar    *path,
                                              ...) G_GNUC_NULL_TERMINATED;
GString      *gcut_utils_get_fixture_data_va_list
                                             (CutTestContext *context,
                                              const gchar   **full_path,
                                              const gchar    *path,
                                              va_list         args);

G_END_DECLS

#endif /* __GCUT_PUBLIC_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
