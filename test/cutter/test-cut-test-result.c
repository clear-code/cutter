#include <gcutter.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-backtrace-entry.h>

void test_get_status(void);
void test_get_message_full(void);
void test_get_message_only_user(void);
void test_get_message_only_system(void);
void test_get_message_none(void);
void test_get_message_empty(void);
void test_set_messages(void);
void test_set_messages_empty(void);
void test_get_backtrace(void);
void test_status_to_signal_name(void);
void test_status_is_critical(void);
void test_get_test_suite(void);
void test_get_diff(void);
void test_set_diff(void);
void test_to_xml_empty(void);
void test_to_xml_empty_failure(void);
void test_new_from_xml(void);
void test_new_from_xml_with_invalid_top_level_tag_name(void);
void test_new_from_xml_with_invalid_line(void);
void test_new_from_xml_with_unexpected_name_tag(void);
void test_new_from_xml_with_unexpected_value_tag(void);
void test_new_from_xml_without_option_name(void);
void test_new_from_xml_without_option_value(void);
void test_new_from_xml_with_multiple_option_names(void);
void test_new_from_xml_with_invalid_status(void);
void test_new_from_xml_with_invalid_elapsed(void);

static CutTestResult *result;
static CutTestSuite *suite;
static GError *error;
static GList *backtrace;

void
cut_setup (void)
{
    result = NULL;
    suite = NULL;
    error = NULL;

    backtrace = g_list_prepend(NULL,
                               cut_backtrace_entry_new("file name",
                                                       999,
                                                       "function name",
                                                       "info"));
}

void
cut_teardown (void)
{
    if (result)
        g_object_unref(result);
    if (suite)
        g_object_unref(suite);
    if (error)
        g_error_free(error);

    if (backtrace) {
        g_list_foreach(backtrace, (GFunc)g_object_unref, NULL);
        g_list_free(backtrace);
    }
}


void
test_get_status (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 backtrace);
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 backtrace);
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_ERROR,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 backtrace);
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_PENDING,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 backtrace);
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(result));
}

void
test_get_message_full (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 backtrace);
    cut_assert_equal_string("user-message\n"
                            "system-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));

    cut_test_result_set_expected(result, "a\nb\nc");
    cut_test_result_set_actual(result, "a\nB\nc");

    cut_assert_equal_string("user-message\n"
                            "system-message\n"
                            "expected: <a\nb\nc>\n"
                            "  actual: <a\nB\nc>\n"
                            "\n"
                            "diff:\n"
                            "  a\n"
                            "- b\n"
                            "+ B\n"
                            "  c",
                            cut_test_result_get_message(result));
}

void
test_get_message_only_user (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 NULL,
                                 backtrace);
    cut_assert_equal_string("user-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));
    cut_assert(cut_test_result_get_system_message(result) == NULL);
}

void
test_get_message_only_system (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL,
                                 "system-message",
                                 backtrace);
    cut_assert_equal_string("system-message",
                            cut_test_result_get_message(result));
    cut_assert(cut_test_result_get_user_message(result) == NULL);
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));
}

void
test_get_message_none (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 backtrace);
    cut_assert_null(cut_test_result_get_message(result));

    cut_test_result_set_message(result, "message");
    cut_assert_equal_string("message", cut_test_result_get_message(result));
    cut_assert_null(cut_test_result_get_user_message(result));

    cut_test_result_set_user_message(result, "user message");
    cut_assert_equal_string("user message",
                            cut_test_result_get_user_message(result));
    cut_assert_null(cut_test_result_get_system_message(result));

    cut_test_result_set_system_message(result, "system message");
    cut_assert_equal_string("system message",
                            cut_test_result_get_system_message(result));
}

void
test_get_message_empty (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "",
                                 "",
                                 backtrace);
    cut_assert_equal_string(NULL,
                            cut_test_result_get_message(result));
    cut_assert_equal_string(NULL,
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string(NULL,
                            cut_test_result_get_system_message(result));
}

void
test_set_messages (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 backtrace);
    cut_assert_equal_string("user-message\nsystem-message",
                            cut_test_result_get_message(result));

    cut_test_result_set_message(result, "message");
    cut_assert_equal_string("message", cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));

    cut_test_result_set_user_message(result, "user message");
    cut_assert_equal_string("user message",
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));

    cut_test_result_set_system_message(result, "system message");
    cut_assert_equal_string("system message",
                            cut_test_result_get_system_message(result));
}

void
test_set_messages_empty (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user message",
                                 "system message",
                                 backtrace);
    cut_assert_equal_string("user message\nsystem message",
                            cut_test_result_get_message(result));

    cut_test_result_set_user_message(result, "");
    cut_assert_equal_string(NULL,
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string("system message",
                            cut_test_result_get_system_message(result));
    cut_assert_equal_string("system message",
                            cut_test_result_get_message(result));

    cut_test_result_set_system_message(result, "");
    cut_assert_equal_string(NULL,
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string(NULL,
                            cut_test_result_get_system_message(result));
    cut_assert_equal_string(NULL,
                            cut_test_result_get_message(result));
}

void
test_get_backtrace (void)
{
    const GList *actual_backtrace;
    CutBacktraceEntry *entry;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 backtrace);
    actual_backtrace = cut_test_result_get_backtrace(result);
    cut_assert_not_null(actual_backtrace);

    entry = actual_backtrace->data;
    cut_assert_equal_string("file name", cut_backtrace_entry_get_file(entry));
    cut_assert_equal_uint(999, cut_backtrace_entry_get_line(entry));
    cut_assert_equal_string("function name",
                            cut_backtrace_entry_get_function(entry));
    cut_assert_equal_string("info", cut_backtrace_entry_get_info(entry));

    g_object_unref(result);
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL);
    cut_assert_null(cut_test_result_get_backtrace(result));
}

void
test_status_to_signal_name (void)
{
#define STATUS_TO_SIGNAL_NAME(status) \
    cut_test_result_status_to_signal_name(CUT_TEST_RESULT_ ## status)

    cut_assert_equal_string("success", STATUS_TO_SIGNAL_NAME(SUCCESS));
    cut_assert_equal_string("notification", STATUS_TO_SIGNAL_NAME(NOTIFICATION));
    cut_assert_equal_string("omission", STATUS_TO_SIGNAL_NAME(OMISSION));
    cut_assert_equal_string("pending", STATUS_TO_SIGNAL_NAME(PENDING));
    cut_assert_equal_string("failure", STATUS_TO_SIGNAL_NAME(FAILURE));
    cut_assert_equal_string("error", STATUS_TO_SIGNAL_NAME(ERROR));

#undef STATUS_IS_CRITICAL
}

void
test_status_is_critical (void)
{
#define STATUS_IS_CRITICAL(status) \
    cut_test_result_status_is_critical(CUT_TEST_RESULT_ ## status)

    cut_assert(!STATUS_IS_CRITICAL(SUCCESS));
    cut_assert(!STATUS_IS_CRITICAL(NOTIFICATION));
    cut_assert(!STATUS_IS_CRITICAL(OMISSION));
    cut_assert(STATUS_IS_CRITICAL(PENDING));
    cut_assert(STATUS_IS_CRITICAL(FAILURE));
    cut_assert(STATUS_IS_CRITICAL(ERROR));

#undef STATUS_IS_CRITICAL
}

void
test_get_test_suite (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL);
    cut_assert_null(cut_test_result_get_test_suite(result));

    suite = cut_test_suite_new_empty();
    cut_test_result_set_test_suite(result, suite);
    cut_assert_equal_pointer(suite, cut_test_result_get_test_suite(result));

    cut_test_result_set_test_suite(result, NULL);
    cut_assert_null(cut_test_result_get_test_suite(result));
}

void
test_get_diff (void)
{
    result = cut_test_result_new_empty();
    cut_assert_equal_string(NULL, cut_test_result_get_expected(result));
    cut_assert_equal_string(NULL, cut_test_result_get_actual(result));
    cut_assert_equal_string(NULL, cut_test_result_get_diff(result));
    cut_assert_equal_string(NULL, cut_test_result_get_folded_diff(result));

    cut_test_result_set_expected(result, "a\nb\nc");
    cut_assert_equal_string(NULL, cut_test_result_get_diff(result));
    cut_assert_equal_string(NULL, cut_test_result_get_folded_diff(result));

    cut_test_result_set_actual(result, "a\nB\nc");
    cut_assert_equal_string("  a\n"
                            "- b\n"
                            "+ B\n"
                            "  c",
                            cut_test_result_get_diff(result));
    cut_assert_equal_string(NULL, cut_test_result_get_folded_diff(result));

    cut_test_result_set_actual(result, "a\nX\nc");
    cut_assert_equal_string("  a\n"
                            "- b\n"
                            "+ X\n"
                            "  c",
                            cut_test_result_get_diff(result));
    cut_assert_equal_string(NULL, cut_test_result_get_folded_diff(result));
}

void
test_set_diff (void)
{
    const gchar diff[] =
        "  A\n"
        "- B\n"
        "+ b\n"
        "  C";

    result = cut_test_result_new_empty();
    cut_test_result_set_diff(result, diff);

    cut_test_result_set_expected(result, "a\nb\nc");
    cut_assert_equal_string(diff, cut_test_result_get_diff(result));
    cut_assert_equal_string(NULL, cut_test_result_get_folded_diff(result));
    cut_test_result_set_actual(result, "a\nB\nc");
    cut_assert_equal_string(diff, cut_test_result_get_diff(result));
    cut_assert_equal_string(NULL, cut_test_result_get_folded_diff(result));
}

void
test_to_xml_empty (void)
{
    gchar expected[] =
        "<result>\n"
        "  <status>success</status>\n"
        "  <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "  <elapsed>0.000000</elapsed>\n"
        "</result>\n";

    result = cut_test_result_new_empty();
    cut_assert_equal_string_with_free(expected, cut_test_result_to_xml(result));
}

void
test_to_xml_empty_failure (void)
{
    gchar expected[] =
        "<result>\n"
        "  <status>failure</status>\n"
        "  <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "  <elapsed>0.000000</elapsed>\n"
        "</result>\n";

    result = cut_test_result_new_empty();
    cut_test_result_set_status(result, CUT_TEST_RESULT_FAILURE);
    cut_assert_equal_string_with_free(expected, cut_test_result_to_xml(result));
}

void
test_new_from_xml (void)
{
    GError *error = NULL;
    CutTest *test;
    const GList *actual_backtrace;
    CutBacktraceEntry *entry;
    const gchar xml[] =
        "<result>\n"
        "  <test-case>\n"
        "    <name>stub test case</name>\n"
        "  </test-case>\n"
        "  <test>\n"
        "    <name>stub-error-test</name>\n"
        "    <description>Error Test</description>\n"
        "    <option>\n"
        "      <name>bug</name>\n"
        "      <value>1234</value>\n"
        "    </option>\n"
        "  </test>\n"
        "  <status>error</status>\n"
        "  <detail>This test should error</detail>\n"
        "  <backtrace>\n"
        "    <entry>\n"
        "      <file>test-cut-report-xml.c</file>\n"
        "      <line>31</line>\n"
        "      <info>stub_error_test()</info>\n"
        "    </entry>\n"
        "    <entry>\n"
        "      <file>test-cut-test-result.c</file>\n"
        "      <line>29</line>\n"
        "      <info>test_new_from_xml(): additional info</info>\n"
        "    </entry>\n"
        "  </backtrace>\n"
        "  <elapsed>0.000100</elapsed>\n"
        "</result>\n";

    result = cut_test_result_new_from_xml(xml, -1, &error);
    gcut_assert_error(error);
    cut_assert(result);

    test = cut_test_result_get_test(result);
    cut_assert(test);

    cut_assert_equal_string("stub test case",
                            cut_test_result_get_test_case_name(result));
    cut_assert_equal_string("stub-error-test",
                            cut_test_result_get_test_name(result));
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    cut_assert_equal_double(0.0001, 0.0, cut_test_result_get_elapsed(result));

    actual_backtrace = cut_test_result_get_backtrace(result);
    cut_assert_not_null(actual_backtrace);
    entry = actual_backtrace->data;
    cut_assert_equal_string("test-cut-report-xml.c",
                            cut_backtrace_entry_get_file(entry));
    cut_assert_equal_uint(31, cut_backtrace_entry_get_line(entry));
    cut_assert_equal_string("stub_error_test",
                            cut_backtrace_entry_get_function(entry));

    cut_assert_not_null(g_list_next(actual_backtrace));
    entry = g_list_next(actual_backtrace)->data;
    cut_assert_equal_string("test-cut-test-result.c",
                            cut_backtrace_entry_get_file(entry));
    cut_assert_equal_uint(29, cut_backtrace_entry_get_line(entry));
    cut_assert_equal_string("test_new_from_xml",
                            cut_backtrace_entry_get_function(entry));
    cut_assert_equal_string("additional info",
                            cut_backtrace_entry_get_info(entry));

    cut_assert_equal_string("This test should error",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("1234",
                            cut_test_get_attribute(test, "bug"));
    cut_assert_equal_string("Error Test",
                            cut_test_get_description(test));
}

static void
cut_assert_new_from_xml_error_helper(const gchar *expected, const gchar *xml)
{
    result = cut_test_result_new_from_xml(xml, -1, &error);
    cut_assert_null(result);
    cut_assert_not_null(error);
    cut_assert_equal_string(expected, error->message);
}

#define cut_assert_new_from_xml_error(expected, xml)                    \
    cut_trace(cut_assert_new_from_xml_error_helper(expected, xml))

void
test_new_from_xml_with_invalid_top_level_tag_name (void)
{
    const gchar xml[] =
        "<gresult>\n"
        "  <status>success</status>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 2 char 1: "
                                  "/gresult: "
                                  "invalid element",
                                  xml);
}

void
test_new_from_xml_with_invalid_line (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <status>pending</status>\n"
        "  <backtrace>\n"
        "    <entry>\n"
        "      <line>XXX</line>\n"
        "    </entry>\n"
        "  </backtrace>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 5 char 17: "
                                  "/result/backtrace/entry/line: "
                                  "invalid line number: XXX",
                                  xml);
}

void
test_new_from_xml_with_unexpected_name_tag (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <status>success</status>\n"
        "  <name>test name</name>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 3 char 10: "
                                  "/result/name: "
                                  "invalid element",
                                  xml);
}

void
test_new_from_xml_with_unexpected_value_tag (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <value>value</value>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 2 char 11: "
                                  "/result/value: "
                                  "invalid element",
                                  xml);
}

void
test_new_from_xml_without_option_name (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <test>\n"
        "    <option>\n"
        "      <value>value</value>\n"
        "    </option>\n"
        "  </test>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 6 char 1: "
                                  "/result/test/option: "
                                  "option name is not set",
                                  xml);
}

void
test_new_from_xml_without_option_value (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <test>\n"
        "    <option>\n"
        "      <name>name</name>\n"
        "    </option>\n"
        "  </test>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 6 char 1: "
                                  "/result/test/option: "
                                  "option value is not set",
                                  xml);
}

void
test_new_from_xml_with_multiple_option_names (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <test>\n"
        "    <option>\n"
        "      <name>option name1</name>\n"
        "      <name>option name2</name>\n"
        "    </option>\n"
        "  </test>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 5 char 26: "
                                  "/result/test/option/name: "
                                  "multiple option name: option name2",
                                  xml);
}

void
test_new_from_xml_with_invalid_status (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <status>XXXXXX</status>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 2 char 18: "
                                  "/result/status: "
                                  "invalid status: XXXXXX",
                                  xml);
}

void
test_new_from_xml_with_invalid_elapsed (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <elapsed>XXXXXX</elapsed>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 2 char 19: "
                                  "/result/elapsed: "
                                  "invalid elapsed value: XXXXXX",
                                  xml);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
