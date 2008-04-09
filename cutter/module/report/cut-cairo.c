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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <math.h>

#include "cut-cairo.h"

PangoLayout *
cut_cairo_create_pango_layout (cairo_t *cr, const gchar *utf8, gint font_size)
{
    PangoLayout *layout;
    PangoFontDescription *description;
    gchar *description_string;

    if (!utf8)
        return NULL;

    layout = pango_cairo_create_layout(cr);

    if (font_size < 0)
        description_string = g_strdup("Mono");
    else
        description_string = g_strdup_printf("Mono %d", font_size);
    description = pango_font_description_from_string(description_string);
    g_free(description_string);

    pango_layout_set_font_description(layout, description);
    pango_font_description_free(description);

    pango_layout_set_text(layout, utf8, -1);

    return layout;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
