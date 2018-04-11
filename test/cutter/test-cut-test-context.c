#include <gcutter.h>

#include <cutter/cut-test-context.h>
#include <cutter/cut-test-suite.h>
#include <cutter/cut-test-iterator.h>
#include <cutter/cut-pipeline.h>

#include "../lib/cuttest-assertions.h"

#ifdef __clang__
#  define FUNCTION(name) "void " name "(void)"
#else
#  define FUNCTION(name) name "()"
#endif

void test_set_data (void);
void test_set_failed (void);
void test_simple_xml (void);
void test_xml_with_test_suite (void);
void test_xml_with_test_case (void);
void test_xml_with_test_iterator (void);
void test_xml_with_test (void);
void test_xml_with_test_data (void);
void test_relative_path (void);

static CutTestContext *context;
static CutTestSuite *test_suite;
static CutTestCase *test_case;
static CutTestIterator *test_iterator;
static CutTest *test;
static CutTestData *test_data;
static CutRunContext *pipeline;

static gboolean destroy_called;
static gchar *destroyed_string;

void
cut_setup (void)
{
    context = cut_test_context_new_empty();
    test_suite = cut_test_suite_new_empty();
    test_case = cut_test_case_new("my-test-case",
                                  NULL, NULL,
                                  NULL, NULL);
    test_iterator = cut_test_iterator_new("my-test-iterator", NULL, NULL);
    test = cut_test_new("my-test", NULL);
    test_data = NULL;
    destroy_called = FALSE;
    destroyed_string = NULL;

    pipeline = NULL;
}

void
cut_teardown (void)
{
    if (context)
        g_object_unref(context);

    g_object_unref(test_suite);
    g_object_unref(test_case);
    g_object_unref(test_iterator);

    if (test_data)
        g_object_unref(test_data);
    if (destroyed_string)
        g_free(destroyed_string);

    if (pipeline)
        g_object_unref(pipeline);
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
    g_object_unref(test_data);
    test_data = NULL;
    cut_assert_false(destroy_called);
    cut_assert_equal_string(NULL, destroyed_string);

    cut_assert_true(cut_test_context_have_data(context));
    current_test_data = cut_test_context_get_current_data(context);
    cut_assert_not_null(current_test_data);
    cut_assert_equal_string(value, cut_test_data_get_value(current_test_data));

    cut_assert_false(destroy_called);
    cut_assert_equal_string(NULL, destroyed_string);
    g_object_unref(context);
    context = NULL;
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
        "  <failed>false</failed>\n"
        "</test-context>\n";
    gchar failed_expected[] =
        "<test-context>\n"
        "  <failed>true</failed>\n"
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
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <failed>false</failed>\n"
        "</test-context>\n";

    cut_test_context_set_test_suite(context, test_suite);
    cut_assert_match_with_free(expected, cut_test_context_to_xml(context));
}

void
test_xml_with_test_iterator (void)
{
    gchar expected[] =
        "<test-context>\n"
        "  <test-suite>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <test-case>\n"
        "    <name>my-test-case</name>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-case>\n"
        "  <test-iterator>\n"
        "    <name>my-test-iterator</name>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-iterator>\n"
        "  <failed>false</failed>\n"
        "</test-context>\n";

    cut_test_context_set_test_suite(context, test_suite);
    cut_test_context_set_test_case(context, test_case);
    cut_test_context_set_test_iterator(context, test_iterator);
    cut_assert_match_with_free(expected, cut_test_context_to_xml(context));
}

void
test_xml_with_test_case (void)
{
    gchar expected[] =
        "<test-context>\n"
        "  <test-suite>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <test-case>\n"
        "    <name>my-test-case</name>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-case>\n"
        "  <failed>false</failed>\n"
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
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <test-case>\n"
        "    <name>my-test-case</name>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-case>\n"
        "  <test>\n"
        "    <name>my-test</name>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test>\n"
        "  <failed>false</failed>\n"
        "</test-context>\n";

    cut_test_context_set_test_suite(context, test_suite);
    cut_test_context_set_test_case(context, test_case);
    cut_test_context_set_test(context, test);
    cut_assert_match_with_free(expected, cut_test_context_to_xml(context));
}

void
test_xml_with_test_data (void)
{
    const gchar test_data_name[] = "sample test data";
    gchar test_data_value[] = "sample test value";
    gchar expected[] =
        "<test-context>\n"
        "  <test-suite>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-suite>\n"
        "  <test-case>\n"
        "    <name>my-test-case</name>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test-case>\n"
        "  <test>\n"
        "    <name>my-test</name>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>.+?</elapsed>\n"
        "  </test>\n"
        "  <test-data>\n"
        "    <name>sample test data</name>\n"
        "  </test-data>\n"
        "  <failed>false</failed>\n"
        "</test-context>\n";

    test_data = cut_test_data_new(test_data_name, test_data_value, NULL);
    cut_test_context_set_test_suite(context, test_suite);
    cut_test_context_set_test_case(context, test_case);
    cut_test_context_set_test(context, test);
    cut_test_context_set_data(context, test_data);
    cut_assert_match_with_free(expected, cut_test_context_to_xml(context));
}

static void
report_error (CutRunContext *context, GError *error, gpointer user_data)
{
    gcut_assert_error(error, cut_message("Pipeline Error"));
}

void
test_relative_path (void)
{
    gchar *test_dir;
    gchar *test_source_path;
    const gchar *assert_location;
    const gchar *test_location;

    pipeline = cut_pipeline_new();
    test_dir = g_build_filename(cuttest_get_base_dir(),
                                "fixtures",
                                "test-context",
                                NULL);
    cut_run_context_set_test_directory(pipeline, test_dir);
    cut_run_context_set_source_directory(pipeline, cuttest_get_base_dir());
    g_free(test_dir);

    g_signal_connect(pipeline, "error", G_CALLBACK(report_error), NULL);
    cut_assert_false(cut_run_context_start(pipeline));

    test_source_path = g_build_filename(cuttest_get_base_dir(),
                                        "fixtures",
                                        "test-context",
                                        "test-relative-path.c",
                                        NULL);
    assert_location = cut_take_printf("%s:10", test_source_path);
    test_location = cut_take_printf("%s:16", test_source_path);
    g_free(test_source_path);
    cut_assert_test_result_with_message(
        pipeline, 0, CUT_TEST_RESULT_FAILURE,
        "test_fail",
        NULL,
        NULL,
        "always fail",
        NULL, NULL,
        assert_location, FUNCTION("cut_assert_always_fail"),
        test_location, FUNCTION("test_fail"),
        NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
