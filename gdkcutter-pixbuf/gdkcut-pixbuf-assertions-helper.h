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

#ifndef __GDKCUT_PIXBUF_ASSERTIONS_HELPER_H__
#define __GDKCUT_PIXBUF_ASSERTIONS_HELPER_H__

#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gcutter/gcut-assertions-helper.h>

G_BEGIN_DECLS

void  gdkcut_pixbuf_assert_equal_helper (const GdkPixbuf *expected,
                                         const GdkPixbuf *actual,
                                         guint            threshold,
                                         const gchar     *expression_expected,
                                         const gchar     *expression_actual,
                                         const gchar     *expression_threshold);

G_END_DECLS

#endif /* __GDKCUT_PIXBUF_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
