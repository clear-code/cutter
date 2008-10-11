/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-loader.h>
#include <cutter/cut-test.h>

void test_equal (void);
void test_inspect (void);
void test_inspect_null (void);
void test_inspect_custom (void);

static GObject *object;
static GObject *object1, *object2;
static gchar *inspected;

void
setup (void)
{
    object = NULL;
    object1 = NULL;
    object2 = NULL;
    inspected = NULL;
}

void
teardown (void)
{
    if (object)
        g_object_unref(object);

    if (object1)
        g_object_unref(object1);
    if (object2)
        g_object_unref(object2);

    if (inspected)
        g_free(inspected);
}

static gboolean
name_equal (gconstpointer object1, gconstpointer object2)
{
    return g_str_equal(cut_test_get_name(CUT_TEST(object1)),
                       cut_test_get_name(CUT_TEST(object2)));
}

static gboolean
always_false (gconstpointer object1, gconstpointer object2)
{
    return FALSE;
}

void
test_equal (void)
{
    object1 = G_OBJECT(cut_test_new("test", NULL));
    object2 = G_OBJECT(cut_test_new("test", NULL));

    cut_assert_true(gcut_object_equal(NULL, NULL, NULL));
    cut_assert_true(gcut_object_equal(object1, object1, NULL));
    cut_assert_true(gcut_object_equal(object2, object2, NULL));
    cut_assert_false(gcut_object_equal(object1, NULL, NULL));
    cut_assert_false(gcut_object_equal(NULL, object2, NULL));
    cut_assert_true(gcut_object_equal(object1, object2, name_equal));
    cut_assert_false(gcut_object_equal(object1, object2, always_false));
}

void
test_inspect (void)
{
    object = G_OBJECT(cut_loader_new("so-file"));
    inspected = gcut_object_inspect(object);
    cut_assert_equal_string(cut_take_printf("#<CutLoader:%p "
                                            "so-filename=<\"so-file\">, "
                                            "base-directory=<NULL>>",
                                            object),
                            inspected);
}

void
test_inspect_null (void)
{
    inspected = gcut_object_inspect(NULL);
    cut_assert_equal_string("NULL", inspected);
}

static void
custom_inspect (GString *string, gconstpointer data, gpointer user_data)
{
    gchar *message = user_data;

    g_string_append_printf(string, "%s: %s",
                           G_OBJECT_TYPE_NAME(data), message);
}

void
test_inspect_custom (void)
{
    object = G_OBJECT(cut_loader_new("so-file"));
    inspected = gcut_object_inspect_custom(object, custom_inspect, "user-data");
    cut_assert_equal_string("CutLoader: user-data", inspected);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
