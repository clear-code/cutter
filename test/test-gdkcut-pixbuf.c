/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gdkcutter-pixbuf.h>
#include "lib/cuttest-utils.h"

void test_equal_property(void);

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

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
