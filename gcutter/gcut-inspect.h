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

#ifndef __GCUT_INSPECT_H__
#define __GCUT_INSPECT_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-inspect
 * @title: WRITE ME
 * @short_description: WRITE ME
 *
 * WRITE ME
 *
 * WRITE ME
 *
 * Since: 1.0.6
 */


void        gcut_inspect_direct       (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);
void        gcut_inspect_int          (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);
void        gcut_inspect_string       (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);
void        gcut_inspect_gtype        (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);
void        gcut_inspect_flags        (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);
void        gcut_inspect_enum         (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

G_END_DECLS

#endif /* __GCUT_INSPECT_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
