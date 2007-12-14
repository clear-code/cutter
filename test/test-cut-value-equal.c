#include <cutter.h>
#include <cutter/cut-value-equal.h>

void test_equal (void);

static GValue *value1, *value2;

void
setup (void)
{
    value1 = g_new0(GValue, 1);
    value2 = g_new0(GValue, 1);
}

void
teardown (void)
{
    if (G_VALUE_TYPE(value1))
        g_value_unset(value1);
    g_free(value1);
    if (G_VALUE_TYPE(value2))
        g_value_unset(value2);
    g_free(value2);
}

void
test_equal (void)
{
    g_value_init(value1, G_TYPE_INT);
    g_value_set_int(value1, 100);

    g_value_init(value2, G_TYPE_STRING);
    g_value_set_string(value2, "string");

    cut_assert(cut_value_equal(value1, value1));
    cut_assert(cut_value_equal(value2, value2));

    cut_assert(!cut_value_equal(value1, value2));
}

