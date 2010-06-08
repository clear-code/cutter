/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __GLIB_COMPATIBLE_H__
#define __GLIB_COMPATIBLE_H__

#include <glib-object.h>
#if !GLIB_CHECK_VERSION(2, 14, 0)
#  include "gsequence.h"
#  include "gregex.h"
#endif

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
