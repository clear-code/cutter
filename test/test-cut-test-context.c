#include "cutter.h"
#include <cutter/cut-test-context.h>
#include <cutter/cut-test-suite.h>

void test_user_data (void);
void test_set_failed (void);
void test_simple_xml (void);
void test_xml_with_test_suite (void);
void test_xml_with_test_case (void);
void test_xml_with_test (void);

static CutTestContext *context;
static CutTestSuite *test_suite;
static CutTestCase *test_case;
static CutTest *test;

static guint32 user_data;

void
setup (void)
{
    context = cut_test_context_new(NULL, NULL, NULL);
    test_suite = cut_test_suite_new();
    test_case = cut_test_case_new("my-test-case",
                                  NULL, NULL,
                                  NULL, NULL,
                                  NULL,NULL);
    test = cut_test_new("my-test", NULL);

    user_data = 0;
}

void
teardown (void)
{
    g_object_unref(context);
    g_object_unref(test_suite);
    g_object_unref(test_case);
}

static void
destroy_user_data (gpointer data)
{
    cut_assert_equal_int(user_data,
                         GPOINTER_TO_UINT(data));
}

void
test_user_data (void)
{
    user_data = g_random_int();

    cut_test_context_set_user_data(context,
                                   GUINT_TO_POINTER(user_data), 
                                   destroy_user_data);
    cut_assert_equal_int(user_data,
                         cut_test_context_get_user_data(context));

    cut_test_context_set_user_data(context, 0, NULL);
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
