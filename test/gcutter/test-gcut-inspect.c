/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

void test_direct (void);
void test_int (void);
void test_string (void);
void test_gtype (void);

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
test_gtype (void)
{
    GType type;

    type = GCUT_TYPE_DATA;
    gcut_inspect_gtype(string, &type, NULL);
    cut_assert_equal_string("<GCutData>", string->str);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
