#include <cutter.h>
#include <cutter/cut-value-equal.h>

void test_equal (void);
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
test_equal (void)
{
    g_value_init(&value1, G_TYPE_INT);
    g_value_set_int(&value1, 100);

    g_value_init(&value2, G_TYPE_STRING);
    g_value_set_string(&value2, "string");

    cut_assert(cut_value_equal(&value1, &value1));
    cut_assert(cut_value_equal(&value2, &value2));

    cut_assert(!cut_value_equal(&value1, &value2));
}

void
test_equal_uninitialized (void)
{
    cut_assert(cut_value_equal(&value1, &value1));
    cut_assert(cut_value_equal(&value2, &value2));

    g_value_init(&value1, G_TYPE_INT);
    g_value_set_int(&value1, 100);

    cut_assert(!cut_value_equal(&value1, &value2));
}

