/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-enum-types.h>

void test_equal_string_including_null(void);
void test_inspect_string_including_null(void);
void test_inspect_enum(void);
void test_inspect_flags(void);

static GList *list;
static gchar *inspected;

void
cut_setup (void)
{
    list = NULL;
    inspected = NULL;
}

void
cut_teardown (void)
{
    if (list)
        g_list_free(list);
    if (inspected)
        g_free(inspected);
}

void
test_equal_string_including_null (void)
{
    GList *list1 = NULL, *list2 = NULL;

    list1 = g_list_append(list1, g_strdup("a"));
    list1 = g_list_append(list1, NULL);
    list1 = g_list_append(list1, g_strdup("c"));
    gcut_take_list(list1, g_free);

    cut_assert_true(gcut_list_equal_string(list1, list1));

    list2 = g_list_append(list2, NULL);
    list2 = g_list_append(list2, g_strdup("b"));
    list2 = g_list_append(list2, g_strdup("c"));
    gcut_take_list(list2, g_free);

    cut_assert_false(gcut_list_equal_string(list1, list2));
}

void
test_inspect_string_including_null (void)
{
    GList *list = NULL;

    list = g_list_append(list, g_strdup("a"));
    list = g_list_append(list, NULL);
    list = g_list_append(list, g_strdup("c"));
    gcut_take_list(list, g_free);

    inspected = gcut_list_inspect_string(list);
    cut_assert_equal_string("(\"a\", NULL, \"c\")",
                            inspected);
}

void
test_inspect_enum (void)
{
    list = g_list_append(list, GINT_TO_POINTER(CUT_TEST_RESULT_PENDING));
    list = g_list_append(list, GINT_TO_POINTER(CUT_TEST_RESULT_NOTIFICATION));
    inspected = gcut_list_inspect_enum(CUT_TYPE_TEST_RESULT_STATUS, list);
    cut_assert_equal_string("(#<CutTestResultStatus: "
                            "pending(CUT_TEST_RESULT_PENDING:3)>, "
                            "#<CutTestResultStatus: "
                            "notification(CUT_TEST_RESULT_NOTIFICATION:1)>)",
                            inspected);
}

void
test_inspect_flags (void)
{
    static GType type = 0;
    if (type == 0) {
        static const GFlagsValue values[] = {
            {1 << 0, "CUTTEST_LIST_STUB_FIRST", "first"},
            {1 << 1, "CUTTEST_LIST_STUB_SECOND", "second"},
            {1 << 2, "CUTTEST_LIST_STUB_THIRD", "third"},
            {0, NULL, NULL}
        };
        type = g_flags_register_static("CuttestListStubFlags", values);
    }

    list = g_list_append(list, GUINT_TO_POINTER(1 << 0 | 1 << 1));
    list = g_list_append(list, GUINT_TO_POINTER(1 << 2));
    inspected = gcut_list_inspect_flags(type, list);
    cut_assert_equal_string("(#<CuttestListStubFlags: "
                            "first|second "
                            "(CUTTEST_LIST_STUB_FIRST:0x1)|"
                            "(CUTTEST_LIST_STUB_SECOND:0x2)>, "
                            "#<CuttestListStubFlags: "
                            "third "
                            "(CUTTEST_LIST_STUB_THIRD:0x4)>)",
                            inspected);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
