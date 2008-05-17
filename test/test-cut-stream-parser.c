#include "cutter.h"
#include "cut-stream-parser.h"

void test_parse (void);
void test_start_run (void);
void test_complete_run (void);

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
    GList *complete_runs;
};

struct _CuttestEventReceiverClass
{
    CutRunContextClass parent_class;
};

GType          cuttest_event_receiver_get_type  (void) G_GNUC_CONST;

static CutRunContext *cuttest_event_receiver_new       (void);

G_DEFINE_TYPE(CuttestEventReceiver, cuttest_event_receiver, CUT_TYPE_RUN_CONTEXT)

static void
dispose (GObject *object)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(object);

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
    const gchar xml[] = "<result>\n"
                        "  <test-case>\n"
                        "    <name>dummy test case</name>\n"
                        "  </test-case>\n"
                        "  <test>\n"
                        "    <name>dummy-error-test</name>\n"
                        "    <description>Error Test</description>\n"
                        "    <option>\n"
                        "      <name>bug</name>\n"
                        "      <value>1234</value>\n"
                        "    </option>\n"
                        "  </test>\n"
                        "  <status>error</status>\n"
                        "  <detail>This test should error</detail>\n"
                        "  <backtrace>\n"
                        "    <file>test-cut-report-xml.c</file>\n"
                        "    <line>31</line>\n"
                        "    <info>dummy_error_test()</info>\n"
                        "  </backtrace>\n"
                        "  <elapsed>0.000100</elapsed>\n"
                        "</result>\n";
    CutTest *test;

    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)&result);

    cut_assert_true(cut_stream_parser_parse(parser, xml, -1, NULL));
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
test_complete_run (void)
{
    cut_assert_true(cut_stream_parser_parse(parser, "<stream>", -1, NULL));
    cut_assert_equal_int(1, receiver->n_start_runs);

    cut_assert_null(0, receiver->complete_runs);
    cut_assert_true(cut_stream_parser_parse(parser, "</stream>", -1, NULL));
    cut_assert_equal_int(1, g_list_length(receiver->complete_runs));
    cut_assert_true(GPOINTER_TO_INT(receiver->complete_runs->data));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
