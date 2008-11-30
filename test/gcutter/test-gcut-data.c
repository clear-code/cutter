/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

void test_string (void);
void test_string_nonexistent (void);
void test_gtype (void);
void test_gtype_nonexistent (void);

static GCutData *data;
static GError *expected_error;
static GError *actual_error;

void
setup (void)
{
    data = NULL;
    expected_error = NULL;
    actual_error = NULL;
}

void
teardown (void)
{
    if (data)
        g_object_unref(data);

    if (expected_error)
        g_error_free(expected_error);
    if (actual_error)
        g_error_free(actual_error);
}

void
test_string (void)
{
    GError *error = NULL;
    const gchar *actual_value;

    data = gcut_data_new("/name", G_TYPE_STRING, "my name",
                         NULL);
    actual_value = gcut_data_get_string_with_error(data, "/name", &error);
    gcut_assert_error(error);
    cut_assert_equal_string("my name", actual_value);
}

void
test_string_nonexistent (void)
{
    data = gcut_data_new("/name", G_TYPE_STRING, "my name",
                         NULL);

    expected_error = g_error_new(GCUT_DATA_ERROR,
                                 GCUT_DATA_ERROR_NOT_EXIST,
                                 "requested field doesn't exist: <%s>",
                                 "/nonexistent");
    gcut_data_get_string_with_error(data, "/nonexistent", &actual_error);
    gcut_assert_equal_error(expected_error, actual_error);

    /* FIXME: write test for gcut_data_get_string(data, "/nonexistent");. */
}

void
test_gtype (void)
{
    GError *error = NULL;
    GType actual_value;

    data = gcut_data_new("/gtype", G_TYPE_GTYPE, GCUT_TYPE_DATA,
                         NULL);
    actual_value = gcut_data_get_gtype_with_error(data, "/gtype", &error);
    gcut_assert_error(error);
    gcut_assert_equal_type(GCUT_TYPE_DATA, actual_value);
}

void
test_gtype_nonexistent (void)
{
    data = gcut_data_new("/gtype", G_TYPE_GTYPE, GCUT_TYPE_DATA,
                         NULL);

    expected_error = g_error_new(GCUT_DATA_ERROR,
                                 GCUT_DATA_ERROR_NOT_EXIST,
                                 "requested field doesn't exist: <%s>",
                                 "/nonexistent");
    gcut_data_get_gtype_with_error(data, "/nonexistent", &actual_error);
    gcut_assert_equal_error(expected_error, actual_error);

    /* FIXME: write test for gcut_data_get_gtype(data, "/nonexistent");. */
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
