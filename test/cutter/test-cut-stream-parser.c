#include <gcutter.h>
#include <cutter/cut-stream-parser.h>
#include <cutter/cut-backtrace-entry.h>
#include "../lib/cuttest-event-receiver.h"

void test_start_run (void);
void test_ready_test_suite (void);
void test_start_test_suite (void);
void test_ready_test_case (void);
void test_start_test_case (void);
void test_ready_test_iterator (void);
void test_start_test_iterator (void);
void test_start_test (void);
void test_start_test_with_multiple_option_names (void);
void test_start_iterated_test (void);
void test_result_error (void);
void test_result_iterated_test (void);
void test_result_test_iterator (void);
void test_result_test_case (void);
void test_pass_assertion_test (void);
void test_pass_assertion_iterated_test (void);
void test_complete_iterated_test (void);
void test_complete_test (void);
void test_complete_test_iterator (void);
void test_complete_test_case (void);
void test_complete_test_suite (void);
void test_complete_run_without_success_tag (void);
void test_complete_run_with_success_true (void);
void test_complete_run_with_success_false (void);
void test_crash_test (void);

static CutStreamParser *parser;
static CutTestResult *result;
static CutRunContext *run_context;
static CuttestEventReceiver *receiver;

void
cut_setup (void)
{
    run_context = cuttest_event_receiver_new();
    receiver = CUTTEST_EVENT_RECEIVER(run_context);
    parser = cut_stream_parser_new(run_context);
    result = NULL;
}

void
cut_teardown (void)
{
    g_object_unref(receiver);
    g_object_unref(parser);

    if (result)
        g_object_unref(result);
}

#define cut_assert_parse(string) do                             \
{                                                               \
    GError *error = NULL;                                       \
    cut_stream_parser_parse(parser, (string), -1, &error);      \
    gcut_assert_error(error);                                   \
} while (0)

#define cut_assert_parse_error(expected_message, string) do     \
{                                                               \
    GError *error = NULL;                                       \
    const GError *taken_error = NULL;                           \
    cut_stream_parser_parse(parser, (string), -1, &error);      \
    cut_assert_not_null(error);                                 \
    taken_error = gcut_take_error(error);                       \
    cut_assert_equal_string((expected_message),                 \
                            taken_error->message);              \
} while (0)

static void
collect_result (CutStreamParser *parser, CutTestResult *test_result,
                gpointer user_data)
{
    CutTestResult **data = (CutTestResult **)user_data;

    g_object_ref(test_result);
    *data = test_result;
}


void
test_start_run (void)
{
    cut_assert_equal_int(0, receiver->n_start_runs);
    cut_assert_true(cut_stream_parser_parse(parser, "<stream", -1, NULL));
    cut_assert_equal_int(0, receiver->n_start_runs);
    cut_assert_true(cut_stream_parser_parse(parser, ">", -1, NULL));
    cut_assert_equal_int(1, receiver->n_start_runs);
}

void
test_ready_test_suite (void)
{
    CuttestReadyTestSuiteInfo *info;
    GTimeVal expected, actual;

    cut_assert_parse("<stream>\n");
    cut_assert_parse("  <ready-test-suite>\n");
    cut_assert_parse("    <test-suite>\n");
    cut_assert_parse("      <start-time>1970-01-01T00:00:00Z</start-time>\n");
    cut_assert_parse("    </test-suite>\n");
    cut_assert_parse("    <n-test-cases>3</n-test-cases>\n");
    cut_assert_parse("    <n-tests>7</n-tests>\n");

    cut_assert_null(receiver->ready_test_suites);
    cut_assert_parse("  </ready-test-suite>");
    cut_assert_equal_int(1, g_list_length(receiver->ready_test_suites));

    info = receiver->ready_test_suites->data;

    cut_assert_not_null(info->test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(info->test_suite)));

    expected.tv_sec = 0;
    expected.tv_usec = 0;
    cut_test_get_start_time(CUT_TEST(info->test_suite), &actual);
    gcut_assert_equal_time_val(&expected, &actual);

    cut_assert_equal_int(3, info->n_test_cases);
    cut_assert_equal_int(7, info->n_tests);
}

void
test_start_test_suite (void)
{
    CutTestSuite *test_suite;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n";
    gchar start_test_suite[] =
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->start_test_suites);

    cut_assert_parse(start_test_suite);
    cut_assert_equal_int(1, g_list_length(receiver->start_test_suites));

    test_suite = receiver->start_test_suites->data;
    cut_assert_not_null(test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(test_suite)));
}

void
test_ready_test_case (void)
{
    CuttestReadyTestCaseInfo *info;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n";
    gchar ready_test_case[] =
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->ready_test_cases);

    cut_assert_parse(ready_test_case);
    cut_assert_equal_int(1, g_list_length(receiver->ready_test_cases));

    info = receiver->ready_test_cases->data;
    cut_assert_not_null(info->test_case);
    cut_assert_equal_string("my test case",
                            cut_test_get_name(CUT_TEST(info->test_case)));
    cut_assert_equal_int(2, info->n_tests);
}

void
test_start_test_case (void)
{
    CutTestCase *test_case;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n";
    gchar start_test_case[] =
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->start_test_cases);

    cut_assert_parse(start_test_case);
    cut_assert_equal_int(1, g_list_length(receiver->start_test_cases));

    test_case = receiver->start_test_cases->data;
    cut_assert_not_null(test_case);
    cut_assert_equal_string("my test case",
                            cut_test_get_name(CUT_TEST(test_case)));
}

void
test_ready_test_iterator (void)
{
    CuttestReadyTestIteratorInfo *info;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n";
    gchar ready_test_iterator[] =
        "  <ready-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "    <n-tests>5</n-tests>\n"
        "  </ready-test-iterator>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->ready_test_iterators);

    cut_assert_parse(ready_test_iterator);
    cut_assert_equal_int(1, g_list_length(receiver->ready_test_iterators));

    info = receiver->ready_test_iterators->data;
    cut_assert_not_null(info->test_iterator);
    cut_assert_equal_string("my test iterator",
                            cut_test_get_name(CUT_TEST(info->test_iterator)));
    cut_assert_equal_uint(5, info->n_tests);
}

void
test_start_test_iterator (void)
{
    CutTestIterator *test_iterator;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <ready-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "    <n-tests>5</n-tests>\n"
        "  </ready-test-iterator>\n";
    gchar start_test_iterator[] =
        "  <start-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "  </start-test-iterator>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->start_test_iterators);

    cut_assert_parse(start_test_iterator);
    cut_assert_equal_int(1, g_list_length(receiver->start_test_iterators));

    test_iterator = receiver->start_test_iterators->data;
    cut_assert_not_null(test_iterator);
    cut_assert_equal_string("my test iterator",
                            cut_test_get_name(CUT_TEST(test_iterator)));
}

void
test_start_test (void)
{
    CuttestStartTestInfo *info;
    CutTestContext *context;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTest *test;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n";
    gchar start_test[] =
        "  <start-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-test>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->start_tests);

    cut_assert_parse(start_test);
    cut_assert_equal_int(1, g_list_length(receiver->start_tests));

    info = receiver->start_tests->data;
    cut_assert_not_null(info);

    cut_assert_not_null(info->test);
    cut_assert_equal_string("my test", cut_test_get_name(CUT_TEST(info->test)));

    context = info->test_context;
    cut_assert_not_null(context);
    cut_assert_false(cut_test_context_is_failed(context));

    test_suite = cut_test_context_get_test_suite(context);
    cut_assert_not_null(test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(test_suite)));

    test_case = cut_test_context_get_test_case(context);
    cut_assert_not_null(test_case);
    cut_assert_equal_string("my test case",
                            cut_test_get_name(CUT_TEST(test_case)));

    test = cut_test_context_get_test(context);
    cut_assert_not_null(test);
    cut_assert_equal_string("my test", cut_test_get_name(test));

    cut_assert_false(cut_test_context_have_data(context));
}

void
test_start_test_with_multiple_option_names (void)
{
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n";
    gchar start_test[] =
        "  <start-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "      <option>\n"
        "        <name>name1</name>\n"
        "        <name>name2</name>\n"
        "        <value>value</value>\n"
        "      </option>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-test>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->start_tests);

    cut_assert_parse_error("Error on line 28 char 21: "
                           "/stream/start-test/test/option/name: "
                           "multiple option name: name2",
                           start_test);
}

void
test_start_iterated_test (void)
{
    CuttestStartIteratedTestInfo *info;
    CutTestData *test_data;
    CutTestContext *context;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTestIterator *test_iterator;
    CutIteratedTest *iterated_test;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <ready-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "    <n-tests>5</n-tests>\n"
        "  </ready-test-iterator>\n"
        "  <start-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "  </start-test-iterator>\n";
    gchar start_iterated_test[] =
        "  <start-iterated-test>\n"
        "    <iterated-test>\n"
        "      <name>my iterated test</name>\n"
        "    </iterated-test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>my test iterator</name>\n"
        "      </test-iterator>\n"
        "      <iterated-test>\n"
        "        <name>my iterated test</name>\n"
        "      </iterated-test>\n"
        "      <test-data>\n"
        "        <name>the first test data</name>\n"
        "      </test-data>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-iterated-test>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->start_iterated_tests);

    cut_assert_parse(start_iterated_test);
    cut_assert_equal_int(1, g_list_length(receiver->start_iterated_tests));

    info = receiver->start_iterated_tests->data;
    cut_assert_not_null(info);

    cut_assert_not_null(info->iterated_test);
    cut_assert_equal_string("my iterated test",
                            cut_test_get_name(CUT_TEST(info->iterated_test)));

    context = info->test_context;
    cut_assert_not_null(context);

    test_suite = cut_test_context_get_test_suite(context);
    cut_assert_not_null(test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(test_suite)));

    test_case = cut_test_context_get_test_case(context);
    cut_assert_not_null(test_case);
    cut_assert_equal_string("my test case",
                            cut_test_get_name(CUT_TEST(test_case)));

    test_iterator = cut_test_context_get_test_iterator(context);
    cut_assert_not_null(test_iterator);
    cut_assert_equal_string("my test iterator",
                            cut_test_get_name(CUT_TEST(test_iterator)));

    iterated_test = CUT_ITERATED_TEST(cut_test_context_get_test(context));
    cut_assert_not_null(iterated_test);
    cut_assert_equal_string("my iterated test",
                            cut_test_get_name(CUT_TEST(iterated_test)));

    cut_assert_false(cut_test_context_is_failed(context));
    cut_assert_true(cut_test_context_have_data(context));

    test_data = cut_test_context_get_current_data(context);
    cut_assert_not_null(test_data);
    cut_assert_equal_string("the first test data",
                            cut_test_data_get_name(test_data));
}

void
test_result_error (void)
{
    GTimeVal expected_start_time, actual_start_time;
    CutTest *test;
    const GList *actual_backtrace;
    CutBacktraceEntry *entry;
    const gchar xml[] =
        "<stream>\n"
        "  <test-result>\n"
        "    <test>\n"
        "      <name>stub-error-test</name>\n"
        "      <description>Error Test</description>\n"
        "      <option>\n"
        "        <name>bug</name>\n"
        "        <value>1234</value>\n"
        "      </option>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>stub test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>stub-error-test</name>\n"
        "        <description>Error Test</description>\n"
        "        <option>\n"
        "          <name>bug</name>\n"
        "          <value>1234</value>\n"
        "        </option>\n"
        "      </test>\n"
        "      <failed>TRUE</failed>\n"
        "    </test-context>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>stub test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>stub-error-test</name>\n"
        "        <description>Error Test</description>\n"
        "        <option>\n"
        "          <name>bug</name>\n"
        "          <value>1234</value>\n"
        "        </option>\n"
        "      </test>\n"
        "      <status>error</status>\n"
        "      <detail>This test should error</detail>\n"
        "      <backtrace>\n"
        "        <entry>\n"
        "          <file>test-cut-report-xml.c</file>\n"
        "          <line>31</line>\n"
        "          <info>stub_error_test()</info>\n"
        "        </entry>\n"
        "      </backtrace>\n"
        "      <start-time>2008-07-29T05:16:40Z</start-time>\n"
        "      <elapsed>0.000100</elapsed>\n"
        "    </result>\n"
        "  </test-result>\n"
        "</stream>\n";

    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_parse(xml);
    cut_assert_not_null(result);

    test = cut_test_result_get_test(result);
    cut_assert(test);

    cut_assert_equal_string("stub test case",
                            cut_test_result_get_test_case_name(result));
    cut_assert_equal_string("stub-error-test",
                            cut_test_result_get_test_name(result));
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));

    expected_start_time.tv_sec = 1217308600;
    expected_start_time.tv_usec = 0;
    cut_test_result_get_start_time(result, &actual_start_time);
    gcut_assert_equal_time_val(&expected_start_time, &actual_start_time);
    cut_assert_equal_double(0.0001, 0.0, cut_test_result_get_elapsed(result));

    actual_backtrace = cut_test_result_get_backtrace(result);
    cut_assert_not_null(actual_backtrace);
    entry = actual_backtrace->data;
    cut_assert_equal_string("test-cut-report-xml.c",
                            cut_backtrace_entry_get_file(entry));
    cut_assert_equal_uint(31, cut_backtrace_entry_get_line(entry));
    cut_assert_equal_string("stub_error_test",
                            cut_backtrace_entry_get_function(entry));

    cut_assert_equal_string("This test should error",
                            cut_test_result_get_message(result));

    cut_assert_equal_string("1234",
                            cut_test_get_attribute(test, "bug"));
    cut_assert_equal_string("Error Test",
                            cut_test_get_description(test));
}

void
test_result_iterated_test (void)
{
    GTimeVal expected_start_time, actual_start_time;
    CutTest *test;
    CutTestData *test_data;
    const gchar xml[] =
        "<stream>\n"
        "  <test-result>\n"
        "    <iterated-test>\n"
        "      <name>test_count</name>\n"
        "      <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "      <elapsed>0.028738</elapsed>\n"
        "    </iterated-test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>test_cut_pipeline</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.028738</elapsed>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>test_count</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.028738</elapsed>\n"
        "      </test-iterator>\n"
        "      <iterated-test>\n"
        "        <name>test_count</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.028738</elapsed>\n"
        "      </iterated-test>\n"
        "      <test-data>\n"
        "        <name>success</name>\n"
        "      </test-data>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>test_cut_pipeline</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.028738</elapsed>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>test_count</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.028738</elapsed>\n"
        "      </test-iterator>\n"
        "      <iterated-test>\n"
        "        <name>test_count</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.028738</elapsed>\n"
        "      </iterated-test>\n"
        "      <test-data>\n"
        "        <name>first data</name>\n"
        "      </test-data>\n"
        "      <status>success</status>\n"
        "      <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "      <elapsed>0.028738</elapsed>\n"
        "    </result>\n"
        "  </test-result>\n"
        "</stream>\n";

    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_parse(xml);
    cut_assert_not_null(result);

    test = cut_test_result_get_test(result);
    cut_assert(test);

    cut_assert_equal_string("test_cut_pipeline",
                            cut_test_result_get_test_case_name(result));
    cut_assert_equal_string("test_count",
                            cut_test_result_get_test_iterator_name(result));
    test_data = cut_test_result_get_test_data(result);
    cut_assert_not_null(test_data);
    cut_assert_equal_string("first data", cut_test_data_get_name(test_data));
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS,
                         cut_test_result_get_status(result));

    expected_start_time.tv_sec = 1217373749;
    expected_start_time.tv_usec = 0;
    cut_test_result_get_start_time(result, &actual_start_time);
    gcut_assert_equal_time_val(&expected_start_time, &actual_start_time);
    cut_assert_equal_double(0.028, 0.001, cut_test_result_get_elapsed(result));
}

void
test_result_test_iterator (void)
{
    GTimeVal expected_start_time, actual_start_time;
    CutTestIterator *test_iterator;
    const gchar xml[] =
        "<stream>\n"
        "  <test-iterator-result>\n"
        "    <test-iterator>\n"
        "      <name>test_count</name>\n"
        "      <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "      <elapsed>0.152822</elapsed>\n"
        "    </test-iterator>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>test_cut_pipeline</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.152822</elapsed>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>test_count</name>\n"
        "        <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "        <elapsed>0.152822</elapsed>\n"
        "      </test-iterator>\n"
        "      <status>success</status>\n"
        "      <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "      <elapsed>0.152822</elapsed>\n"
        "    </result>\n"
        "  </test-iterator-result>\n"
        "</stream>\n";

    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_parse(xml);
    cut_assert_not_null(result);

    test_iterator = cut_test_result_get_test_iterator(result);
    cut_assert_not_null(test_iterator);

    cut_assert_equal_string("test_count",
                            cut_test_get_name(CUT_TEST(test_iterator)));
    cut_assert_equal_string("test_count",
                            cut_test_result_get_test_iterator_name(result));
    cut_assert_equal_string("test_cut_pipeline",
                            cut_test_result_get_test_case_name(result));
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS,
                         cut_test_result_get_status(result));

    expected_start_time.tv_sec = 1217373749;
    expected_start_time.tv_usec = 0;
    cut_test_result_get_start_time(result, &actual_start_time);
    gcut_assert_equal_time_val(&expected_start_time, &actual_start_time);
    cut_assert_equal_double(0.15, 0.01, cut_test_result_get_elapsed(result));
}

void
test_result_test_case (void)
{
    GTimeVal expected_start_time, actual_start_time;
    CutTestCase *test_case;
    const gchar xml[] =
        "<stream>\n"
        "  <test-case-result>\n"
        "    <test-case>\n"
        "      <name>test_cut_test_iterator</name>\n"
        "      <start-time>2008-07-29T23:22:28Z</start-time>\n"
        "      <elapsed>0.010663</elapsed>\n"
        "    </test-case>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>test_cut_test_iterator</name>\n"
        "        <start-time>2008-07-29T23:22:28Z</start-time>\n"
        "        <elapsed>0.010663</elapsed>\n"
        "      </test-case>\n"
        "      <status>success</status>\n"
        "      <start-time>2008-07-29T23:22:28Z</start-time>\n"
        "      <elapsed>0.010663</elapsed>\n"
        "    </result>\n"
        "  </test-case-result>\n"
        "</stream>\n";

    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_parse(xml);
    cut_assert_not_null(result);

    test_case = cut_test_result_get_test_case(result);
    cut_assert_not_null(test_case);

    cut_assert_equal_string("test_cut_test_iterator",
                            cut_test_get_name(CUT_TEST(test_case)));
    cut_assert_equal_string("test_cut_test_iterator",
                            cut_test_result_get_test_case_name(result));
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS,
                         cut_test_result_get_status(result));

    expected_start_time.tv_sec = 1217373748;
    expected_start_time.tv_usec = 0;
    cut_test_result_get_start_time(result, &actual_start_time);
    gcut_assert_equal_time_val(&expected_start_time, &actual_start_time);
    cut_assert_equal_double(0.01, 0.001, cut_test_result_get_elapsed(result));
}

void
test_pass_assertion_test (void)
{
    CuttestPassAssertionInfo *info;
    CutTest *test;
    CutTestCase *test_case;
    CutTestContext *test_context;
    const gchar xml[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-suite>\n"
        "    <n-test-cases>34</n-test-cases>\n"
        "    <n-tests>236</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>test_cut_test</name>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-case>\n"
        "    <n-tests>13</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>test_cut_test</name>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <start-test>\n"
        "    <test>\n"
        "      <name>test_error_signal</name>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>test_cut_test</name>\n"
        "        <elapsed>0.000000</elapsed>\n"
        "      </test-case>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-test>\n"
        "  <pass-assertion>\n"
        "    <test>\n"
        "      <name>test_error_signal</name>\n"
        "      <elapsed>0.000039</elapsed>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>test_cut_test</name>\n"
        "        <elapsed>0.000062</elapsed>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>test_error_signal</name>\n"
        "        <elapsed>0.000077</elapsed>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </pass-assertion>\n"
        "  <test-result>\n"
        "    <test>\n"
        "      <name>test_error_signal</name>\n"
        "      <elapsed>0.000895</elapsed>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>test_cut_test</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>test_error_signal</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>test_cut_test</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>test_error_signal</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test>\n"
        "      <status>success</status>\n"
        "      <elapsed>0.000895</elapsed>\n"
        "    </result>\n"
        "  </test-result>\n"
        "  <complete-test>\n"
        "    <test>\n"
        "      <name>test_error_signal</name>\n"
        "      <elapsed>0.000895</elapsed>\n"
        "    </test>\n"
        "  </complete-test>\n"
        "  <complete-test-case>\n"
        "    <test-case>\n"
        "      <name>test_cut_test</name>\n"
        "      <elapsed>0.003745</elapsed>\n"
        "    </test-case>\n"
        "  </complete-test-case>\n"
        "  <complete-test-suite>\n"
        "    <test-suite>\n"
        "      <elapsed>139.666143</elapsed>\n"
        "    </test-suite>\n"
        "  </complete-test-suite>\n"
        "  <success>false</success>\n"
        "</stream>\n";

    cut_assert_null(receiver->pass_assertions);
    cut_assert_parse(xml);
    cut_assert_not_null(receiver->pass_assertions);

    cut_assert_equal_uint(1, g_list_length(receiver->pass_assertions));
    info = receiver->pass_assertions->data;

    test = info->test;
    cut_assert_not_null(test);
    cut_assert_equal_string("test_error_signal", cut_test_get_name(test));

    test_context = info->test_context;
    cut_assert_not_null(test_context);

    test_case = cut_test_context_get_test_case(test_context);
    cut_assert_not_null(test_case);
    cut_assert_equal_string("test_cut_test",
                            cut_test_get_name(CUT_TEST(test_case)));

    test = cut_test_context_get_test(test_context);
    cut_assert_not_null(test);
    cut_assert_equal_string("test_error_signal", cut_test_get_name(test));
}

void
test_pass_assertion_iterated_test (void)
{
    CuttestPassAssertionInfo *info;
    CutTestData *test_data;
    CutTest *test;
    CutTestIterator *test_iterator;
    CutTestCase *test_case;
    CutTestContext *test_context;
    const gchar xml[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-suite>\n"
        "    <n-test-cases>34</n-test-cases>\n"
        "    <n-tests>236</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>test_cut_test</name>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-case>\n"
        "    <n-tests>13</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>test_cut_test</name>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <start-test>\n"
        "    <test>\n"
        "      <name>test_error_signal</name>\n"
        "      <elapsed>0.000000</elapsed>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>test_cut_test</name>\n"
        "        <elapsed>0.000000</elapsed>\n"
        "      </test-case>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-test>\n"
        "  <pass-assertion>\n"
        "    <iterated-test>\n"
        "      <name>test_count</name>\n"
        "      <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "      <elapsed>0.039929</elapsed>\n"
        "    </iterated-test>\n"
        "   <test-context>\n"
        "     <test-case>\n"
        "       <name>test_cut_pipeline</name>\n"
        "       <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "       <elapsed>0.068691</elapsed>\n"
        "     </test-case>\n"
        "     <test-iterator>\n"
        "       <name>test_count</name>\n"
        "       <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "       <elapsed>0.068712</elapsed>\n"
        "     </test-iterator>\n"
        "     <iterated-test>\n"
        "       <name>test_count</name>\n"
        "       <start-time>2008-07-29T23:22:29Z</start-time>\n"
        "       <elapsed>0.039995</elapsed>\n"
        "     </iterated-test>\n"
        "     <test-data>\n"
        "       <name>failure</name>\n"
        "     </test-data>\n"
        "     <failed>FALSE</failed>\n"
        "   </test-context>\n"
        "  </pass-assertion>\n"
        "  <test-result>\n"
        "    <test>\n"
        "      <name>test_error_signal</name>\n"
        "      <elapsed>0.000895</elapsed>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>test_cut_test</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>test_error_signal</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>test_cut_test</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>test_error_signal</name>\n"
        "        <elapsed>0.000895</elapsed>\n"
        "      </test>\n"
        "      <status>success</status>\n"
        "      <elapsed>0.000895</elapsed>\n"
        "    </result>\n"
        "  </test-result>\n"
        "  <complete-test>\n"
        "    <test>\n"
        "      <name>test_error_signal</name>\n"
        "      <elapsed>0.000895</elapsed>\n"
        "    </test>\n"
        "  </complete-test>\n"
        "  <complete-test-case>\n"
        "    <test-case>\n"
        "      <name>test_cut_test</name>\n"
        "      <elapsed>0.003745</elapsed>\n"
        "    </test-case>\n"
        "  </complete-test-case>\n"
        "  <complete-test-suite>\n"
        "    <test-suite>\n"
        "      <elapsed>139.666143</elapsed>\n"
        "    </test-suite>\n"
        "  </complete-test-suite>\n"
        "  <success>false</success>\n"
        "</stream>\n";

    cut_assert_null(receiver->pass_assertions);
    cut_assert_parse(xml);
    cut_assert_not_null(receiver->pass_assertions);

    cut_assert_equal_uint(1, g_list_length(receiver->pass_assertions));
    info = receiver->pass_assertions->data;

    test = info->test;
    cut_assert_not_null(test);
    cut_assert_equal_string("test_count", cut_test_get_name(test));

    test_context = info->test_context;
    cut_assert_not_null(test_context);

    test_case = cut_test_context_get_test_case(test_context);
    cut_assert_not_null(test_case);
    cut_assert_equal_string("test_cut_pipeline",
                            cut_test_get_name(CUT_TEST(test_case)));

    test_iterator = cut_test_context_get_test_iterator(test_context);
    cut_assert_not_null(test_iterator);
    cut_assert_equal_string("test_count",
                            cut_test_get_name(CUT_TEST(test_iterator)));

    test_data = cut_test_context_get_current_data(test_context);
    cut_assert_not_null(test_data);
    cut_assert_equal_string("failure", cut_test_data_get_name(test_data));

    cut_assert_false(cut_test_context_is_failed(test_context));
}

void
test_complete_iterated_test (void)
{
    CuttestCompleteIteratedTestInfo *info;
    CutTestContext *context;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTestIterator *test_iterator;
    CutIteratedTest *iterated_test;
    CutTestData *test_data;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <ready-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "    <n-tests>5</n-tests>\n"
        "  </ready-test-iterator>\n"
        "  <start-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "  </start-test-iterator>\n"
        "  <start-iterated-test>\n"
        "    <iterated-test>\n"
        "      <name>my iterated test</name>\n"
        "    </iterated-test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>my test iterator</name>\n"
        "      </test-iterator>\n"
        "      <iterated-test>\n"
        "        <name>my iterated test</name>\n"
        "      </iterated-test>\n"
        "      <test-data>\n"
        "        <name>the first test data</name>\n"
        "      </test-data>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-iterated-test>\n";
    gchar complete_iterated_test[] =
        "  <complete-iterated-test>\n"
        "    <iterated-test>\n"
        "      <name>my iterated test</name>\n"
        "    </iterated-test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>my test iterator</name>\n"
        "      </test-iterator>\n"
        "      <iterated-test>\n"
        "        <name>my iterated test</name>\n"
        "      </iterated-test>\n"
        "      <test-data>\n"
        "        <name>the first test data</name>\n"
        "      </test-data>\n"
        "      <failed>TRUE</failed>\n"
        "    </test-context>\n"
        "  </complete-iterated-test>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->complete_iterated_tests);

    cut_assert_parse(complete_iterated_test);
    cut_assert_equal_uint(1, g_list_length(receiver->complete_iterated_tests));

    info = receiver->complete_iterated_tests->data;
    cut_assert_not_null(info);

    cut_assert_not_null(info->iterated_test);
    cut_assert_equal_string("my iterated test",
                            cut_test_get_name(CUT_TEST(info->iterated_test)));

    context = info->test_context;
    cut_assert_not_null(context);
    cut_assert_true(cut_test_context_is_failed(context));

    test_suite = cut_test_context_get_test_suite(context);
    cut_assert_not_null(test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(test_suite)));

    test_case = cut_test_context_get_test_case(context);
    cut_assert_not_null(test_case);
    cut_assert_equal_string("my test case",
                            cut_test_get_name(CUT_TEST(test_case)));

    test_iterator = cut_test_context_get_test_iterator(context);
    cut_assert_not_null(test_iterator);
    cut_assert_equal_string("my test iterator",
                            cut_test_get_name(CUT_TEST(test_iterator)));

    iterated_test = CUT_ITERATED_TEST(cut_test_context_get_test(context));
    cut_assert_not_null(iterated_test);
    cut_assert_equal_string("my iterated test",
                            cut_test_get_name(CUT_TEST(iterated_test)));

    cut_assert_true(cut_test_context_have_data(context));
    test_data = cut_test_context_get_current_data(context);
    cut_assert_not_null(test_data);
    cut_assert_equal_string("the first test data",
                            cut_test_data_get_name(test_data));
}

void
test_complete_test (void)
{
    CuttestCompleteTestInfo *info;
    CutTestContext *context;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTest *test;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <start-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-test>\n";
    gchar complete_test[] =
        "  <complete-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>TRUE</failed>\n"
        "    </test-context>\n"
        "  </complete-test>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->complete_tests);

    cut_assert_parse(complete_test);
    cut_assert_equal_int(1, g_list_length(receiver->complete_tests));

    info = receiver->complete_tests->data;
    cut_assert_not_null(info);

    cut_assert_not_null(info->test);
    cut_assert_equal_string("my test", cut_test_get_name(CUT_TEST(info->test)));

    context = info->test_context;
    cut_assert_not_null(context);
    cut_assert_true(cut_test_context_is_failed(context));

    test_suite = cut_test_context_get_test_suite(context);
    cut_assert_not_null(test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(test_suite)));

    test_case = cut_test_context_get_test_case(context);
    cut_assert_not_null(test_case);
    cut_assert_equal_string("my test case",
                            cut_test_get_name(CUT_TEST(test_case)));

    test = cut_test_context_get_test(context);
    cut_assert_not_null(test);
    cut_assert_equal_string("my test", cut_test_get_name(test));
}

void
test_complete_test_iterator (void)
{
    CutTestIterator *test_iterator;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <ready-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "    <n-tests>5</n-tests>\n"
        "  </ready-test-iterator>\n"
        "  <start-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "  </start-test-iterator>\n"
        "  <start-iterated-test>\n"
        "    <iterated-test>\n"
        "      <name>my iterated test</name>\n"
        "    </iterated-test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>my test iterator</name>\n"
        "      </test-iterator>\n"
        "      <iterated-test>\n"
        "        <name>my iterated test</name>\n"
        "      </iterated-test>\n"
        "      <test-data>\n"
        "        <name>the first test data</name>\n"
        "      </test-data>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-iterated-test>\n"
        "  <complete-iterated-test>\n"
        "    <iterated-test>\n"
        "      <name>my iterated test</name>\n"
        "    </iterated-test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test-iterator>\n"
        "        <name>my test iterator</name>\n"
        "      </test-iterator>\n"
        "      <iterated-test>\n"
        "        <name>my iterated test</name>\n"
        "      </iterated-test>\n"
        "      <test-data>\n"
        "        <name>the first test data</name>\n"
        "      </test-data>\n"
        "      <failed>TRUE</failed>\n"
        "    </test-context>\n"
        "  </complete-iterated-test>\n";
    gchar complete_test_iterator[] =
        "  <complete-test-iterator>\n"
        "    <test-iterator>\n"
        "      <name>my test iterator</name>\n"
        "    </test-iterator>\n"
        "  </complete-test-iterator>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->complete_test_iterators);

    cut_assert_parse(complete_test_iterator);
    cut_assert_equal_uint(1, g_list_length(receiver->complete_test_iterators));

    test_iterator = receiver->complete_test_iterators->data;
    cut_assert_not_null(test_iterator);
    cut_assert_equal_string("my test iterator",
                            cut_test_get_name(CUT_TEST(test_iterator)));
}

void
test_complete_test_case (void)
{
    CutTestCase *test_case;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <start-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-test>\n"
        "  <complete-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>TRUE</failed>\n"
        "    </test-context>\n"
        "  </complete-test>\n";
    gchar complete_test_case[] =
        "  <complete-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </complete-test-case>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->complete_test_cases);

    cut_assert_parse(complete_test_case);
    cut_assert_equal_int(1, g_list_length(receiver->complete_test_cases));

    test_case = receiver->complete_test_cases->data;
    cut_assert_not_null(test_case);
    cut_assert_equal_string("my test case",
                            cut_test_get_name(CUT_TEST(test_case)));
}

void
test_complete_test_suite (void)
{
    CutTestSuite *test_suite;
    gchar header[] =
        "<stream>\n"
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "    <n-test-cases>3</n-test-cases>\n"
        "    <n-tests>7</n-tests>\n"
        "  </ready-test-suite>\n"
        "  <start-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </start-test-suite>\n"
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "    <n-tests>2</n-tests>\n"
        "  </ready-test-case>\n"
        "  <start-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </start-test-case>\n"
        "  <start-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "  </start-test>\n"
        "  <complete-test>\n"
        "    <test>\n"
        "      <name>my test</name>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-suite>\n"
        "      </test-suite>\n"
        "      <test-case>\n"
        "        <name>my test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>my test</name>\n"
        "      </test>\n"
        "      <failed>TRUE</failed>\n"
        "    </test-context>\n"
        "  </complete-test>\n"
        "  <complete-test-case>\n"
        "    <test-case>\n"
        "      <name>my test case</name>\n"
        "    </test-case>\n"
        "  </complete-test-case>\n";
    gchar complete_test_suite[] =
        "  <complete-test-suite>\n"
        "    <test-suite>\n"
        "    </test-suite>\n"
        "  </complete-test-suite>\n";

    cut_assert_parse(header);
    cut_assert_null(receiver->complete_test_suites);

    cut_assert_parse(complete_test_suite);
    cut_assert_equal_int(1, g_list_length(receiver->complete_test_suites));

    test_suite = receiver->complete_test_suites->data;
    cut_assert_not_null(test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(test_suite)));
}

void
test_complete_run_without_success_tag (void)
{
    cut_assert_parse("<stream>\n");
    cut_assert_equal_int(1, receiver->n_start_runs);

    cut_assert_null(receiver->complete_runs);
    cut_assert_parse("</stream>\n");
    cut_assert_equal_int(1, g_list_length(receiver->complete_runs));
    cut_assert_true(GPOINTER_TO_INT(receiver->complete_runs->data));
}

void
test_complete_run_with_success_true (void)
{
    cut_assert_parse("<stream>\n");
    cut_assert_equal_int(1, receiver->n_start_runs);

    cut_assert_null(receiver->complete_runs);
    cut_assert_parse("  <success>TRUE</success>\n");
    cut_assert_null(receiver->complete_runs);
    cut_assert_parse("</stream>\n");
    cut_assert_equal_int(1, g_list_length(receiver->complete_runs));
    cut_assert_true(GPOINTER_TO_INT(receiver->complete_runs->data));
}

void
test_complete_run_with_success_false (void)
{
    cut_assert_parse("<stream>\n");
    cut_assert_equal_int(1, receiver->n_start_runs);

    cut_assert_null(receiver->complete_runs);
    cut_assert_parse("  <success>FALSE</success>\n");
    cut_assert_null(receiver->complete_runs);
    cut_assert_parse("</stream>");
    cut_assert_equal_int(1, g_list_length(receiver->complete_runs));
    cut_assert_false(GPOINTER_TO_INT(receiver->complete_runs->data));
}

void
test_crash_test (void)
{
    gchar xml[] =
        "<stream>\n"
        "  <test-result>\n"
        "    <test>\n"
        "      <name>stub-crash-test</name>\n"
        "      <description>Crash Test</description>\n"
        "      <option>\n"
        "        <name>bug</name>\n"
        "        <value>1234</value>\n"
        "      </option>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>stub test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>stub-crash-test</name>\n"
        "        <description>Crash Test</description>\n"
        "        <option>\n"
        "          <name>bug</name>\n"
        "          <value>1234</value>\n"
        "        </option>\n"
        "      </test>\n"
        "      <failed>TRUE</failed>\n"
        "    </test-context>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>stub test case</name>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>stub-crash-test</name>\n"
        "        <description>Crash Test</description>\n"
        "        <option>\n"
        "          <name>bug</name>\n"
        "          <value>1234</value>\n"
        "        </option>\n"
        "      </test>\n"
        "      <status>crash</status>\n"
        "      <detail>This test should crash</detail>\n"
        "      <backtrace>\n"
        "        <entry>\n"
        "          <file>test-cut-stream-parser.c</file>\n"
        "          <line>1099</line>\n"
        "          <info>test_crash_test()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <info>cut_test_run()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <info>run()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <info>cut_test_case_run_with_filter()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <file>cut-test-suite.c</file>\n"
        "          <line>129</line>\n"
        "          <info>run()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <info>cut_test_suite_run_test_cases()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <info>cut_test_suite_run_with_filter()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <file>cut-runner.c</file>\n"
        "          <line>67</line>\n"
        "          <info>cut_runner_run()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <info>cut_run_context_start()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <info>cut_start_run_context()</info>\n"
        "        </entry>\n"
        "        <entry>\n"
        "          <file>cut-main.c</file>\n"
        "          <line>317</line>\n"
        "          <info>cut_run()</info>\n"
        "        </entry>\n"
        "      </backtrace>\n"
        "      <start-time>2008-07-29T05:16:40Z</start-time>\n"
        "      <elapsed>0.000100</elapsed>\n"
        "    </result>\n"
        "  </test-result>\n"
        "</stream>\n";
/*
    gchar crashed_backtrace[] =
        "#4  0x00007fd67b4fbfc5 in test_crash_test () at test-cut-stream-parser.c:1099\n"
        "#5  0x00007fd68285ea77 in cut_test_run (test=0xfc0e30, test_context=0xf90840, \n"
        "#6  0x00007fd682860cc4 in run (test_case=0xfb3400, test=0xfc0e30, \n"
        "#7  0x00007fd682860e9d in cut_test_case_run_with_filter (test_case=0xfb3400, \n"
        "#8  0x00007fd682862c66 in run (data=0xfc3560) at cut-test-suite.c:129\n"
        "#9  0x00007fd68286313e in cut_test_suite_run_test_cases (test_suite=0xf88c60, \n"
        "#10 0x00007fd6828631e0 in cut_test_suite_run_with_filter (test_suite=0xf88c60, \n"
        "#11 0x00007fd68285dbe8 in cut_runner_run (runner=0xf8d840) at cut-runner.c:67\n"
        "#12 0x00007fd68285bc7f in cut_run_context_start (context=0xf8d840)\n"
        "#13 0x00007fd68285e072 in cut_start_run_context (run_context=0xf8d840)\n"
        "#14 0x00007fd68285e1be in cut_run () at cut-main.c:317\n";
*/


    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_parse(xml);

    cut_assert_equal_int(CUT_TEST_RESULT_CRASH,
                         cut_test_result_get_status(result));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
