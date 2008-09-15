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

#ifndef __GDKCUT_PIXBUF_ASSERTIONS_H__
#define __GDKCUT_PIXBUF_ASSERTIONS_H__

#include <glib.h>

#include <gdkcutter-pixbuf/gdkcut-pixbuf.h>

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
 * @expected: an expected GdkPixbuf *.
 * @actual: an actual GdkPixbuf *.
 * @threshold: an threshold used for detecting pixel difference.
 * @...: optional format string, followed by parameters to insert
 *       into the format string (as with printf())
 *
 * Passes if @expected == @actual within threshold difference.
 *
 * Since: 1.0.5
 */
#define gdkcut_pixbuf_assert_equal(expected, actual, threshold, ...) do \
{                                                                       \
    GdkPixbuf *_expected;                                               \
    GdkPixbuf *_actual;                                                 \
    gint _threshold;                                                    \
                                                                        \
    _expected = (expected);                                             \
    _actual = (actual);                                                 \
    _threshold = (threshold);                                           \
    if (gdkcut_pixbuf_equal_content(_expected, _actual, _threshold)) {  \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *message;                                           \
                                                                        \
        message = cut_take_printf("<%s == %s> (%s)",                    \
                                  #expected, #actual, #threshold);      \
        if (gdkcut_pixbuf_equal_property(_expected, _actual)) {         \
            gchar *diff_image_file;                                     \
                                                                        \
            diff_iamge_file =                                           \
                gdkcut_pixbuf_save_diff(_expected,                      \
                                        _actual,                        \
                                        threshold,                      \
                                        cut_take_printf("%s-%d",        \
                                                        __FILE__,       \
                                                        __LINE__));     \
            message = cut_take_printf("%s\n"                            \
                                      " threshold: <%d>\n"              \
                                      "diff image: <%s>",               \
                                      message,                          \
                                      _threshold,                       \
                                      diff_iamge_file);                 \
            g_free(diff_iamge_file);                                    \
        } else {                                                        \
            gchar *expected_property;                                   \
            gchar *actual_property;                                     \
                                                                        \
            expected_property =                                         \
                gdkcut_pixbuf_inspect_property(_expected);              \
            actual_property =                                           \
                gdkcut_pixbuf_inspect_property(_actual);                \
            message = cut_take_printf("%s\n"                            \
                                      "expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      message,                          \
                                      expected_property,                \
                                      actual_property);                 \
            message = cut_append_diff(message,                          \
                                      expected_property,                \
                                      actual_property);                 \
        }                                                               \
        cut_test_fail(FAILURE, message, ## __VA_ARGS__);                \
    }                                                                   \
} while(0)

G_END_DECLS

#endif /* __GDKCUT_PIXBUF_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
