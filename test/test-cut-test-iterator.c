#include <gcutter.h>
#include <cutter/cut-test-iterator.h>
#include <cutter/cut-test-runner.h>

#include "lib/cuttest-assertions.h"

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

#define PREPEND_GUINT(list, guint_value)                         \
    list = g_list_prepend(list,                                  \
                          GUINT_TO_POINTER(guint_value))

#define cut_assert_n_signals(_n_start_signals, _n_complete_signals,     \
                             _n_setup_calls, _n_teardown_calls,         \
                             _n_tests,                                  \
                             _n_assertions, _n_successes,               \
                             _n_failures, _n_errors, _n_pendings,       \
                             _n_notifications, _n_omissions) do         \
{                                                                       \
    GList *_result_summary;                                             \
    const GList *_expected_result_summary;                              \
    const GList *_actual_result_summary;                                \
    guint __n_start_signals, __n_complete_signals;                      \
    guint __n_setup_calls, __n_teardown_calls;                          \
                                                                        \
    __n_start_signals = (_n_start_signals);                             \
    __n_complete_signals = (_n_complete_signals);                       \
    __n_setup_calls = (_n_setup_calls);                                 \
    __n_teardown_calls = (_n_teardown_calls);                           \
                                                                        \
    _result_summary =                                                   \
        cuttest_result_summary_list_new((_n_tests),                     \
                                        (_n_assertions),                \
                                        (_n_successes),                 \
                                        (_n_failures),                  \
                                        (_n_errors),                    \
                                        (_n_pendings),                  \
                                        (_n_notifications),             \
                                        (_n_omissions));                \
    PREPEND_GUINT(_result_summary, __n_teardown_calls);                 \
    PREPEND_GUINT(_result_summary, __n_setup_calls);                    \
    PREPEND_GUINT(_result_summary, __n_complete_signals);               \
    PREPEND_GUINT(_result_summary, __n_start_signals);                  \
    _expected_result_summary =                                          \
        cut_take_result_summary_list(_result_summary);                  \
                                                                        \
    _result_summary =                                                   \
        cuttest_result_summary_list_new(n_tests,                        \
                                        n_pass_assertion_signals,       \
                                        n_success_signals,              \
                                        n_failure_signals,              \
                                        n_error_signals,                \
                                        n_pending_signals,              \
                                        n_notification_signals,         \
                                        n_omission_signals);            \
    PREPEND_GUINT(_result_summary, n_teardown_calls);                   \
    PREPEND_GUINT(_result_summary, n_setup_calls);                      \
    PREPEND_GUINT(_result_summary, n_complete_signals);                 \
    PREPEND_GUINT(_result_summary, n_start_signals);                    \
    _actual_result_summary =                                            \
        cut_take_result_summary_list(_result_summary);                  \
                                                                        \
    cut_assert_equal_g_list_uint(_expected_result_summary,              \
                                 _actual_result_summary);               \
} while (0)

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
setup (void)
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
                                  get_current_test_context,
                                  set_current_test_context,
                                  NULL, NULL);
    test_iterator = NULL;
}

void
teardown (void)
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
    CutTestContext *original_test_context;
    CutTestContext *test_context;
    CutTest *test;

    test = CUT_TEST(test_iterator);
    connect_signals(test);
    test_context = cut_test_context_new(NULL, test_case, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);
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
    cut_assert_true(TRUE, "always pass");
    cut_assert_equal_int(i, GPOINTER_TO_INT(data));
}

void
test_success (void)
{
    test_iterator = cut_test_iterator_new("success test iterator",
                                          stub_success_iterated_test,
                                          stub_iterated_data);
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));
    cut_assert_true(run());
    cut_assert_equal_uint(3, cut_test_iterator_get_n_tests(test_iterator));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 1, 0, 0, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 6, 3, 0, 0, 0, 0, 0);
}

static void
stub_failure_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, "always pass");
    cut_assert_equal_int(2, GPOINTER_TO_INT(data));
    cut_assert_true(TRUE, "always pass if come here");
}

void
test_failure (void)
{
    test_iterator = cut_test_iterator_new("failure test iterator",
                                          stub_failure_iterated_test,
                                          stub_iterated_data);
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));
    cut_assert_false(run());
    cut_assert_equal_uint(3, cut_test_iterator_get_n_tests(test_iterator));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 5, 1, 2, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "failure test iterator (First)",
                           NULL,
                           "<2 == GPOINTER_TO_INT(data)>\n"
                           "expected: <2>\n"
                           " but was: <1>",
                           "stub_failure_iterated_test");
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_SUCCESS,
                           "failure test iterator (Second)",
                           NULL, NULL, NULL);
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_FAILURE,
                           "failure test iterator (Third)",
                           NULL,
                           "<2 == GPOINTER_TO_INT(data)>\n"
                           "expected: <2>\n"
                           " but was: <3>",
                           "stub_failure_iterated_test");
}

static void
stub_error_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, "always pass");
    if (GPOINTER_TO_INT(data) == 2)
        cut_error("ERROR!");
    cut_assert_true(TRUE, "always pass if come here");
}

void
test_error (void)
{
    test_iterator = cut_test_iterator_new("error test iterator",
                                          stub_error_iterated_test,
                                          stub_iterated_data);
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));
    cut_assert_false(run());
    cut_assert_equal_uint(3, cut_test_iterator_get_n_tests(test_iterator));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 5, 2, 0, 1, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "error test iterator (First)",
                           NULL, NULL, NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_ERROR,
                           "error test iterator (Second)",
                           "ERROR!",
                           NULL,
                           "stub_error_iterated_test");
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "error test iterator (Third)",
                           NULL, NULL, NULL);
}

static void
stub_pending_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, "always pass");
    if (GPOINTER_TO_INT(data) == 2)
        cut_pend("PENDING!");
    cut_assert_true(TRUE, "always pass if come here");
}

void
test_pending (void)
{
    test_iterator = cut_test_iterator_new("pending test iterator",
                                          stub_pending_iterated_test,
                                          stub_iterated_data);
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));
    cut_assert_false(run());
    cut_assert_equal_uint(3, cut_test_iterator_get_n_tests(test_iterator));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 0, 1, 0, 0);
    cut_assert_test_result_summary(run_context, 3, 5, 2, 0, 0, 1, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "pending test iterator (First)",
                           NULL, NULL, NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_PENDING,
                           "pending test iterator (Second)",
                           "PENDING!",
                           NULL,
                           "stub_pending_iterated_test");
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "pending test iterator (Third)",
                           NULL, NULL, NULL);
}

static void
stub_notification_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, "always pass");
    if (GPOINTER_TO_INT(data) == 2)
        cut_notify("NOTIFICATION!");
    cut_assert_true(TRUE, "always pass if come here");
}

void
test_notification (void)
{
    test_iterator = cut_test_iterator_new("notification test iterator",
                                          stub_notification_iterated_test,
                                          stub_iterated_data);
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));
    cut_assert_true(run());
    cut_assert_equal_uint(3, cut_test_iterator_get_n_tests(test_iterator));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 0, 0, 1, 0);
    cut_assert_test_result_summary(run_context, 3, 6, 3, 0, 0, 0, 1, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "notification test iterator (First)",
                           NULL, NULL, NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_NOTIFICATION,
                           "notification test iterator (Second)",
                           "NOTIFICATION!",
                           NULL,
                           "stub_notification_iterated_test");
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "notification test iterator (Second)",
                           NULL, NULL, NULL);
    cut_assert_test_result(run_context, 3, CUT_TEST_RESULT_SUCCESS,
                           "notification test iterator (Third)",
                           NULL, NULL, NULL);
}

static void
stub_omission_iterated_test (gconstpointer data)
{
    cut_assert_true(TRUE, "always pass");
    if (GPOINTER_TO_INT(data) == 2)
        cut_omit("OMISSION!");
    cut_assert_true(TRUE, "always pass if come here");
}

void
test_omission (void)
{
    test_iterator = cut_test_iterator_new("omission test iterator",
                                          stub_omission_iterated_test,
                                          stub_iterated_data);
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));
    cut_assert_true(run());
    cut_assert_equal_uint(3, cut_test_iterator_get_n_tests(test_iterator));

    cut_assert_n_signals(1, 1, 3, 3, 3, 0, 0, 0, 0, 0, 0, 1);
    cut_assert_test_result_summary(run_context, 3, 5, 3, 0, 0, 0, 0, 1);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_SUCCESS,
                           "omission test iterator (First)",
                           NULL, NULL, NULL);
    cut_assert_test_result(run_context, 1, CUT_TEST_RESULT_OMISSION,
                           "omission test iterator (Second)",
                           "OMISSION!",
                           NULL,
                           "stub_omission_iterated_test");
    cut_assert_test_result(run_context, 2, CUT_TEST_RESULT_SUCCESS,
                           "omission test iterator (Second)",
                           NULL, NULL, NULL);
    cut_assert_test_result(run_context, 3, CUT_TEST_RESULT_SUCCESS,
                           "omission test iterator (Third)",
                           NULL, NULL, NULL);
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
    cut_assert_true(TRUE, "always pass");
}

void
test_error_in_data_setup (void)
{
    test_iterator =
        cut_test_iterator_new("error in data setup test iterator",
                              stub_error_in_data_setup_iterated_test,
                              stub_error_in_data_setup_iterated_data);
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));
    cut_assert_false(run());
    cut_assert_equal_uint(0, cut_test_iterator_get_n_tests(test_iterator));

    cut_assert_n_signals(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result_summary(run_context, 0, 0, 0, 0, 0, 0, 0, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
