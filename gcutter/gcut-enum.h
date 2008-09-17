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

#ifndef __GCUT_ENUM_H__
#define __GCUT_ENUM_H__

#include <glib-object.h>

G_BEGIN_DECLS

gboolean         gcut_enum_equal                (GType enum_type,
                                                 gint  enum1,
                                                 gint  enum2);
gchar           *gcut_enum_inspect              (GType enum_type,
                                                 gint  enum_value);

gboolean         gcut_flags_equal               (GType flags_type,
                                                 guint flags1,
                                                 guint flags2);
gchar           *gcut_flags_inspect             (GType flags_type,
                                                 guint flags);


G_END_DECLS

#endif /* __GCUT_ENUM_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
