/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>

void test_fail(void);

static void
cut_assert_always_fail (void)
{
    cut_fail("always fail");
}

void
test_fail (void)
{
    cut_trace(cut_assert_always_fail());
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
