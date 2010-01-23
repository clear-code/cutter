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

void test_list_string (void);
void test_list_string_array (void);
void test_list_new (void);
void test_take_new_list_string (void);
void test_take_new_list_string_array (void);
void test_take_new_list_object (void);
void test_data_get (void);

static GList *string_list;
static GList *object_list;
static GCutDynamicData *data;

void
cut_setup (void)
{
    string_list = NULL;
    object_list = NULL;
    data = NULL;
}

void
cut_teardown (void)
{
    if (string_list)
        gcut_list_string_free(string_list);

    if (object_list)
        gcut_list_object_free(object_list);

    if (data)
        g_object_unref(data);
}


void
test_list_string (void)
{
    GList *expected = NULL;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    string_list = gcut_list_string_new("a", "zzz", "123", NULL);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), string_list);
}

void
test_list_string_array (void)
{
    const gchar *strings[] = {"a", "zzz", "123", NULL};
    GList *expected = NULL;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    string_list = gcut_list_string_new_array(strings);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), string_list);
}

void
test_list_new (void)
{
    GList *expected = NULL;
    GCutDynamicData *string_data, *int_data, *enum_data;

    string_data = gcut_dynamic_data_new("string", G_TYPE_STRING, NULL,
                                        NULL);
    int_data = gcut_dynamic_data_new("int", G_TYPE_INT, 0,
                                     NULL);
    enum_data = gcut_dynamic_data_new("enum",
                                      CUT_TYPE_TEST_RESULT_STATUS,
                                      CUT_TEST_RESULT_SUCCESS,
                                      NULL);

    expected = g_list_append(expected, string_data);
    expected = g_list_append(expected, int_data);
    expected = g_list_append(expected, enum_data);

    object_list = gcut_list_new(string_data, int_data, enum_data, NULL);
    gcut_assert_equal_list_object(gcut_take_list(expected, NULL),
                                  object_list);
}

void
test_take_new_list_string (void)
{
    GList *expected = NULL;
    const GList *actual;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    actual = gcut_take_new_list_string("a", "zzz", "123", NULL);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), actual);
}

void
test_take_new_list_string_array (void)
{
    const gchar *strings[] = {"a", "zzz", "123", NULL};
    GList *expected = NULL;
    const GList *actual;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    actual = gcut_take_new_list_string_array(strings);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), actual);
}

void
test_take_new_list_object (void)
{
    GList *expected = NULL;
    const GList *actual;
    GCutDynamicData *string_data, *int_data, *enum_data;

    string_data = gcut_dynamic_data_new("string", G_TYPE_STRING, NULL,
                                        NULL);
    int_data = gcut_dynamic_data_new("int", G_TYPE_INT, 0,
                                     NULL);
    enum_data = gcut_dynamic_data_new("enum",
                                      CUT_TYPE_TEST_RESULT_STATUS,
                                      CUT_TEST_RESULT_SUCCESS,
                                      NULL);

    expected = g_list_append(expected, string_data);
    expected = g_list_append(expected, int_data);
    expected = g_list_append(expected, enum_data);

    actual = gcut_take_new_list_object(string_data, int_data, enum_data, NULL);
    gcut_assert_equal_list_object(gcut_take_list(expected, NULL),
                                  actual);
}

void
test_data_get (void)
{
    gpointer pointer;
    GError *error;

    pointer = malloc(10);
    error = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "not found");
    data = gcut_dynamic_data_new("string", G_TYPE_STRING, "value",
                                 "int", G_TYPE_INT, -29,
                                 "uint", G_TYPE_UINT, 29,
                                 "type", G_TYPE_GTYPE, GCUT_TYPE_DYNAMIC_DATA,
                                 "flags", CUTTEST_TYPE_FLAGS,
                                 CUTTEST_FLAG_FIRST | CUTTEST_FLAG_THIRD,
                                 "enum", CUT_TYPE_TEST_RESULT_STATUS,
                                 CUT_TEST_RESULT_SUCCESS,
                                 "pointer", G_TYPE_POINTER, pointer, g_free,
                                 "boxed", GCUT_TYPE_ERROR, error,
                                 NULL);

    cut_assert_equal_string("value", gcut_data_get_string(data, "string"));
    cut_assert_equal_int(-29, gcut_data_get_int(data, "int"));
    cut_assert_equal_uint(29, gcut_data_get_uint(data, "uint"));
    gcut_assert_equal_type(GCUT_TYPE_DYNAMIC_DATA,
                           gcut_data_get_type(data, "type"));
    gcut_assert_equal_flags(CUTTEST_TYPE_FLAGS,
                            CUTTEST_FLAG_FIRST | CUTTEST_FLAG_THIRD,
                            gcut_data_get_flags(data, "flags"));
    gcut_assert_equal_enum(CUT_TYPE_TEST_RESULT_STATUS,
                           CUT_TEST_RESULT_SUCCESS,
                           gcut_data_get_enum(data, "enum"));
    cut_assert_equal_pointer(pointer,
                             gcut_data_get_pointer(data, "pointer"));
    gcut_assert_equal_error(error,
                            gcut_data_get_boxed(data, "boxed"));
}
