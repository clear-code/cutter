/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
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

#ifndef __CUT_CAIRO_H__
#define __CUT_CAIRO_H__

#include <glib.h>
#include <cairo.h>
#include <pango/pangocairo.h>

#include <cutter/cut-test-result.h>

G_BEGIN_DECLS

PangoLayout *cut_cairo_create_pango_layout        (cairo_t *cr,
                                                   const gchar *utf8,
                                                   gint font_size);
void         cut_cairo_set_source_result_color    (cairo_t *cr,
                                                   CutTestResultStatus status);
#endif /* __CUT_CAIRO_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
