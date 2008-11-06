#include <gcutter.h>

void test_list_string (void);
void test_list_string_array (void);
void test_take_new_list_string (void);
void test_take_new_list_string_array (void);

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


void
test_list_string (void)
{
    GList *expected = NULL;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    list = gcut_list_string_new("a", "zzz", "123", NULL);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), list);
}

void
test_list_string_array (void)
{
    const gchar *strings[] = {"a", "zzz", "123", NULL};
    GList *expected = NULL;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    list = gcut_list_string_new_array(strings);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), list);
}

void
test_take_new_list_string (void)
{
    GList *expected = NULL;
    const GList *actual;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    actual = gcut_take_new_list_string("a", "zzz", "123", NULL);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), actual);
}

void
test_take_new_list_string_array (void)
{
    const gchar *strings[] = {"a", "zzz", "123", NULL};
    GList *expected = NULL;
    const GList *actual;

    expected = g_list_append(expected, "a");
    expected = g_list_append(expected, "zzz");
    expected = g_list_append(expected, "123");

    actual = gcut_take_new_list_string_array(strings);
    gcut_assert_equal_list_string(gcut_take_list(expected, NULL), actual);
}
