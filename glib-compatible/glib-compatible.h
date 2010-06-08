/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 */

#ifndef __GLIB_COMPATIBLE_H__
#define __GLIB_COMPATIBLE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#if !GLIB_CHECK_VERSION(2, 14, 0)
#  define g_hash_table_get_keys(hash_table) \
    gcompatible_hash_table_get_keys(hash_table)
GList         *gcompatible_hash_table_get_keys (GHashTable *hash_table);
#endif

G_END_DECLS

#endif /* __GLIB_COMPATIBLE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
