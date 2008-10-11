/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>

void test_fail_in_deep_path(void);

void
test_fail_in_deep_path (void)
{
    cut_fail("fail in parent/sub/%s", __FILE__);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
