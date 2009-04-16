/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void test_string (void);
void test_int (void);
void test_uint (void);
void test_data_type (void);
void test_flags (void);
void test_enum (void);
void test_pointer (void);
void test_boxed (void);

static GCutDynamicData *data;
static GError *expected_error;
static GError *actual_error;

void
cut_setup (void)
{
    data = NULL;
    expected_error = NULL;
    actual_error = NULL;
}

void
cut_teardown (void)
{
    if (data)
        g_object_unref(data);

    if (expected_error)
        g_error_free(expected_error);
    if (actual_error)
        g_error_free(actual_error);
}

typedef void (*GetValueFunc) (GCutDynamicData *data,
                              const gchar *field_name,
                              GError **error);

#define assert_nonexistent_field(get_value_func)                        \
    cut_trace(assert_nonexistent_field_helper((GetValueFunc)get_value_func))

static void
assert_nonexistent_field_helper (GetValueFunc get_value_func)
{
    expected_error = g_error_new(GCUT_DYNAMIC_DATA_ERROR,
                                 GCUT_DYNAMIC_DATA_ERROR_NOT_EXIST,
                                 "requested field doesn't exist: <%s>",
                                 "nonexistent");
    get_value_func(data, "nonexistent", &actual_error);
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_string (void)
{
    GError *error = NULL;
    const gchar *actual_value;

    data = gcut_dynamic_data_new("string", G_TYPE_STRING, "value",
                                 NULL);
    actual_value = gcut_dynamic_data_get_string(data, "string", &error);
    gcut_assert_error(error);
    cut_assert_equal_string("value", actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_string);
}

void
test_int (void)
{
    GError *error = NULL;
    gint actual_value;

    data = gcut_dynamic_data_new("int", G_TYPE_INT, -29,
                                 NULL);
    actual_value = gcut_dynamic_data_get_uint(data, "int", &error);
    gcut_assert_error(error);
    cut_assert_equal_uint(-29, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_int);
}

void
test_uint (void)
{
    GError *error = NULL;
    guint actual_value;

    data = gcut_dynamic_data_new("uint", G_TYPE_UINT, 29,
                                 NULL);
    actual_value = gcut_dynamic_data_get_uint(data, "uint", &error);
    gcut_assert_error(error);
    cut_assert_equal_uint(29, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_uint);
}

void
test_data_type (void)
{
    GError *error = NULL;
    GType actual_value;

    data = gcut_dynamic_data_new("type", G_TYPE_GTYPE, GCUT_TYPE_DYNAMIC_DATA,
                                 NULL);
    actual_value = gcut_dynamic_data_get_data_type(data, "type", &error);
    gcut_assert_error(error);
    gcut_assert_equal_type(GCUT_TYPE_DYNAMIC_DATA, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_data_type);
}

void
test_flags (void)
{
    GError *error = NULL;
    CuttestFlags value, actual_value;

    value = CUTTEST_FLAG_FIRST | CUTTEST_FLAG_THIRD;
    data = gcut_dynamic_data_new("flags", CUTTEST_TYPE_FLAGS, value,
                                 NULL);
    actual_value = gcut_dynamic_data_get_flags(data, "flags", &error);
    gcut_assert_error(error);
    gcut_assert_equal_flags(CUTTEST_TYPE_FLAGS, value, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_flags);
}

void
test_enum (void)
{
    GError *error = NULL;
    CutTestResultStatus value, actual_value;

    value = CUT_TEST_RESULT_SUCCESS;
    data = gcut_dynamic_data_new("enum", CUT_TYPE_TEST_RESULT_STATUS, value,
                                 NULL);
    actual_value = gcut_dynamic_data_get_enum(data, "enum", &error);
    gcut_assert_error(error);
    gcut_assert_equal_enum(CUT_TYPE_TEST_RESULT_STATUS, value, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_enum);
}

void
test_pointer (void)
{
    GError *error = NULL;
    gpointer value;
    gconstpointer actual_value;

    value = malloc(10);
    data = gcut_dynamic_data_new("pointer", G_TYPE_POINTER, value, g_free,
                                 NULL);
    actual_value = gcut_dynamic_data_get_pointer(data, "pointer", &error);
    gcut_assert_error(error);
    cut_assert_equal_pointer(value, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_pointer);
}

void
test_boxed (void)
{
    GError *error = NULL;
    GError *value;
    const GError *actual_value;

    value = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "not found");
    data = gcut_dynamic_data_new("error", GCUT_TYPE_ERROR, value,
                                 NULL);
    actual_value = gcut_dynamic_data_get_boxed(data, "error", &error);
    gcut_assert_error(error);
    gcut_assert_equal_error(value, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_boxed);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
