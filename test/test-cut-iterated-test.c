#include <gcutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-runner.h>

#include "lib/cuttest-assertions.h"

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

#define cut_assert_n_signals(_n_assertions, _n_successes,               \
                             _n_failures, _n_errors, _n_pendings,       \
                             _n_notifications, _n_omissions) do         \
{                                                                       \
    GList *_result_summary;                                             \
    const GList *_expected_result_summary;                              \
    const GList *_actual_result_summary;                                \
                                                                        \
    _result_summary =                                                   \
        cuttest_result_summary_list_new(0,                              \
                                        (_n_assertions),                \
                                        (_n_successes),                 \
                                        (_n_failures),                  \
                                        (_n_errors),                    \
                                        (_n_pendings),                  \
                                        (_n_notifications),             \
                                        (_n_omissions));                \
    _expected_result_summary =                                          \
        cut_take_result_summary_list(_result_summary);                  \
                                                                        \
    _result_summary =                                                   \
        cuttest_result_summary_list_new(0,                              \
                                        n_pass_assertion_signals,       \
                                        n_success_signals,              \
                                        n_failure_signals,              \
                                        n_error_signals,                \
                                        n_pending_signals,              \
                                        n_notification_signals,         \
                                        n_omission_signals);            \
    _actual_result_summary =                                            \
        cut_take_result_summary_list(_result_summary);                  \
                                                                        \
    gcut_assert_equal_list_uint(_expected_result_summary,               \
                                _actual_result_summary);                \
} while (0)

void
setup (void)
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
teardown (void)
{
    if (test_data)
        g_object_unref(test_data);
    if (iterated_test)
        g_object_unref(iterated_test);
    g_object_unref(run_context);
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
    CutTestContext *original_test_context;
    CutTestContext *test_context;
    CutTest *test;

    test = CUT_TEST(iterated_test);
    connect_signals(test);
    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_set_data(test_context, test_data);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);
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
    iterated_test = cut_iterated_test_new("success test",
                                          stub_test_equal_value_string);
    test_data = cut_test_data_new("First data", g_strdup("value"), g_free);
    cut_assert_true(run());

    cut_assert_n_signals(1, 1, 0, 0, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 0, 1, 1, 0, 0, 0, 0, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
