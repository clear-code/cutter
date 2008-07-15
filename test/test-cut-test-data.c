#include <gcutter.h>
#include <cutter/cut-test-data.h>
#include "lib/cuttest-utils.h"

void test_new (void);
void test_new_empty (void);
void test_new_and_destroy (void);
void test_set_name (void);
void test_set_value (void);
void test_to_xml (void);
void test_to_xml_empty (void);
void test_to_xml_string (void);

static CutTestData *test_data;
static gboolean destroy_called;
static gchar *destroyed_string;
static GString *string;

void
setup (void)
{
    test_data = NULL;
    destroy_called = FALSE;
    destroyed_string = NULL;
    string = NULL;
}

void
teardown (void)
{
    if (test_data)
        g_object_unref(test_data);

    if (destroyed_string)
        g_free(destroyed_string);

    if (string)
        g_string_free(string, TRUE);
}

void
test_new (void)
{
    const gchar name[] = "sample test data";
    gchar value[] = "sample test value";

    test_data = cut_test_data_new(name, value, NULL);
    cut_assert_equal_string(name, cut_test_data_get_name(test_data));
    cut_assert_equal_string(value, cut_test_data_get_value(test_data));
}

void
test_new_empty (void)
{
    test_data = cut_test_data_new_empty();
    cut_assert_equal_string(NULL, cut_test_data_get_name(test_data));
    cut_assert_equal_string(NULL, cut_test_data_get_value(test_data));
}

static void
string_data_free (gpointer data)
{
    destroy_called = TRUE;
    destroyed_string = data;
}

void
test_new_and_destroy (void)
{
    const gchar name[] = "sample test data";
    const gchar value[] = "sample test value";

    test_data = cut_test_data_new(name, g_strdup(value), string_data_free);
    cut_assert_false(destroy_called);
    cut_assert_equal_string(NULL, destroyed_string);
    g_object_unref(test_data);
    test_data = NULL;
    cut_assert_true(destroy_called);
    cut_assert_equal_string(value, destroyed_string);
}

void
test_set_name (void)
{
    const gchar name[] = "sample test data";
    const gchar changed_name[] = "changed name";

    test_data = cut_test_data_new(name, NULL, NULL);
    cut_assert_equal_string(name, cut_test_data_get_name(test_data));

    cut_test_data_set_name(test_data, changed_name);
    cut_assert_equal_string(changed_name, cut_test_data_get_name(test_data));
}

void
test_to_xml (void)
{
    const gchar name[] = "sample test data";
    const gchar value[] = "sample test value";
    const gchar expected[] =
        "<test-data>\n"
        "  <name>sample test data</name>\n"
        "</test-data>\n";

    test_data = cut_test_data_new(name, g_strdup(value), g_free);
    cut_assert_equal_string_with_free(expected, cut_test_data_to_xml(test_data));
}

void
test_to_xml_empty (void)
{
    const gchar expected[] =
        "<test-data>\n"
        "</test-data>\n";

    test_data = cut_test_data_new_empty();
    cut_assert_equal_string_with_free(expected, cut_test_data_to_xml(test_data));
}

void
test_to_xml_string (void)
{
    const gchar name[] = "sample test data";
    const gchar value[] = "sample test value";
    const gchar expected[] =
        "before content\n"
        "  <test-data>\n"
        "    <name>sample test data</name>\n"
        "  </test-data>\n";

    test_data = cut_test_data_new(name, g_strdup(value), g_free);
    string = g_string_new("before content\n");
    cut_test_data_to_xml_string(test_data, string, 2);
    cut_assert_equal_string(expected, string->str);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
