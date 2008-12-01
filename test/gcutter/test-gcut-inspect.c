/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void test_direct (void);
void test_int (void);
void test_string (void);
void test_type (void);
void test_flags (void);
void test_enum (void);

static GString *string;

void
setup (void)
{
    string = g_string_new(NULL);
}

void
teardown (void)
{
    if (string)
        g_string_free(string, TRUE);
}

void
test_direct (void)
{
    gcut_inspect_direct(string, GUINT_TO_POINTER(100), NULL);
    cut_assert_equal_string("100", string->str);
}

void
test_int (void)
{
    gint value = 99;

    gcut_inspect_int(string, &value, NULL);
    cut_assert_equal_string("99", string->str);
}

void
test_string (void)
{
    gcut_inspect_string(string, "XXX", NULL);
    cut_assert_equal_string("\"XXX\"", string->str);
}

void
test_type (void)
{
    GType type;

    type = GCUT_TYPE_DATA;
    gcut_inspect_type(string, &type, NULL);
    cut_assert_equal_string("<GCutData>", string->str);
}

void
test_flags (void)
{
    CuttestFlags flags;
    GType flags_type;

    flags = CUTTEST_FLAG_FIRST | CUTTEST_FLAG_SECOND;
    flags_type = CUTTEST_TYPE_FLAGS;
    gcut_inspect_flags(string, &flags, &flags_type);
    cut_assert_equal_string("#<CuttestFlags: first|second "
                            "(CUTTEST_FLAG_FIRST:0x1)|"
                            "(CUTTEST_FLAG_SECOND:0x2)>",
                            string->str);
}

void
test_enum (void)
{
    CutTestResultStatus value;
    GType enum_type;

    value = CUT_TEST_RESULT_SUCCESS;
    enum_type = CUT_TYPE_TEST_RESULT_STATUS;
    gcut_inspect_enum(string, &value, &enum_type);
    cut_assert_equal_string(cut_take_printf("#<CutTestResultStatus: "
                                            "success"
                                            "(CUT_TEST_RESULT_SUCCESS:%d)>",
                                            CUT_TEST_RESULT_SUCCESS),
                            string->str);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
