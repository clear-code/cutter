/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-enum-types.h>

void test_inspect_enum(void);
void test_inspect_flags(void);

static gchar *inspected;

void
setup (void)
{
    inspected = NULL;
}

void
teardown (void)
{
    if (inspected)
        g_free(inspected);
}

void
test_inspect_enum (void)
{
    inspected = gcut_enum_inspect(CUT_TYPE_TEST_RESULT_STATUS,
                                  CUT_TEST_RESULT_PENDING);
    cut_assert_equal_string("#<CutTestResultStatus: "
                            "pending(CUT_TEST_RESULT_PENDING:3)>",
                            inspected);

    g_free(inspected);
    inspected = gcut_enum_inspect(CUT_TYPE_TEST_RESULT_STATUS, -100);
    cut_assert_equal_string("#<CutTestResultStatus: -100>", inspected);
}

void
test_inspect_flags (void)
{
    static GType type = 0;
    if (type == 0) {
        static const GFlagsValue values[] = {
            {1 << 0, "CUTTEST_STUB_FIRST", "first"},
            {1 << 1, "CUTTEST_STUB_SECOND", "second"},
            {1 << 2, "CUTTEST_STUB_THIRD", "third"},
            {0, NULL, NULL}
        };
        type = g_flags_register_static("CuttestStubFlags", values);
    }

    inspected = gcut_flags_inspect(type, 0);
    cut_assert_equal_string("#<CuttestStubFlags>", inspected);

    g_free(inspected);
    inspected = gcut_flags_inspect(type, (1 << 0) | (1 << 1));
    cut_assert_equal_string("#<CuttestStubFlags: "
                            "first|second "
                            "(CUTTEST_STUB_FIRST:0x1)|"
                            "(CUTTEST_STUB_SECOND:0x2)>",
                            inspected);

    g_free(inspected);
    inspected = gcut_flags_inspect(type, (1 << 0) | (1 << 3));
    cut_assert_equal_string("#<CuttestStubFlags: "
                            "first "
                            "(CUTTEST_STUB_FIRST:0x1) "
                            "(unknown flags: 0x8)>",
                            inspected);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
