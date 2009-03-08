/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

void test_error(void);

void
test_error (void)
{
    cut_assert_equal_string("GError", g_type_name(GCUT_TYPE_ERROR));
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
