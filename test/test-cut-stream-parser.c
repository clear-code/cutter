#include <gcutter.h>
#include <cutter/cut-stream-parser.h>

void test_parse (void);
void test_start_run (void);
void test_ready_test_suite (void);
void test_complete_run_without_success_tag (void);
void test_complete_run_with_success_true (void);
void test_complete_run_with_success_false (void);

#define CUTTEST_TYPE_EVENT_RECEIVER            (cuttest_event_receiver_get_type ())
#define CUTTEST_EVENT_RECEIVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTTEST_TYPE_EVENT_RECEIVER, CuttestEventReceiver))
#define CUTTEST_EVENT_RECEIVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTTEST_TYPE_EVENT_RECEIVER, CuttestEventReceiverClass))
#define CUTTEST_IS_EVENT_RECEIVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTTEST_TYPE_EVENT_RECEIVER))
#define CUTTEST_IS_EVENT_RECEIVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTTEST_TYPE_EVENT_RECEIVER))
#define CUTTEST_EVENT_RECEIVER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUTTEST_TYPE_EVENT_RECEIVER, CuttestEventReceiverClass))

typedef struct _CuttestEventReceiver      CuttestEventReceiver;
typedef struct _CuttestEventReceiverClass CuttestEventReceiverClass;

struct _CuttestEventReceiver
{
    CutRunContext object;

    gint n_start_runs;
    GList *ready_test_suites;
    GList *complete_runs;
};

struct _CuttestEventReceiverClass
{
    CutRunContextClass parent_class;
};

GType          cuttest_event_receiver_get_type  (void) G_GNUC_CONST;

static CutRunContext *cuttest_event_receiver_new       (void);

G_DEFINE_TYPE(CuttestEventReceiver, cuttest_event_receiver, CUT_TYPE_RUN_CONTEXT)

typedef struct _ReadyTestSuiteInfo
{
    CutTestSuite *test_suite;
    gint n_test_cases;
    gint n_tests;
} ReadyTestSuiteInfo;

static ReadyTestSuiteInfo *
ready_test_suite_info_new (CutTestSuite *test_suite, gint n_test_cases,
                           gint n_tests)
{
    ReadyTestSuiteInfo *info;

    info = g_slice_new(ReadyTestSuiteInfo);
    info->test_suite = test_suite;
    if (info->test_suite)
        g_object_ref(info->test_suite);
    info->n_test_cases = n_test_cases;
    info->n_tests = n_tests;

    return info;
}

static void
ready_test_suite_info_free (ReadyTestSuiteInfo *info)
{
    if (info->test_suite)
        g_object_unref(info->test_suite);
    g_slice_free(ReadyTestSuiteInfo, info);
}

static void
dispose (GObject *object)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(object);

    if (receiver->ready_test_suites) {
        g_list_foreach(receiver->ready_test_suites,
                       (GFunc)ready_test_suite_info_free, NULL);
        g_list_free(receiver->ready_test_suites);
        receiver->ready_test_suites = NULL;
    }

    if (receiver->complete_runs) {
        g_list_free(receiver->complete_runs);
        receiver->complete_runs = NULL;
    }
}

static void
start_run (CutRunContext *context)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->n_start_runs++;
}

static void
ready_test_suite (CutRunContext *context, CutTestSuite *test_suite,
                  guint n_test_cases, guint n_tests)
{
    CuttestEventReceiver *receiver;
    ReadyTestSuiteInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = ready_test_suite_info_new(test_suite, n_test_cases, n_tests);
    receiver->ready_test_suites =
        g_list_append(receiver->ready_test_suites, info);
}

static void
complete_run (CutRunContext *context, gboolean success)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->complete_runs = g_list_append(receiver->complete_runs,
                                            GINT_TO_POINTER(success));
}

static void
cuttest_event_receiver_class_init (CuttestEventReceiverClass *klass)
{
    GObjectClass *gobject_class;
    CutRunContextClass *run_context_class;

    gobject_class = G_OBJECT_CLASS(klass);
    run_context_class = CUT_RUN_CONTEXT_CLASS(klass);

    gobject_class->dispose = dispose;

    run_context_class->start_run = start_run;
    run_context_class->ready_test_suite = ready_test_suite;
    run_context_class->complete_run = complete_run;
}

static void
cuttest_event_receiver_init (CuttestEventReceiver *receiver)
{
    receiver->n_start_runs = 0;
    receiver->complete_runs = NULL;
}

static CutRunContext *
cuttest_event_receiver_new (void)
{
    return g_object_new(CUTTEST_TYPE_EVENT_RECEIVER, NULL);
}

static CutStreamParser *parser;
static CutTestResult *result;
static CutRunContext *run_context;
static CuttestEventReceiver *receiver;

void
setup (void)
{
    run_context = cuttest_event_receiver_new();
    receiver = CUTTEST_EVENT_RECEIVER(run_context);
    parser = cut_stream_parser_new(run_context);
    result = NULL;
}

void
teardown (void)
{
    g_object_unref(parser);

    if (result)
        g_object_unref(result);
}

#define cut_assert_parse(string) do                             \
{                                                               \
    GError *error = NULL;                                       \
    cut_stream_parser_parse(parser, (string), -1, &error);      \
    cut_assert_g_error(error);                                  \
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
test_parse (void)
{
    const gchar xml[] =
        "<stream>\n"
        "  <result>\n"
        "    <test-case>\n"
        "      <name>dummy test case</name>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>dummy-error-test</name>\n"
        "      <description>Error Test</description>\n"
        "      <option>\n"
        "        <name>bug</name>\n"
        "        <value>1234</value>\n"
        "      </option>\n"
        "    </test>\n"
        "    <status>error</status>\n"
        "    <detail>This test should error</detail>\n"
        "    <backtrace>\n"
        "      <file>test-cut-report-xml.c</file>\n"
        "      <line>31</line>\n"
        "      <info>dummy_error_test()</info>\n"
        "    </backtrace>\n"
        "    <elapsed>0.000100</elapsed>\n"
        "  </result>\n"
        "</stream>\n";
    CutTest *test;

    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_parse(xml);
    cut_assert(result);

    test = cut_test_result_get_test(result);
    cut_assert(test);

    cut_assert_equal_string("dummy test case",
                            cut_test_result_get_test_case_name(result));
    cut_assert_equal_string("dummy-error-test",
                            cut_test_result_get_test_name(result));
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    cut_assert_equal_double(0.0001, 0.0, cut_test_result_get_elapsed(result));
    cut_assert_equal_int(31, cut_test_result_get_line(result));
    cut_assert_equal_string("test-cut-report-xml.c",
                            cut_test_result_get_filename(result));
    cut_assert_equal_string("dummy_error_test",
                            cut_test_result_get_function_name(result));
    cut_assert_equal_string("This test should error",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("1234",
                            cut_test_get_attribute(test, "bug"));
    cut_assert_equal_string("Error Test",
                            cut_test_get_description(test));
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
    ReadyTestSuiteInfo *info;

    cut_assert_parse("<stream>\n");
    cut_assert_parse("  <ready-test-suite>\n");
    cut_assert_parse("    <test-suite>\n");
    cut_assert_parse("    </test-suite>\n");
    cut_assert_parse("    <n-test-cases>3</n-test-cases>\n");
    cut_assert_parse("    <n-tests>7</n-tests>\n");

    cut_assert_null(0, receiver->ready_test_suites);
    cut_assert_parse("  </ready-test-suite>");
    cut_assert_equal_int(1, g_list_length(receiver->ready_test_suites));

    info = receiver->ready_test_suites->data;
    cut_assert_not_null(info->test_suite);
    cut_assert_equal_string(NULL, cut_test_get_name(CUT_TEST(info->test_suite)));
    cut_assert_equal_int(3, info->n_test_cases);
    cut_assert_equal_int(7, info->n_tests);
}

void
test_complete_run_without_success_tag (void)
{
    cut_assert_parse("<stream>\n");
    cut_assert_equal_int(1, receiver->n_start_runs);

    cut_assert_null(0, receiver->complete_runs);
    cut_assert_parse("</stream>\n");
    cut_assert_equal_int(1, g_list_length(receiver->complete_runs));
    cut_assert_true(GPOINTER_TO_INT(receiver->complete_runs->data));
}

void
test_complete_run_with_success_true (void)
{
    cut_assert_parse("<stream>\n");
    cut_assert_equal_int(1, receiver->n_start_runs);

    cut_assert_null(0, receiver->complete_runs);
    cut_assert_parse("  <success>TRUE</success>\n");
    cut_assert_null(0, receiver->complete_runs);
    cut_assert_parse("</stream>\n");
    cut_assert_equal_int(1, g_list_length(receiver->complete_runs));
    cut_assert_true(GPOINTER_TO_INT(receiver->complete_runs->data));
}

void
test_complete_run_with_success_false (void)
{
    cut_assert_parse("<stream>\n");
    cut_assert_equal_int(1, receiver->n_start_runs);

    cut_assert_null(0, receiver->complete_runs);
    cut_assert_parse("  <success>FALSE</success>\n");
    cut_assert_null(0, receiver->complete_runs);
    cut_assert_parse("</stream>");
    cut_assert_equal_int(1, g_list_length(receiver->complete_runs));
    cut_assert_false(GPOINTER_TO_INT(receiver->complete_runs->data));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
