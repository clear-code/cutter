/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void test_string (void);
void test_gtype (void);
void test_flags (void);
void test_enum (void);

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

typedef void (*GetValueFunc) (GCutData *data,
                              const gchar *field_name,
                              GError **error);

#define assert_nonexistent_field(get_value_func)                        \
    cut_trace(assert_nonexistent_field_helper((GetValueFunc)get_value_func))

static void
assert_nonexistent_field_helper (GetValueFunc get_value_func)
{
    expected_error = g_error_new(GCUT_DATA_ERROR,
                                 GCUT_DATA_ERROR_NOT_EXIST,
                                 "requested field doesn't exist: <%s>",
                                 "/nonexistent");
    get_value_func(data, "/nonexistent", &actual_error);
    gcut_assert_equal_error(expected_error, actual_error);
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

    assert_nonexistent_field(gcut_data_get_string_with_error);
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

    assert_nonexistent_field(gcut_data_get_gtype_with_error);
}

void
test_flags (void)
{
    GError *error = NULL;
    CuttestFlags value, actual_value;

    value = CUTTEST_FLAG_FIRST | CUTTEST_FLAG_THIRD;
    data = gcut_data_new("/flags", CUTTEST_TYPE_FLAGS, value,
                         NULL);
    actual_value = gcut_data_get_flags_with_error(data, "/flags", &error);
    gcut_assert_error(error);
    gcut_assert_equal_flags(CUTTEST_TYPE_FLAGS, value, actual_value);

    assert_nonexistent_field(gcut_data_get_flags_with_error);
}

void
test_enum (void)
{
    GError *error = NULL;
    CutTestResultStatus value, actual_value;

    value = CUT_TEST_RESULT_SUCCESS;
    data = gcut_data_new("/enum", CUT_TYPE_TEST_RESULT_STATUS, value,
                         NULL);
    actual_value = gcut_data_get_enum_with_error(data, "/enum", &error);
    gcut_assert_error(error);
    gcut_assert_equal_enum(CUT_TYPE_TEST_RESULT_STATUS, value, actual_value);

    assert_nonexistent_field(gcut_data_get_enum_with_error);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
