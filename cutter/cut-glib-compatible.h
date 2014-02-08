/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2013  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_GLIB_COMPATIBLE_H__
#define __CUT_GLIB_COMPATIBLE_H__

#include <glib.h>

G_BEGIN_DECLS

#if !GLIB_CHECK_VERSION(2, 32, 0)
#  define GPrivate                  GStaticPrivate
#  define G_PRIVATE_INIT(notify)    G_STATIC_PRIVATE_INIT
#  undef g_private_get
#  undef g_private_set
#  define g_private_get(key)        g_static_private_get(key)
#  define g_private_set(key, value) g_static_private_set(key, value, NULL)
#else
#  define g_mutex_new()             cut_glib_compatible_mutex_new()
#  define g_mutex_free(mutex)       cut_glib_compatible_mutex_free(mutex)

GMutex *cut_glib_compatible_mutex_new (void);
void    cut_glib_compatible_mutex_free(GMutex *mutex);

#endif

G_END_DECLS

#endif /* __CUT_GLIB_COMPATIBLE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
