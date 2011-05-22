/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2011  Kouhei Sutou <kou@clear-code.com>
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

static inline gboolean
equal_channel (guchar channel1, guchar channel2, guint threshold)
{
    guchar diff;

    if (channel1 == channel2)
        return TRUE;

    if (channel1 > channel2)
        diff = channel1 - channel2;
    else
        diff = channel2 - channel1;

    return diff <= threshold;
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
                if (!equal_channel(row1[x + channel], row2[x + channel],
                                   threshold))
                    return FALSE;
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

static void
draw_frame (GdkPixbuf *diff, gint sub_width, gint sub_height,
            guchar red, guchar green, guchar blue, guchar alpha)
{
    guchar *pixels;
    gint n_channels, row_stride, width, height;
    gint x, y, i, channel;
    guchar pixel[4];
    gint xes[3], yes[3];

    pixel[0] = red;
    pixel[1] = green;
    pixel[2] = blue;
    pixel[3] = alpha;

    xes[0] = 0;
    xes[1] = sub_width + 1;
    xes[2] = sub_width * 2 + 2;

    yes[0] = 0;
    yes[1] = sub_height + 1;
    yes[2] = sub_height * 2 + 2;

    n_channels = gdk_pixbuf_get_n_channels(diff);
    row_stride = gdk_pixbuf_get_rowstride(diff);
    width = gdk_pixbuf_get_width(diff);
    height = gdk_pixbuf_get_height(diff);
    pixels = gdk_pixbuf_get_pixels(diff);

    for (y = 0; y < height; y++) {
        for (i = 0; i < sizeof(xes) / sizeof(*xes); i++) {
            guchar *target_pixel;

            target_pixel = pixels + xes[i] * n_channels + y * row_stride;
            for (channel = 0; channel < n_channels; channel++) {
                target_pixel[channel] = pixel[channel];
            }
        }
    }

    for (x = 0; x < width; x++) {
        for (i = 0; i < sizeof(yes) / sizeof(*yes); i++) {
            guchar *target_pixel;

            target_pixel = pixels + x * n_channels + yes[i] * row_stride;
            for (channel = 0; channel < n_channels; channel++) {
                target_pixel[channel] = pixel[channel];
            }
        }
    }
}

static void
draw_pixbuf (GdkPixbuf *diff, const GdkPixbuf *source,
             gint x_offset, gint y_offset)
{
    guchar *pixels, *source_pixels;
    gint n_channels, row_stride;
    gint source_n_channels, source_row_stride, source_width, source_height;
    gint x, y, channel;

    n_channels = gdk_pixbuf_get_n_channels(diff);
    row_stride = gdk_pixbuf_get_rowstride(diff);
    pixels = gdk_pixbuf_get_pixels(diff);

    source_n_channels = gdk_pixbuf_get_n_channels(source);
    source_row_stride = gdk_pixbuf_get_rowstride(source);
    source_width = gdk_pixbuf_get_width(source);
    source_height = gdk_pixbuf_get_height(source);
    source_pixels = gdk_pixbuf_get_pixels(source);

    for (y = 0; y < source_height; y++) {
        guchar *row, *source_row;

        row = pixels + (y + y_offset) * row_stride;
        source_row = source_pixels + y * source_row_stride;
        for (x = 0; x < source_width; x++) {
            guchar *target_pixel, *source_target_pixel;

            target_pixel = row + (x + x_offset) * n_channels;
            source_target_pixel = source_row + x * source_n_channels;
            for (channel = 0; channel < source_n_channels; channel++) {
                target_pixel[channel] = source_target_pixel[channel];
            }
            for (; channel < n_channels; channel++) {
                target_pixel[channel] = 0xff;
            }
        }
    }
}

static void
draw_diff_highlight (GdkPixbuf *diff,
                     const GdkPixbuf *pixbuf1, const GdkPixbuf *pixbuf2,
                     guint threshold, gint x_offset, gint y_offset)
{
    guchar *pixels, *target_pixels1, *target_pixels2;
    guchar same_pixel[4], different_pixel[4];
    gint n_channels, row_stride;
    gint target_n_channels, target_row_stride, target_width, target_height;
    gint x, y, channel;

    n_channels = gdk_pixbuf_get_n_channels(diff);
    row_stride = gdk_pixbuf_get_rowstride(diff);
    pixels = gdk_pixbuf_get_pixels(diff);

    target_n_channels = gdk_pixbuf_get_n_channels(pixbuf1);
    target_row_stride = gdk_pixbuf_get_rowstride(pixbuf1);
    target_width = gdk_pixbuf_get_width(pixbuf1);
    target_height = gdk_pixbuf_get_height(pixbuf1);
    target_pixels1 = gdk_pixbuf_get_pixels(pixbuf1);
    target_pixels2 = gdk_pixbuf_get_pixels(pixbuf2);

    same_pixel[0] = 0x00;
    same_pixel[1] = 0x00;
    same_pixel[2] = 0x00;
    same_pixel[3] = 0xff;

    different_pixel[0] = 0xff;
    different_pixel[1] = 0x00;
    different_pixel[2] = 0x00;
    different_pixel[3] = 0xff;

    for (y = 0; y < target_height; y++) {
        guchar *row, *target_row1, *target_row2;

        row = pixels + (y + y_offset) * row_stride;
        target_row1 = target_pixels1 + y * target_row_stride;
        target_row2 = target_pixels2 + y * target_row_stride;
        for (x = 0; x < target_width; x++) {
            gboolean different = FALSE;
            guchar *result_pixel;
            guchar *pixel, *target_pixel1, *target_pixel2;

            pixel = row + (x + x_offset) * n_channels;
            target_pixel1 = target_row1 + x * target_n_channels;
            target_pixel2 = target_row2 + x * target_n_channels;
            for (channel = 0; channel < target_n_channels; channel++) {
                guchar value1, value2;

                value1 = target_pixel1[channel];
                value2 = target_pixel2[channel];
                if (!equal_channel(value1, value2, threshold)) {
                    different = TRUE;
                    break;
                }
            }

            if (different) {
                result_pixel = different_pixel;
            } else {
                result_pixel = same_pixel;
            }
            for (channel = 0; channel < n_channels; channel++) {
                pixel[channel] = result_pixel[channel];
            }
        }
    }
}

static void
draw_diff_emphasis (GdkPixbuf *diff,
                    const GdkPixbuf *pixbuf1, const GdkPixbuf *pixbuf2,
                    guint threshold, gint x_offset, gint y_offset)
{
    guchar *pixels, *target_pixels1, *target_pixels2;
    gint n_channels, row_stride;
    gint target_n_channels, target_row_stride, target_width, target_height;
    gint x, y, channel;

    n_channels = gdk_pixbuf_get_n_channels(diff);
    row_stride = gdk_pixbuf_get_rowstride(diff);
    pixels = gdk_pixbuf_get_pixels(diff);

    target_n_channels = gdk_pixbuf_get_n_channels(pixbuf1);
    target_row_stride = gdk_pixbuf_get_rowstride(pixbuf1);
    target_width = gdk_pixbuf_get_width(pixbuf1);
    target_height = gdk_pixbuf_get_height(pixbuf1);
    target_pixels1 = gdk_pixbuf_get_pixels(pixbuf1);
    target_pixels2 = gdk_pixbuf_get_pixels(pixbuf2);

    for (y = 0; y < target_height; y++) {
        guchar *row, *target_row1, *target_row2;

        row = pixels + (y + y_offset) * row_stride;
        target_row1 = target_pixels1 + y * target_row_stride;
        target_row2 = target_pixels2 + y * target_row_stride;
        for (x = 0; x < target_width; x++) {
            gboolean different = FALSE;
            guchar result_pixel[4];
            guchar *pixel, *target_pixel1, *target_pixel2;

            pixel = row + (x + x_offset) * n_channels;
            target_pixel1 = target_row1 + x * target_n_channels;
            target_pixel2 = target_row2 + x * target_n_channels;
            for (channel = 0; channel < target_n_channels; channel++) {
                guchar value1, value2;

                value1 = target_pixel1[channel];
                value2 = target_pixel2[channel];
                if (equal_channel(value1, value2, threshold)) {
                    result_pixel[channel] = value1 / 3;
                } else {
                    different = TRUE;
                    result_pixel[channel] = value1 * 3;
                }
            }

            if (different || target_n_channels < 4)
                result_pixel[3] = 0xff;

            for (channel = 0; channel < n_channels; channel++) {
                pixel[channel] = result_pixel[channel];
            }
        }
    }
}

GdkPixbuf *
gdkcut_pixbuf_diff (const GdkPixbuf *pixbuf1,
                    const GdkPixbuf *pixbuf2,
                    guint            threshold)
{
    GdkPixbuf *diff;
    gint bits_per_sample, width, height;

    if (!gdkcut_pixbuf_equal_property(pixbuf1, pixbuf2))
        return NULL;

    if (gdkcut_pixbuf_equal_content(pixbuf1, pixbuf2, threshold))
        return NULL;

    bits_per_sample = gdk_pixbuf_get_bits_per_sample(pixbuf1);
    width = gdk_pixbuf_get_width(pixbuf1);
    height = gdk_pixbuf_get_height(pixbuf1);
    diff = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(pixbuf1),
                          TRUE,
                          bits_per_sample,
                          width * 2 + 3,
                          height * 2 + 3);
    gdk_pixbuf_fill(diff, 0xffffff00);
    draw_frame(diff, width, height, 0xff, 0xff, 0xff, 0xff);
    draw_pixbuf(diff, pixbuf1, 1, 1);
    draw_pixbuf(diff, pixbuf2, width + 2, 1);
    draw_diff_highlight(diff, pixbuf1, pixbuf2, threshold, 1, height + 2);
    draw_diff_emphasis(diff, pixbuf1, pixbuf2, threshold, width + 2, height + 2);
    return diff;
}

gchar *
gdkcut_pixbuf_save_diff (const GdkPixbuf *pixbuf1,
                         const GdkPixbuf *pixbuf2,
                         guint            threshold,
                         const gchar     *prefix,
                         GError         **error)
{
    GdkPixbuf *diff;
    gchar *filename;
    gboolean success;

    diff = gdkcut_pixbuf_diff(pixbuf1, pixbuf2, threshold);
    if (!diff)
        return NULL;

    filename = g_strdup_printf("%s.png", prefix);
    success = gdk_pixbuf_save(diff, filename, "png", error, NULL);
    g_object_unref(diff);

    if (success)
        return filename;

    g_free(filename);
    return NULL;
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
