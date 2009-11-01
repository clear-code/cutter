#include <gcutter.h>
#include <cutter/cut-test-iterator.h>
#include <cutter/cut-test-runner.h>

#include "../lib/cuttest-assertions.h"

void test_success(void);
void test_failure(void);
void test_error(void);
void test_pending(void);
void test_notification(void);
void test_omission(void);
void test_error_in_data_setup(void);

static CutRunContext *run_context;
static CutTestCase *test_case;
static CutTestIterator *test_iterator;
static gboolean run_test_flag = FALSE;
static guint n_setup_calls = 0;
static guint n_teardown_calls = 0;
static guint n_tests = 0;
static guint n_start_signals = 0;
static guint n_complete_signals = 0;
static guint n_success_signals = 0;
static guint n_failure_signals = 0;
static guint n_error_signals = 0;
static guint n_pending_signals = 0;
static guint n_pass_assertion_signals = 0;
static guint n_notification_signals = 0;
static guint n_omission_signals = 0;

static gint fail_line;

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))


static void
cut_assert_n_signals_helper(guint expected_n_start_signals,
                            guint expected_n_complete_signals,
                            guint expected_n_setup_calls,
                            guint expected_n_teardown_calls,
                            guint expected_n_tests,
                            guint expected_n_assertions,
                            guint expected_n_successes,
                            guint expected_n_failures,
                            guint expected_n_errors,
                            guint expected_n_pendings,
                            guint expected_n_notifications,
                            guint expected_n_omissions)
{
    GList *result_summary;
    const GList *expected_result_summary;
    const GList *actual_result_summary;

#define APPEND(value)                                                   \
    result_summary = g_list_append(result_summary, GUINT_TO_POINTER(value))

    result_summary = NULL;
    APPEND(expected_n_start_signals);
    APPEND(expected_n_complete_signals);
    APPEND(expected_n_setup_calls);
    APPEND(expected_n_teardown_calls);
    APPEND(expected_n_tests);
    APPEND(expected_n_assertions);
    APPEND(expected_n_successes);
    APPEND(expected_n_failures);
    APPEND(expected_n_errors);
    APPEND(expected_n_pendings);
    APPEND(expected_n_notifications);
    APPEND(expected_n_omissions);
    expected_result_summary = gcut_take_list(result_summary, NULL);

    result_summary = NULL;
    APPEND(n_start_signals);
    APPEND(n_complete_signals);
    APPEND(n_setup_calls);
    APPEND(n_teardown_calls);
    APPEND(n_tests);
    APPEND(n_pass_assertion_signals);
    APPEND(n_success_signals);
    APPEND(n_failure_signals);
    APPEND(n_error_signals);
    APPEND(n_pending_signals);
    APPEND(n_notification_signals);
    APPEND(n_omission_signals);
    actual_result_summary = gcut_take_list(result_summary, NULL);
#undef APPEND

    gcut_assert_equal_list_uint(expected_result_summary,
                                actual_result_summary);
}

#define cut_assert_n_signals(n_start_signals, n_complete_signals,       \
                             n_setup_calls, n_teardown_calls,           \
                             n_tests,                                   \
                             n_assertions, n_successes,                 \
                             n_failures, n_errors, n_pendings,          \
                             n_notifications, n_omissions)              \
    cut_trace_with_info_expression(                                     \
        cut_assert_n_signals_helper(n_start_signals,                    \
                                    n_complete_signals,                 \
                                    n_setup_calls, n_teardown_calls,    \
                                    n_tests, n_assertions,              \
                                    n_successes, n_failures,            \
                                    n_errors, n_pendings,               \
                                    n_notifications, n_omissions),      \
        cut_assert_n_signals(n_start_signals,                           \
                             n_complete_signals,                        \
                             n_setup_calls, n_teardown_calls,           \
                             n_tests, n_assertions,                     \
                             n_successes, n_failures,                   \
                             n_errors, n_pendings,                      \
                             n_notifications, n_omissions))

static void
stub_setup (void)
{
    n_setup_calls++;
}

static void
stub_teardown (void)
{
    n_teardown_calls++;
}

void
cut_setup (void)
{
    run_test_flag = FALSE;
    n_setup_calls = 0;
    n_teardown_calls = 0;
    n_tests = 0;
    n_start_signals = 0;
    n_complete_signals = 0;
    n_success_signals = 0;
    n_failure_signals = 0;
    n_error_signals = 0;
    n_pending_signals = 0;
    n_notification_signals = 0;
    n_pass_assertion_signals = 0;
    n_omission_signals = 0;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());
    test_case = cut_test_case_new("driver test case",
                                  stub_setup, stub_teardown,
                                  NULL, NULL);
    test_iterator = NULL;

    fail_line = 0;
}

void
cut_teardown (void)
{
    if (test_iterator)
        g_object_unref(test_iterator);
    g_object_unref(test_case);
    g_object_unref(run_context);
}

static void
cb_ready_signal (CutTestIterator *test_iteartor, guint n_iterated_tests,
                 gpointer data)
{
    n_tests += n_iterated_tests;
}

static void
cb_start_signal (CutTest *test, gpointer data)
{
    n_start_signals++;
}

static void
cb_complete_signal (CutTest *test, gpointer data)
{
    n_complete_signals++;
}

static void
cb_success_signal (CutTest *test, gpointer data)
{
    n_success_signals++;
}

static void
cb_failure_signal (CutTest *test, gpointer data)
{
    n_failure_signals++;
}

static void
cb_error_signal (CutTest *test, gpointer data)
{
    n_error_signals++;
}

static void
cb_pending_signal (CutTest *test, gpointer data)
{
    n_pending_signals++;
}

static void
cb_notification_signal (CutTest *test, gpointer data)
{
    n_notification_signals++;
}

static void
cb_pass_assertion_signal (CutTest *test, gpointer data)
{
    n_pass_assertion_signals++;
}

static void
cb_omission_signal (CutTest *test, gpointer data)
{
    n_omission_signals++;
}

static void
connect_signals (CutTest *test)
{
#define CONNECT(name)                                                   \
    g_signal_connect(test, #name, G_CALLBACK(cb_ ## name ## _signal), NULL)

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(pass_assertion);
    CONNECT(omission);

#undef CONNECT
}

static void
disconnect_signals (CutTest *test)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test,                          \
                                         G_CALLBACK(cb_ ## name ## _signal), \
                                         NULL)

    DISCONNECT(ready);
    DISCONNECT(start);
    DISCONNECT(complete);
    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);
    DISCONNECT(pass_assertion);
    DISCONNECT(omission);

#undef DISCONNECT
}

static gboolean
run (void)
{
    gboolean success;
    CutTestContext *test_context;
    CutTest *test;

    test = CUT_TEST(test_iterator);
    connect_signals(test);
    test_context = cut_test_context_new(run_context,
                                        NULL, test_case, NULL, NULL);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test_iterator(CUT_TEST_RUNNER(run_context),
                                                test_iterator, test_context);
    cut_test_context_current_pop();
    disconnect_signals(test);

    g_object_unref(test_context);

    return success;
}

static void
stub_iterated_data (void)
{
    cut_add_data("First", GINT_TO_POINTER(1), NULL,
                 "Second", GINT_TO_POINTER(2), NULL,
                 "Third", GINT_TO_POINTER(3), NULL);
}

static void
stub_success_iterated_test (gconstpointer data)
{
    static int i = 0;

    i++;
    cut_assert_true(TRUE, cut_message("always pass"));
    cut_assert_equal_int(i, GPOINTER_TO_INT(data));
}

void
test_success (void)
{
    CutTestContainer *container;

    test_iterator = cut_test_iterator_new("success test iterator",
                                          stub_success_iterated_test,
                                          stub_iterated_data);
    container = CUT_TEST_CONTAINER(test_iterator);
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));
    cut_assert_true(run());
    cut_assert_equal_uint(3, cut_test_container_get_n_tests(container, NULL));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 1, 0, 0, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 6, 3, 0, 0, 0, 0, 0);
}

static void
stub_failure_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, cut_message("always pass"));
    MARK_FAIL(cut_assert_equal_int(2, GPOINTER_TO_INT(data)));
    cut_assert_true(TRUE, cut_message("always pass if come here"));
}

void
test_failure (void)
{
    CutTestContainer *container;

    test_iterator = cut_test_iterator_new("failure test iterator",
                                          stub_failure_iterated_test,
                                          stub_iterated_data);
    container = CUT_TEST_CONTAINER(test_iterator);
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));
    cut_assert_false(run());
    cut_assert_equal_uint(3, cut_test_container_get_n_tests(container, NULL));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 5, 1, 2, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "failure test iterator (First)",
                           NULL,
                           "<2 == GPOINTER_TO_INT(data)>",
                           "2", "1",
                           FAIL_LOCATION, "stub_failure_iterated_test",
                           NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_SUCCESS,
                           "failure test iterator (Second)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_FAILURE,
                           "failure test iterator (Third)",
                           NULL,
                           "<2 == GPOINTER_TO_INT(data)>",
                           "2", "3",
                           FAIL_LOCATION, "stub_failure_iterated_test",
                           NULL);
}

static void
stub_error_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, cut_message("always pass"));
    if (GPOINTER_TO_INT(data) == 2)
        MARK_FAIL(cut_error("ERROR!"));
    cut_assert_true(TRUE, cut_message("always pass if come here"));
}

void
test_error (void)
{
    CutTestContainer *container;

    test_iterator = cut_test_iterator_new("error test iterator",
                                          stub_error_iterated_test,
                                          stub_iterated_data);
    container = CUT_TEST_CONTAINER(test_iterator);
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));
    cut_assert_false(run());
    cut_assert_equal_uint(3, cut_test_container_get_n_tests(container, NULL));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 5, 2, 0, 1, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "error test iterator (First)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_ERROR,
                           "error test iterator (Second)",
                           "ERROR!",
                           NULL,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_error_iterated_test",
                           NULL);
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "error test iterator (Third)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
}

static void
stub_pending_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, cut_message("always pass"));
    if (GPOINTER_TO_INT(data) == 2)
        MARK_FAIL(cut_pend("PENDING!"));
    cut_assert_true(TRUE, cut_message("always pass if come here"));
}

void
test_pending (void)
{
    CutTestContainer *container;

    test_iterator = cut_test_iterator_new("pending test iterator",
                                          stub_pending_iterated_test,
                                          stub_iterated_data);
    container = CUT_TEST_CONTAINER(test_iterator);
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));
    cut_assert_false(run());
    cut_assert_equal_uint(3, cut_test_container_get_n_tests(container, NULL));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 0, 1, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 5, 2, 0, 0, 1, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "pending test iterator (First)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_PENDING,
                           "pending test iterator (Second)",
                           "PENDING!",
                           NULL,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_pending_iterated_test",
                           NULL);
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "pending test iterator (Third)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
}

static void
stub_notification_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, cut_message("always pass"));
    if (GPOINTER_TO_INT(data) == 2)
        MARK_FAIL(cut_notify("NOTIFICATION!"));
    cut_assert_true(TRUE, cut_message("always pass if come here"));
}

void
test_notification (void)
{
    CutTestContainer *container;

    test_iterator = cut_test_iterator_new("notification test iterator",
                                          stub_notification_iterated_test,
                                          stub_iterated_data);
    container = CUT_TEST_CONTAINER(test_iterator);
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));
    cut_assert_true(run());
    cut_assert_equal_uint(3, cut_test_container_get_n_tests(container, NULL));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 0, 0, 1, 0);
    cut_assert_test_result_summary(run_context, 3, 6, 3, 0, 0, 0, 1, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "notification test iterator (First)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_NOTIFICATION,
                           "notification test iterator (Second)",
                           "NOTIFICATION!",
                           NULL,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_notification_iterated_test",
                           NULL);
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "notification test iterator (Second)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
    cut_assert_test_result(run_context, 3, CUT_TEST_RESULT_SUCCESS,
                           "notification test iterator (Third)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
}

static void
stub_omission_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, cut_message("always pass"));
    if (GPOINTER_TO_INT(data) == 2)
        MARK_FAIL(cut_omit("OMISSION!"));
    cut_assert_true(TRUE, cut_message("always pass if come here"));
}

void
test_omission (void)
{
    CutTestContainer *container;

    test_iterator = cut_test_iterator_new("omission test iterator",
                                          stub_omission_iterated_test,
                                          stub_iterated_data);
    container = CUT_TEST_CONTAINER(test_iterator);
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));
    cut_assert_true(run());
    cut_assert_equal_uint(3, cut_test_container_get_n_tests(container, NULL));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 0, 0, 0, 1);
    cut_assert_test_result_summary(run_context, 3, 5, 3, 0, 0, 0, 0, 1);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "omission test iterator (First)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_OMISSION,
                           "omission test iterator (Second)",
                           "OMISSION!",
                           NULL,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_omission_iterated_test",
                           NULL);
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "omission test iterator (Second)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
    cut_assert_test_result(run_context, 3, CUT_TEST_RESULT_SUCCESS,
                           "omission test iterator (Third)",
                           NULL, NULL,
                           NULL, NULL,
                           NULL);
}

static void
stub_error_in_data_setup_iterated_data (void)
{
    cut_add_data("First", GINT_TO_POINTER(1), NULL);
    cut_add_data("Second", GINT_TO_POINTER(2), NULL);
    cut_error("error in data setup");
    cut_add_data("Third", GINT_TO_POINTER(3), NULL);
}

static void
stub_error_in_data_setup_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, cut_message("always pass"));
}

void
test_error_in_data_setup (void)
{
    CutTestContainer *container;

    test_iterator =
        cut_test_iterator_new("error in data setup test iterator",
                              stub_error_in_data_setup_iterated_test,
                              stub_error_in_data_setup_iterated_data);
    container = CUT_TEST_CONTAINER(test_iterator);
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));
    cut_assert_false(run());
    cut_assert_equal_uint(0, cut_test_container_get_n_tests(container, NULL));

    cut_assert_n_signals(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 0, 0, 0, 0, 0, 0, 0, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
