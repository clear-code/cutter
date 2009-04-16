/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-utils.h>

void test_take_replace (void);

void
cut_setup (void)
{
}

void
cut_teardown (void)
{
}

void
test_take_replace (void)
{
    cut_assert_equal_string("aaa BBB ccc",
                            cut_take_replace("aaa bbb ccc", "bbb", "BBB"));
    cut_assert_equal_string("Xaa\nXbb\nXcc",
                            cut_take_replace("aaa\nbbb\nccc", "^.", "X"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
