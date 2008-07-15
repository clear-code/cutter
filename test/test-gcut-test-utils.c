#include <gcutter.h>

void test_list_string (void);


static GList *list;

void
setup (void)
{
    list = NULL;
}

void
teardown (void)
{
    if (list)
        gcut_list_string_free(list);
}


void test_list_string (void)
{
    GList *expected = NULL;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    list = gcut_list_string_new("a", "zzz", "123", NULL);

    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), list);
}
