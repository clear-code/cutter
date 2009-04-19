#include <gcutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-runner.h>

#include "../lib/cuttest-assertions.h"

void test_run(void *data);

static CutRunContext *run_context;
static CutIteratedTest *iterated_test;
static CutTestData *test_data;
static gboolean run_test_flag = FALSE;
static gint n_start_signals = 0;
static gint n_complete_signals = 0;
static gint n_success_signals = 0;
static gint n_failure_signals = 0;
static gint n_error_signals = 0;
static gint n_pending_signals = 0;
static gint n_pass_assertion_signals = 0;
static gint n_notification_signals = 0;
static gint n_omission_signals = 0;

static void
cut_assert_n_signals_helper(guint expected_n_assertions,
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
    APPEND(expected_n_assertions);
    APPEND(expected_n_successes);
    APPEND(expected_n_failures);
    APPEND(expected_n_errors);
    APPEND(expected_n_pendings);
    APPEND(expected_n_notifications);
    APPEND(expected_n_omissions);
    expected_result_summary = gcut_take_list(result_summary, NULL);

    result_summary = NULL;
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

#define cut_assert_n_signals(n_assertions, n_successes,                 \
                             n_failures, n_errors, n_pendings,          \
                             n_notifications, n_omissions)              \
    cut_trace_with_info_expression(                                     \
        cut_assert_n_signals_helper(n_assertions, n_successes,          \
                                    n_failures, n_errors,               \
                                    n_pendings, n_notifications,        \
                                    n_omissions),                       \
        cut_assert_n_signals(n_assertions, n_successes, n_failures,     \
                             n_errors, n_pendings, n_notifications,     \
                             n_omissions))

void
cut_setup (void)
{
    run_test_flag = FALSE;
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
    iterated_test = NULL;
    test_data = NULL;
}

void
cut_teardown (void)
{
    if (test_data)
        g_object_unref(test_data);
    if (iterated_test)
        g_object_unref(iterated_test);
    g_object_unref(run_context);
}

static void
cb_start_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_start_signals++;
}

static void
cb_complete_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_complete_signals++;
}

static void
cb_success_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_success_signals++;
}

static void
cb_failure_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_failure_signals++;
}

static void
cb_error_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_error_signals++;
}

static void
cb_pending_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_pending_signals++;
}

static void
cb_notification_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_notification_signals++;
}

static void
cb_pass_assertion_signal (CutTest *test, CutTestContext *test_context,
                          gpointer data)
{
    n_pass_assertion_signals++;
}

static void
cb_omission_signal (CutTest *test, CutTestContext *test_context, gpointer data)
{
    n_omission_signals++;
}

static void
connect_signals (CutTest *test)
{
#define CONNECT(name)                                                   \
    g_signal_connect(test, #name, G_CALLBACK(cb_ ## name ## _signal), NULL)

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

    test = CUT_TEST(iterated_test);
    connect_signals(test);
    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_set_data(test_context, test_data);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                       test, test_context);
    cut_test_context_current_pop();
    disconnect_signals(test);

    g_object_unref(test_context);

    return success;
}

static void
stub_test_equal_value_string (gconstpointer data)
{
    cut_assert_equal_string("value", data);
}

void
test_run (void *data)
{
    test_data = cut_test_data_new("First data", g_strdup("value"), g_free);
    iterated_test = cut_iterated_test_new("success test",
                                          stub_test_equal_value_string,
                                          test_data);
    cut_assert_true(run());

    cut_assert_n_signals(1, 1, 0, 0, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 1, 1, 1, 0, 0, 0, 0, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
