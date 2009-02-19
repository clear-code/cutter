/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void test_list_string (void);
void test_list_string_array (void);
void test_take_new_list_string (void);
void test_take_new_list_string_array (void);
void test_data_get (void);

static GList *list;
static GCutDynamicData *data;

void
setup (void)
{
    list = NULL;
    data = NULL;
}

void
teardown (void)
{
    if (list)
        gcut_list_string_free(list);

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

    list = gcut_list_string_new("a", "zzz", "123", NULL);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), list);
}

void
test_list_string_array (void)
{
    const gchar *strings[] = {"a", "zzz", "123", NULL};
    GList *expected = NULL;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    list = gcut_list_string_new_array(strings);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), list);
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
test_data_get (void)
{
    gpointer pointer;

    pointer = malloc(10);
    data = gcut_dynamic_data_new("string", G_TYPE_STRING, "value",
                                 "int", G_TYPE_INT, -29,
                                 "uint", G_TYPE_UINT, 29,
                                 "type", G_TYPE_GTYPE, GCUT_TYPE_DYNAMIC_DATA,
                                 "flags", CUTTEST_TYPE_FLAGS,
                                 CUTTEST_FLAG_FIRST | CUTTEST_FLAG_THIRD,
                                 "enum", CUT_TYPE_TEST_RESULT_STATUS,
                                 CUT_TEST_RESULT_SUCCESS,
                                 "pointer", G_TYPE_POINTER, pointer, g_free,
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
}
