/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>
#include <gcutter.h>

void test_inspect_condition (void);

static gchar *message;

void
cut_setup (void)
{
    message = NULL;
}

void
cut_teardown (void)
{
    if (message)
        g_free(message);
}

#define cut_assert_equal_inspected_condition(expected, condition)       \
    cut_assert_equal_string_with_free(expected,                         \
                                      gcut_io_inspect_condition(condition))

void
test_inspect_condition (void)
{
    cut_assert_equal_inspected_condition("", 0);
    cut_assert_equal_inspected_condition("In | Out | Priority high | Error "
                                         "| Hung up | Invalid request",
                                         G_IO_IN | G_IO_OUT | G_IO_PRI |
                                         G_IO_ERR | G_IO_HUP | G_IO_NVAL);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
