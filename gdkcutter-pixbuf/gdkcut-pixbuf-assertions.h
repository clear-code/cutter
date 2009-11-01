/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __GDKCUT_PIXBUF_ASSERTIONS_H__
#define __GDKCUT_PIXBUF_ASSERTIONS_H__

#include <glib.h>

#include <gdkcutter-pixbuf/gdkcut-pixbuf-assertions-helper.h>

G_BEGIN_DECLS

/**
 * SECTION: gdkcut-pixbuf-assertions
 * @title: Assertions with gdk-pixbuf support
 * @short_description: Checks that your program works as you
 * expect with gdk-pixbuf support.
 *
 */

/**
 * gdkcut_pixbuf_assert_equal:
 * @expected: an expected #GdkPixbuf.
 * @actual: an actual #GdkPixbuf.
 * @threshold: an threshold used for detecting pixel difference.
 *
 * Passes if @expected == @actual. If difference of each
 * corresponding pixel value is within threshold, @expected
 * and @actual are treated as equal image.
 *
 * If threshold is 3, pixel value of @expected at (1, 1) is
 * 127 and pixel value of @actual at (1, 1) is 131, @actual
 * doesn't equals to @expected. If pixel value of @actual at
 * (1, 1) is 130, @actual may equal to @expected. (All other
 * corresponding pixel values are needed to fill this
 * condition.)
 *
 * Since: 1.0.5
 */
#define gdkcut_pixbuf_assert_equal(expected, actual, threshold, ...) do \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        cut_test_with_user_message(                                     \
            gdkcut_pixbuf_assert_equal_helper(expected, actual,         \
                                              threshold,                \
                                              #expected, #actual,       \
                                              #threshold),              \
            __VA_ARGS__),                                               \
        gdkcut_pixbuf_assert_equal(expected, actual, threshold));       \
} while (0)

G_END_DECLS

#endif /* __GDKCUT_PIXBUF_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
