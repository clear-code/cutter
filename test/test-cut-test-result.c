#include <gcutter.h>
#include <cutter/cut-test-result.h>

void test_get_status(void);
void test_get_message_full(void);
void test_get_message_only_user(void);
void test_get_message_only_system(void);
void test_get_message_none(void);
void test_set_messages(void);
void test_get_function_name(void);
void test_get_filename(void);
void test_get_line(void);
void test_status_to_signal_name(void);
void test_status_is_critical(void);
void test_get_test_suite(void);
void test_to_xml_empty(void);
void test_to_xml_empty_failure(void);
void test_new_from_xml(void);
void test_new_from_xml_with_invalid_top_level_tag_name(void);
void test_new_from_xml_with_invalid_line(void);
void test_new_from_xml_with_invalid_info(void);
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

void
setup (void)
{
    result = NULL;
    suite = NULL;
    error = NULL;
}

void
teardown (void)
{
    if (result)
        g_object_unref(result);
    if (suite)
        g_object_unref(suite);
    if (error)
        g_error_free(error);
}


void
test_get_status (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_ERROR,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_PENDING,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
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
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("user-message\nsystem-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));
}

void
test_get_message_only_user (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 NULL,
                                 "function-name",
                                 "filename",
                                 999);
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
                                 "function-name",
                                 "filename",
                                 999);
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
                                 "function-name",
                                 "filename",
                                 999);
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
test_set_messages (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
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
test_get_function_name (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("function-name",
                            cut_test_result_get_function_name(result));
    cut_test_result_set_function_name(result, "another-function-name");
    cut_assert_equal_string("another-function-name",
                            cut_test_result_get_function_name(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 "filename",
                                 999);
    cut_assert(cut_test_result_get_function_name(result) == NULL,
               "cut_test_result_get_function_name() should return NULL!");
}

void
test_get_filename (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("filename", cut_test_result_get_filename(result));

    cut_test_result_set_filename(result, "another-filename");
    cut_assert_equal_string("another-filename",
                            cut_test_result_get_filename(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 999);
    cut_assert_null(cut_test_result_get_filename(result));
}

void
test_get_line (void)
{
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(999, cut_test_result_get_line(result));

    cut_test_result_set_line(result, 9999);
    cut_assert_equal_int(9999, cut_test_result_get_line(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 0);
    cut_assert_equal_int(0, cut_test_result_get_line(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 G_MAXUINT);
    cut_assert_equal_int(G_MAXUINT, cut_test_result_get_line(result));
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
                                 NULL, NULL, NULL, NULL, 0);
    cut_assert_null(cut_test_result_get_test_suite(result));

    suite = cut_test_suite_new_empty();
    cut_test_result_set_test_suite(result, suite);
    cut_assert_equal_pointer(suite, cut_test_result_get_test_suite(result));

    cut_test_result_set_test_suite(result, NULL);
    cut_assert_null(cut_test_result_get_test_suite(result));
}

void
test_to_xml_empty (void)
{
    gchar expected[] =
        "<result>\n"
        "  <status>success</status>\n"
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
    cut_assert_equal_int(31, cut_test_result_get_line(result));
    cut_assert_equal_string("test-cut-report-xml.c",
                            cut_test_result_get_filename(result));
    cut_assert_equal_string("stub_error_test",
                            cut_test_result_get_function_name(result));
    cut_assert_equal_string("This test should error",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("1234",
                            cut_test_get_attribute(test, "bug"));
    cut_assert_equal_string("Error Test",
                            cut_test_get_description(test));
}

#define cut_assert_new_from_xml_error(expected, xml) do         \
{                                                               \
    result = cut_test_result_new_from_xml(xml, -1, &error);     \
    cut_assert_null(result);                                    \
    cut_assert_not_null(error);                                 \
    cut_assert_equal_string((expected), error->message);        \
} while (0)

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
test_new_from_xml_with_invalid_info (void)
{
    const gchar xml[] =
        "<result>\n"
        "  <backtrace>\n"
        "    <entry>\n"
        "      <info>stub_error_test</info>\n"
        "    </entry>\n"
        "  </backtrace>\n"
        "</result>\n";

    cut_assert_new_from_xml_error("Error on line 4 char 29: "
                                  "/result/backtrace/entry/info: "
                                  "invalid function name: stub_error_test",
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
