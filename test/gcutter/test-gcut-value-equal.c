/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>

#include <cutter.h>
#include <gcutter/gcut-value-equal.h>

void test_equal_int_string (void);
void test_equal_long (void);
void test_equal_uninitialized (void);

static GValue value1, value2;

void
setup (void)
{
    memset(&value1, 0, sizeof(GValue));
    memset(&value2, 0, sizeof(GValue));
}

void
teardown (void)
{
    if (G_VALUE_TYPE(&value1))
        g_value_unset(&value1);
    if (G_VALUE_TYPE(&value2))
        g_value_unset(&value2);
}

void
test_equal_int_string (void)
{
    g_value_init(&value1, G_TYPE_INT);
    g_value_set_int(&value1, 100);

    g_value_init(&value2, G_TYPE_STRING);
    g_value_set_string(&value2, "string");

    cut_assert_true(gcut_value_equal(&value1, &value1));
    cut_assert_true(gcut_value_equal(&value2, &value2));

    cut_assert_false(gcut_value_equal(&value1, &value2));
}

void
test_equal_uninitialized (void)
{
    cut_assert_true(gcut_value_equal(&value1, &value1));
    cut_assert_true(gcut_value_equal(&value2, &value2));

    g_value_init(&value1, G_TYPE_INT);
    g_value_set_int(&value1, 100);

    cut_assert_false(gcut_value_equal(&value1, &value2));
}

void
test_equal_long (void)
{
    g_value_init(&value1, G_TYPE_LONG);
    g_value_set_long(&value1, 100);

    g_value_init(&value2, G_TYPE_LONG);
    g_value_set_long(&value2, 200);

    cut_assert_true(gcut_value_equal(&value1, &value1));
    cut_assert_true(gcut_value_equal(&value2, &value2));

    cut_assert_false(gcut_value_equal(&value1, &value2));

    g_value_set_long(&value2, 100);
    cut_assert_true(gcut_value_equal(&value1, &value2));
    cut_assert_true(gcut_value_equal(&value2, &value1));
}
