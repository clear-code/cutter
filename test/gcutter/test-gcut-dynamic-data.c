/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <gcutter.h>

#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void test_char (void);
void test_string (void);
void test_int (void);
void test_uint (void);
void test_size (void);
void test_data_type (void);
void test_flags (void);
void test_enum (void);
void test_pointer (void);
void test_boxed (void);
void test_object (void);
void test_boolean (void);
void test_double (void);

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
test_char (void)
{
    GError *error = NULL;
    gchar actual_value;

    data = gcut_dynamic_data_new("char", G_TYPE_CHAR, 'X',
                                 NULL);
    actual_value = gcut_dynamic_data_get_char(data, "char", &error);
    gcut_assert_error(error);
    cut_assert_equal_char('X', actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_char);
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
    actual_value = gcut_dynamic_data_get_int(data, "int", &error);
    gcut_assert_error(error);
    cut_assert_equal_int(-29, actual_value);

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
test_size (void)
{
    GError *error = NULL;
    gsize actual_value;

    data = gcut_dynamic_data_new("size", GCUT_TYPE_SIZE, 29,
                                 NULL);
    actual_value = gcut_dynamic_data_get_size(data, "size", &error);
    gcut_assert_error(error);
    cut_assert_equal_size(29, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_size);
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

void
test_object (void)
{
    GError *error = NULL;
    GCutEgg *value;
    const GError *actual_value;

    value = gcut_egg_new("echo", "Hello", NULL);
    data = gcut_dynamic_data_new("object", GCUT_TYPE_EGG, value,
                                 NULL);
    actual_value = gcut_dynamic_data_get_object(data, "object", &error);
    gcut_assert_error(error);
    gcut_assert_equal_object(value, actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_object);
}

void
test_boolean (void)
{
    GError *error = NULL;
    gboolean actual_value;

    data = gcut_dynamic_data_new("boolean", G_TYPE_BOOLEAN, TRUE,
                                 NULL);
    actual_value = gcut_dynamic_data_get_boolean(data, "boolean", &error);
    gcut_assert_error(error);
    cut_assert_true(actual_value);

    assert_nonexistent_field(gcut_dynamic_data_get_boolean);
}

void
test_double (void)
{
    GError *error = NULL;
    gdouble actual_value;

    data = gcut_dynamic_data_new("double", G_TYPE_DOUBLE, 2.9,
                                 NULL);
    actual_value = gcut_dynamic_data_get_double(data, "double", &error);
    gcut_assert_error(error);
    cut_assert_equal_double(2.9, actual_value, 0.1);

    assert_nonexistent_field(gcut_dynamic_data_get_double);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
