/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2011  Kouhei Sutou <kou@clear-code.com>
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

#include <string.h>

#include <gdkcutter-pixbuf.h>
#include <gdkcutter-pixbuf/gdkcut-pixbuf.h>
#include "../lib/cuttest-utils.h"

void test_equal_property(void);
void test_equal_content(void);
void test_equal_content_threshold(void);
void test_inspect(void);
void test_diff(void);
void test_diff_between_equal_images(void);
void test_diff_no_alpha(void);

static GdkPixbuf *pixbuf1, *pixbuf2;
static GdkPixbuf *expected_diff, *actual_diff;

void
cut_setup (void)
{
    pixbuf1 = NULL;
    pixbuf2 = NULL;
    expected_diff = NULL;
    actual_diff = NULL;

    cut_set_fixture_data_dir(cuttest_get_base_dir(),
                             "fixtures",
                             "pixbuf",
                             NULL);
}

void
cut_teardown (void)
{
    if (pixbuf1)
        g_object_unref(pixbuf1);
    if (pixbuf2)
        g_object_unref(pixbuf2);

    if (expected_diff)
        g_object_unref(expected_diff);
    if (actual_diff)
        g_object_unref(actual_diff);
}

static GdkPixbuf *
load_pixbuf (const gchar *path)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    gchar *pixbuf_path;

    pixbuf_path = cut_build_fixture_data_path(path, NULL);
    pixbuf = gdk_pixbuf_new_from_file(pixbuf_path, &error);
    g_free(pixbuf_path);

    gcut_assert_error(error);

    return pixbuf;
}

void
test_equal_property (void)
{
    pixbuf1 = load_pixbuf("dark-circle.png");
    pixbuf2 = load_pixbuf("nested-circle.png");

    cut_assert_true(gdkcut_pixbuf_equal_property(pixbuf1, pixbuf1));
    cut_assert_true(gdkcut_pixbuf_equal_property(pixbuf2, pixbuf2));
    cut_assert_true(gdkcut_pixbuf_equal_property(pixbuf1, pixbuf2));

    g_object_unref(pixbuf2);
    pixbuf2 = load_pixbuf("small-circle.png");
    cut_assert_false(gdkcut_pixbuf_equal_property(pixbuf1, pixbuf2));
}

void
test_equal_content (void)
{
    pixbuf1 = load_pixbuf("dark-circle.png");
    pixbuf2 = load_pixbuf("nested-circle.png");

    cut_assert_true(gdkcut_pixbuf_equal_content(pixbuf1, pixbuf1, 0));
    cut_assert_true(gdkcut_pixbuf_equal_content(pixbuf2, pixbuf2, 0));
    cut_assert_false(gdkcut_pixbuf_equal_content(pixbuf1, pixbuf2, 0));

    g_object_unref(pixbuf2);
    pixbuf2 = load_pixbuf("small-circle.png");
    cut_assert_false(gdkcut_pixbuf_equal_content(pixbuf1, pixbuf2, 0));
}

void
test_equal_content_threshold (void)
{
    pixbuf1 = load_pixbuf("dark-circle.png");
    pixbuf2 = load_pixbuf("max-3-pixel-value-different-dark-circle.png");

    cut_assert_false(gdkcut_pixbuf_equal_content(pixbuf1, pixbuf2, 0));
    cut_assert_true(gdkcut_pixbuf_equal_content(pixbuf1, pixbuf1, 3));
    cut_assert_false(gdkcut_pixbuf_equal_content(pixbuf1, pixbuf2, 2));
}

void
test_inspect (void)
{
    const gchar *expected;
    pixbuf1 = load_pixbuf("dark-circle.png");
    pixbuf2 = load_pixbuf("small-circle-no-alpha.png");

    expected = cut_take_printf("#<GdkPixbuf:%p "
                               "colorspace="
                               "<#<GdkColorspace: rgb(GDK_COLORSPACE_RGB:0)>>, "
                               "n-channels=<4>, "
                               "has-alpha=<TRUE>, "
                               "bits-per-sample=<8>, "
                               "width=<100>, "
                               "height=<100>, "
                               "rowstride=<400>, "
                               "pixels=<((gpointer) %p)>"
                               ">",
                               pixbuf1,
                               gdk_pixbuf_get_pixels(pixbuf1));
    cut_assert_equal_string_with_free(expected,
                                      gcut_object_inspect(G_OBJECT(pixbuf1)));

    expected = cut_take_printf("#<GdkPixbuf:%p "
                               "colorspace="
                               "<#<GdkColorspace: rgb(GDK_COLORSPACE_RGB:0)>>, "
                               "n-channels=<3>, "
                               "has-alpha=<FALSE>, "
                               "bits-per-sample=<8>, "
                               "width=<50>, "
                               "height=<50>, "
                               "rowstride=<152>, "
                               "pixels=<((gpointer) %p)>"
                               ">",
                               pixbuf2,
                               gdk_pixbuf_get_pixels(pixbuf2));
    cut_assert_equal_string_with_free(expected,
                                      gcut_object_inspect(G_OBJECT(pixbuf2)));
}

void
test_diff (void)
{
    pixbuf1 = load_pixbuf("dark-circle.png");
    pixbuf2 = load_pixbuf("nested-circle.png");
    expected_diff = load_pixbuf("diff-dark-and-nested-circle.png");

    actual_diff = gdkcut_pixbuf_diff(pixbuf1, pixbuf2, 0);
    gdkcut_pixbuf_assert_equal(expected_diff, actual_diff, 0);
}

void
test_diff_between_equal_images (void)
{
    pixbuf1 = load_pixbuf("dark-circle.png");

    actual_diff = gdkcut_pixbuf_diff(pixbuf1, pixbuf1, 0);
    gcut_assert_equal_object(NULL, actual_diff);
}

void
test_diff_no_alpha (void)
{
    pixbuf1 = load_pixbuf("dark-circle-no-alpha.png");
    pixbuf2 = load_pixbuf("nested-circle-no-alpha.png");
    expected_diff = load_pixbuf("diff-dark-and-nested-circle-no-alpha.png");

    actual_diff = gdkcut_pixbuf_diff(pixbuf1, pixbuf2, 0);
    gdkcut_pixbuf_assert_equal(expected_diff, actual_diff, 0);
}


/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
