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

#include "gdkcut-pixbuf.h"

gboolean
gdkcut_pixbuf_equal_property (const GdkPixbuf *pixbuf1, const GdkPixbuf *pixbuf2)
{
    if (pixbuf1 == pixbuf2)
        return TRUE;

    if (pixbuf1 == NULL || pixbuf2 == NULL)
        return FALSE;

    if (gdk_pixbuf_get_n_channels(pixbuf1) != gdk_pixbuf_get_n_channels(pixbuf2))
        return FALSE;

    if (gdk_pixbuf_get_has_alpha(pixbuf1) != gdk_pixbuf_get_has_alpha(pixbuf2))
        return FALSE;

    if (gdk_pixbuf_get_bits_per_sample(pixbuf1) !=
        gdk_pixbuf_get_bits_per_sample(pixbuf2))
        return FALSE;

    if (gdk_pixbuf_get_width(pixbuf1) != gdk_pixbuf_get_width(pixbuf2))
        return FALSE;

    if (gdk_pixbuf_get_height(pixbuf1) != gdk_pixbuf_get_height(pixbuf2))
        return FALSE;

    if (gdk_pixbuf_get_rowstride(pixbuf1) != gdk_pixbuf_get_rowstride(pixbuf2))
        return FALSE;

    return TRUE;
}

static gboolean
pixels_equal (const guchar *pixels1, const guchar *pixels2,
              guint threshold, gint n_channels, gboolean has_alpha,
              gint bits_per_sample, gint width, gint height, gint row_stride)
{
    gint x, y;
    gint n_row_pixels;

    n_row_pixels = width * n_channels;
    for (y = 0; y < height; y++) {
        const guchar *row1, *row2;

        row1 = pixels1 + y * row_stride;
        row2 = pixels2 + y * row_stride;
        for (x = 0; x < n_row_pixels; x += n_channels) {
            gint channel;

            for (channel = 0; channel < n_channels; channel++) {
                guchar value1, value2;

                value1 = row1[x + channel];
                value2 = row2[x + channel];
                if (value1 != value2) {
                    guchar diff;

                    if (value1 > value2)
                        diff = value1 - value2;
                    else
                        diff = value2 - value1;

                    if (diff > threshold)
                        return FALSE;
                }
            }
        }
    }

    return TRUE;
}


gboolean
gdkcut_pixbuf_equal_content (const GdkPixbuf *pixbuf1,
                             const GdkPixbuf *pixbuf2,
                             guint            threshold)
{
    if (pixbuf1 == pixbuf2)
        return TRUE;

    if (!gdkcut_pixbuf_equal_property(pixbuf1, pixbuf2))
        return FALSE;

    return pixels_equal(gdk_pixbuf_get_pixels(pixbuf1),
                        gdk_pixbuf_get_pixels(pixbuf2),
                        threshold,
                        gdk_pixbuf_get_n_channels(pixbuf1),
                        gdk_pixbuf_get_has_alpha(pixbuf1),
                        gdk_pixbuf_get_bits_per_sample(pixbuf1),
                        gdk_pixbuf_get_width(pixbuf1),
                        gdk_pixbuf_get_height(pixbuf1),
                        gdk_pixbuf_get_rowstride(pixbuf1));
}

gchar *
gdkcut_pixbuf_inspect_property (const GdkPixbuf *pixbuf)
{
    GString *inspected;

    if (!pixbuf)
        return g_strdup("(null)");

    inspected = g_string_new("#<GdkPixbuf: ");

    g_string_append_printf(inspected,
                           "n-channels: %d; ",
                           gdk_pixbuf_get_n_channels(pixbuf));
    g_string_append_printf(inspected,
                           "has-alpha: %s; ",
                           gdk_pixbuf_get_has_alpha(pixbuf) ? "TRUE" : "FALSE");
    g_string_append_printf(inspected,
                           "bits-per-sample: %d; ",
                           gdk_pixbuf_get_bits_per_sample(pixbuf));
    g_string_append_printf(inspected,
                           "width: %d; ",
                           gdk_pixbuf_get_width(pixbuf));
    g_string_append_printf(inspected,
                           "height: %d; ",
                           gdk_pixbuf_get_height(pixbuf));
    g_string_append_printf(inspected,
                           "row-stride: %d",
                           gdk_pixbuf_get_rowstride(pixbuf));

    g_string_append(inspected, ">");

    return g_string_free(inspected, FALSE);
}

GdkPixbuf *
gdkcut_pixbuf_diff (const GdkPixbuf *pixbuf1,
                    const GdkPixbuf *pixbuf2,
                    guint            threshold)
{
    return NULL;
}

gchar *
gdkcut_pixbuf_save_diff (const GdkPixbuf *pixbuf1,
                         const GdkPixbuf *pixbuf2,
                         guint            threshold,
                         const gchar     *prefix)
{
    return NULL;
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
