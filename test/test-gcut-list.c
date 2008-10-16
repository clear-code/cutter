/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-enum-types.h>

void test_inspect_enum(void);
void test_inspect_flags(void);

static GList *list;
static gchar *inspected;

void
setup (void)
{
    list = NULL;
    inspected = NULL;
}

void
teardown (void)
{
    if (list)
        g_list_free(list);
    if (inspected)
        g_free(inspected);
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
