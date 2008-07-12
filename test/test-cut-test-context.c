#include "cutter.h"
#include <cutter/cut-test-context.h>
#include <cutter/cut-test-suite.h>

void test_set_data (void);
void test_set_failed (void);
void test_simple_xml (void);
void test_xml_with_test_suite (void);
void test_xml_with_test_case (void);
void test_xml_with_test (void);

static CutTestContext *context;
static CutTestSuite *test_suite;
static CutTestCase *test_case;
static CutTest *test;
static CutTestData *test_data;

static gboolean destroy_called;
static gchar *destroyed_string;

void
setup (void)
{
    context = cut_test_context_new(NULL, NULL, NULL);
    test_suite = cut_test_suite_new_empty();
    test_case = cut_test_case_new("my-test-case",
                                  NULL, NULL,
                                  NULL, NULL,
                                  NULL,NULL);
    test = cut_test_new("my-test", NULL);
    test_data = NULL;
    destroy_called = FALSE;
    destroyed_string = NULL;
}

void
teardown (void)
{
    if (context)
        g_object_unref(context);

    g_object_unref(test_suite);
    g_object_unref(test_case);

    if (test_data)
        g_object_unref(test_data);
    if (destroyed_string)
        g_free(destroyed_string);
}

static void
string_data_free (gpointer data)
{
    destroy_called = TRUE;
    destroyed_string = data;
}

void
test_set_data (void)
{
    CutTestData *current_test_data;
    const gchar name[] = "sample test data";
    const gchar value[] = "sample test value";

    cut_assert_false(cut_test_context_have_data(context));

    test_data = cut_test_data_new(name, g_strdup(value), string_data_free);
    cut_test_context_set_data(context, test_data);

    cut_assert_true(cut_test_context_have_data(context));
    current_test_data = cut_test_context_get_current_data(context);
    cut_assert_not_null(current_test_data);
    cut_assert_equal_string(value, cut_test_data_get_value(current_test_data));

    cut_assert_false(destroy_called);
    cut_assert_equal_string(NULL, destroyed_string);
    g_object_unref(context);
    context = NULL;
    cut_assert_false(destroy_called);
    cut_assert_equal_string(NULL, destroyed_string);

    g_object_unref(test_data);
    test_data = NULL;
    cut_assert_true(destroy_called);
    cut_assert_equal_string(value, destroyed_string);
}

void
test_set_failed (void)
{
    cut_assert_false(cut_test_context_is_failed(context));

    cut_test_context_set_failed(context, TRUE);
    cut_assert_true(cut_test_context_is_failed(context));

    cut_test_context_set_failed(context, FALSE);
    cut_assert_false(cut_test_context_is_failed(context));
}

void
test_simple_xml (void)
{
    gchar success_expected[] =
        "<test-context>\n"
        "  <failed>FALSE</failed>\n"
        "</test-context>\n";
    gchar failed_expected[] =
        "<test-context>\n"
        "  <failed>TRUE</failed>\n"
        "</test-context>\n";

    cut_assert_false(cut_test_context_is_failed(context));
    cut_assert_equal_string_with_free(success_expected,
                                      cut_test_context_to_xml(context));

    cut_test_context_set_failed(context, TRUE);
    cut_assert_true(cut_test_context_is_failed(context));
    cut_assert_equal_string_with_free(failed_expected,
                                      cut_test_context_to_xml(context));
}

void
test_xml_with_test_suite (void)
{
    gchar expected[] =
        "<test-context>\n"
        "  <test-suite>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <failed>FALSE</failed>\n"
        "</test-context>\n";

    cut_test_context_set_test_suite(context, test_suite);
    cut_assert_match_with_free(expected, cut_test_context_to_xml(context));
}

void
test_xml_with_test_case (void)
{
    gchar expected[] =
        "<test-context>\n"
        "  <test-suite>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <test-case>\n"
        "    <name>my-test-case</name>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-case>\n"
        "  <failed>FALSE</failed>\n"
        "</test-context>\n";

    cut_test_context_set_test_suite(context, test_suite);
    cut_test_context_set_test_case(context, test_case);
    cut_assert_match_with_free(expected, cut_test_context_to_xml(context));
}

void
test_xml_with_test (void)
{
    gchar expected[] =
        "<test-context>\n"
        "  <test-suite>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <test-case>\n"
        "    <name>my-test-case</name>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-case>\n"
        "  <test>\n"
        "    <name>my-test</name>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test>\n"
        "  <failed>FALSE</failed>\n"
        "</test-context>\n";

    cut_test_context_set_test_suite(context, test_suite);
    cut_test_context_set_test_case(context, test_case);
    cut_test_context_set_test(context, test);
    cut_assert_match_with_free(expected, cut_test_context_to_xml(context));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
