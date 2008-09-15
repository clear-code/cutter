/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gdkcutter-pixbuf.h>
#include "lib/cuttest-utils.h"

void test_equal_property(void);
void test_equal_content(void);
void test_equal_content_threshold(void);
void test_inspect_property(void);

static GdkPixbuf *pixbuf1, *pixbuf2;

void
setup (void)
{
    pixbuf1 = NULL;
    pixbuf2 = NULL;

    cut_set_fixture_data_dir(cuttest_get_base_dir(),
                             "fixtures",
                             "pixbuf",
                             NULL);
}

void
teardown (void)
{
    if (pixbuf1)
        g_object_unref(pixbuf1);
    if (pixbuf2)
        g_object_unref(pixbuf2);
}

static GdkPixbuf *
load_pixbuf (const gchar *path)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    gchar *pixbuf_path;

    pixbuf_path = cut_build_fixture_data_path(path);
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
test_inspect_property (void)
{
    pixbuf1 = load_pixbuf("dark-circle.png");
    pixbuf2 = load_pixbuf("no-alpha-small-circle.png");

    cut_assert_equal_string_with_free("#<GdkPixbuf: "
                                      "n-channels: 4; "
                                      "has-alpha: TRUE; "
                                      "bits-per-sample: 8; "
                                      "width: 100; "
                                      "height: 100; "
                                      "row-stride: 400"
                                      ">",
                                      gdkcut_pixbuf_inspect_property(pixbuf1));
    cut_assert_equal_string_with_free("#<GdkPixbuf: "
                                      "n-channels: 3; "
                                      "has-alpha: FALSE; "
                                      "bits-per-sample: 8; "
                                      "width: 50; "
                                      "height: 50; "
                                      "row-stride: 152"
                                      ">",
                                      gdkcut_pixbuf_inspect_property(pixbuf2));
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
