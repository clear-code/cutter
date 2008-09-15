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

#ifndef __GDKCUT_PIXBUF_H__
#define __GDKCUT_PIXBUF_H__

#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

gboolean         gdkcut_pixbuf_equal_property   (const GdkPixbuf *pixbuf1,
                                                 const GdkPixbuf *pixbuf2);
gboolean         gdkcut_pixbuf_equal_content    (const GdkPixbuf *pixbuf1,
                                                 const GdkPixbuf *pixbuf2,
                                                 guint            threshold);

gchar           *gdkcut_pixbuf_inspect_property (const GdkPixbuf *pixbuf);

GdkPixbuf       *gdkcut_pixbuf_diff             (const GdkPixbuf *pixbuf1,
                                                 const GdkPixbuf *pixbuf2,
                                                 guint            threshold);
gchar           *gdkcut_pixbuf_save_diff        (const GdkPixbuf *pixbuf1,
                                                 const GdkPixbuf *pixbuf2,
                                                 guint            threshold,
                                                 const gchar     *prefix);

G_END_DECLS

#endif /* __GDKCUT_PIXBUF_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
