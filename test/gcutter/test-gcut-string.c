/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>
#include <gcutter.h>

void test_equal_null_terminated_string (void);

void
cut_setup (void)
{
}

void
cut_teardown (void)
{
}

void
test_equal_null_terminated_string (void)
{
    GString actual = {"0123456789", 10, 0};
    GString expected = {"0123456789", 10, 0};

    gcut_assert_equal_string(&actual, &expected);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
