/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include <string.h>

#include "cut-run-context.h"
#include "cut-runner.h"
#include "cut-listener.h"
#include "cut-repository.h"
#include "cut-test-case.h"
#include "cut-test-result.h"

#include "cut-enum-types.h"
#include <gcutter/gcut-marshalers.h>

#if !GLIB_CHECK_VERSION(2, 18, 0)
#  define g_set_error_literal(error, domain, code, message) \
    g_set_error(error, domain, code, "%s", message)
#endif

#define CUT_RUN_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_RUN_CONTEXT, CutRunContextPrivate))

typedef struct _CutRunContextPrivate	CutRunContextPrivate;
struct _CutRunContextPrivate
{
    guint n_tests;
    guint n_assertions;
    guint n_successes;
    guint n_failures;
    guint n_errors;
    guint n_pendings;
    guint n_notifications;
    guint n_omissions;
    gdouble elapsed;
    GTimer *timer;
    GList *results;
    GList *reversed_results;
    gboolean use_multi_thread;
    gboolean is_multi_thread;
    gboolean max_threads;
    gboolean handle_signals;
    GMutex *mutex;
    gboolean crashed;
    gchar *backtrace;
    gchar *test_directory;
    gchar **exclude_files;
    gchar **exclude_directories;
    gchar *source_directory;
    gchar *log_directory;
    gchar **target_test_case_names;
    gchar **target_test_names;
    gboolean canceled;
    CutTestSuite *test_suite;
    GList *listeners;
    CutOrder test_case_order;
    gchar **command_line_args;
    gboolean completed;
    gboolean fatal_failures;
    gboolean keep_opening_modules;
    gboolean enable_convenience_attribute_definition;
    gboolean stop_before_test;
};

enum
{
    PROP_0,
    PROP_N_TESTS,
    PROP_N_ASSERTIONS,
    PROP_N_SUCCESSES,
    PROP_N_FAILURES,
    PROP_N_ERRORS,
    PROP_N_PENDINGS,
    PROP_N_NOTIFICATIONS,
    PROP_N_OMISSIONS,
    PROP_USE_MULTI_THREAD,
    PROP_IS_MULTI_THREAD,
    PROP_MAX_THREADS,
    PROP_HANDLE_SIGNALS,
    PROP_TEST_CASE_ORDER,
    PROP_TEST_DIRECTORY,
    PROP_SOURCE_DIRECTORY,
    PROP_LOG_DIRECTORY,
    PROP_TARGET_TEST_CASE_NAMES,
    PROP_TARGET_TEST_NAMES,
    PROP_EXCLUDE_FILES,
    PROP_EXCLUDE_DIRECTORIES,
    PROP_COMMAND_LINE_ARGS,
    PROP_FATAL_FAILURES,
    PROP_KEEP_OPENING_MODULES,
    PROP_ENABLE_CONVENIENCE_ATTRIBUTE_DEFINITION,
    PROP_STOP_BEFORE_TEST
};

enum
{
    START_SIGNAL,

    START_RUN,
    READY_TEST_SUITE,
    START_TEST_SUITE,
    READY_TEST_CASE,
    START_TEST_CASE,
    READY_TEST_ITERATOR,
    START_TEST_ITERATOR,
    START_TEST,
    START_ITERATED_TEST,

    PASS_ASSERTION,

    SUCCESS_TEST,
    FAILURE_TEST,
    ERROR_TEST,
    PENDING_TEST,
    NOTIFICATION_TEST,
    OMISSION_TEST,
    CRASH_TEST,

    SUCCESS_TEST_ITERATOR,
    FAILURE_TEST_ITERATOR,
    ERROR_TEST_ITERATOR,
    PENDING_TEST_ITERATOR,
    NOTIFICATION_TEST_ITERATOR,
    OMISSION_TEST_ITERATOR,
    CRASH_TEST_ITERATOR,

    SUCCESS_TEST_CASE,
    FAILURE_TEST_CASE,
    ERROR_TEST_CASE,
    PENDING_TEST_CASE,
    NOTIFICATION_TEST_CASE,
    OMISSION_TEST_CASE,
    CRASH_TEST_CASE,

    FAILURE_IN_TEST_CASE,
    ERROR_IN_TEST_CASE,
    PENDING_IN_TEST_CASE,
    NOTIFICATION_IN_TEST_CASE,
    OMISSION_IN_TEST_CASE,
    CRASH_IN_TEST_CASE,

    CRASH_TEST_SUITE,

    COMPLETE_ITERATED_TEST,
    COMPLETE_TEST,
    COMPLETE_TEST_ITERATOR,
    COMPLETE_TEST_CASE,
    COMPLETE_TEST_SUITE,
    COMPLETE_RUN,

    ERROR,

    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};
static CutRunnerIface *parent_runner_iface;

static GQuark detail_delegate;

static void runner_init (CutRunnerIface *iface);

G_DEFINE_ABSTRACT_TYPE_WITH_CODE(CutRunContext, cut_run_context, G_TYPE_OBJECT,
                                 G_IMPLEMENT_INTERFACE(CUT_TYPE_RUNNER,
                                                       runner_init));

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void start_run      (CutRunContext   *context);

static void start_iterated_test
                           (CutRunContext   *context,
                            CutIteratedTest *iterated_test,
                            CutTestContext  *test_context);
static void start_test     (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context);
static void pass_assertion (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context);
static void success_test   (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            CutTestResult   *result);
static void failure_test   (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            CutTestResult   *result);
static void error_test     (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            CutTestResult   *result);
static void pending_test   (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            CutTestResult   *result);
static void notification_test
                           (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            CutTestResult   *result);
static void omission_test  (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            CutTestResult   *result);
static void crash_test     (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            CutTestResult   *result);
static void complete_test  (CutRunContext   *context,
                            CutTest         *test,
                            CutTestContext  *test_context,
                            gboolean         success);
static void complete_iterated_test
                           (CutRunContext   *context,
                            CutIteratedTest *iterated_test,
                            CutTestContext  *test_context,
                            gboolean         success);

static void failure_in_test_case
                           (CutRunContext   *context,
                            CutTestCase     *test_case,
                            CutTestResult   *result);
static void error_in_test_case
                           (CutRunContext   *context,
                            CutTestCase     *test_case,
                            CutTestResult   *result);
static void pending_in_test_case
                           (CutRunContext   *context,
                            CutTestCase     *test_case,
                            CutTestResult   *result);
static void notification_in_test_case
                           (CutRunContext   *context,
                            CutTestCase     *test_case,
                            CutTestResult   *result);
static void omission_in_test_case
                           (CutRunContext   *context,
                            CutTestCase     *test_case,
                            CutTestResult   *result);
static void crash_in_test_case
                           (CutRunContext   *context,
                            CutTestCase     *test_case,
                            CutTestResult   *result);

static void crash_test_suite
                           (CutRunContext   *context,
                            CutTestSuite    *test_suite,
                            CutTestResult   *result);

static void complete_run   (CutRunContext   *context,
                            gboolean         success);

static gboolean runner_run (CutRunner *runner);

static void
cut_run_context_class_init (CutRunContextClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->start_run         = start_run;

    klass->start_iterated_test = start_iterated_test;
    klass->start_test        = start_test;
    klass->pass_assertion    = pass_assertion;
    klass->success_test      = success_test;
    klass->failure_test      = failure_test;
    klass->error_test        = error_test;
    klass->pending_test      = pending_test;
    klass->notification_test = notification_test;
    klass->omission_test     = omission_test;
    klass->crash_test        = crash_test;
    klass->complete_test     = complete_test;
    klass->complete_iterated_test = complete_iterated_test;

    klass->failure_in_test_case = failure_in_test_case;
    klass->error_in_test_case   = error_in_test_case;
    klass->pending_in_test_case = pending_in_test_case;
    klass->notification_in_test_case = notification_in_test_case;
    klass->omission_in_test_case = omission_in_test_case;
    klass->crash_in_test_case = crash_in_test_case;

    klass->crash_test_suite  = crash_test_suite;

    klass->complete_run      = complete_run;

    spec = g_param_spec_uint("n-tests",
                             "Number of tests",
                             "The number of tests of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_TESTS, spec);

    spec = g_param_spec_uint("n-assertions",
                             "Number of assertions",
                             "The number of assertions of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_ASSERTIONS, spec);

    spec = g_param_spec_uint("n-successes",
                             "Number of successes",
                             "The number of successes of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_SUCCESSES, spec);

    spec = g_param_spec_uint("n-failures",
                             "Number of failures",
                             "The number of failures of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_FAILURES, spec);

    spec = g_param_spec_uint("n-errors",
                             "Number of errors",
                             "The number of errors of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_ERRORS, spec);

    spec = g_param_spec_uint("n-pendings",
                             "Number of pendings",
                             "The number of pendings of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_PENDINGS, spec);

    spec = g_param_spec_uint("n-notifications",
                             "Number of notifications",
                             "The number of notifications of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_NOTIFICATIONS, spec);

    spec = g_param_spec_uint("n-omissions",
                             "Number of omissions",
                             "The number of omissions of the run context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_OMISSIONS, spec);

    spec = g_param_spec_boolean("use-multi-thread",
                                "Use multi thread",
                                "Whether use multi thread or not in the run context",
                                FALSE,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_USE_MULTI_THREAD, spec);

    spec = g_param_spec_boolean("is-multi-thread",
                                "Is multi thread?",
                                "Whether the run context is running tests with multi thread",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_IS_MULTI_THREAD, spec);

    spec = g_param_spec_int("max-threads",
                            "Max number of threads",
                            "How many threads are used concurrently at a maximum",
                            -1, G_MAXINT32, 10,
                            G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MAX_THREADS, spec);

    spec = g_param_spec_boolean("handle-signals",
                                "Whether handle signals",
                                "Whether the run context handles signals",
                                TRUE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HANDLE_SIGNALS, spec);

    spec = g_param_spec_enum("test-case-order",
                             "Test case order",
                             "Sort key for test case",
                             CUT_TYPE_ORDER,
                             CUT_ORDER_NONE_SPECIFIED,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_CASE_ORDER, spec);

    spec = g_param_spec_string("test-directory",
                               "Test directory",
                               "The directory name in which test cases are stored",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_DIRECTORY, spec);

    spec = g_param_spec_string("source-directory",
                               "Source directory",
                               "The directory name in which source files are stored",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SOURCE_DIRECTORY, spec);

    spec = g_param_spec_string("log-directory",
                               "Log directory",
                               "The directory name in which log files are stored",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LOG_DIRECTORY, spec);

    spec = g_param_spec_pointer("target-test-case-names",
                                "Test case names",
                                "The names of the target test case",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TARGET_TEST_CASE_NAMES, spec);

    spec = g_param_spec_pointer("target-test-names",
                                "Test names",
                                "The names of the target test",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TARGET_TEST_NAMES, spec);

    spec = g_param_spec_pointer("exclude-files",
                                "Exclude files",
                                "The file names of excluding from the target",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_EXCLUDE_FILES, spec);

    spec = g_param_spec_pointer("exclude-directories",
                                "Exclude directories",
                                "The directory names of excluding from target",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_EXCLUDE_DIRECTORIES, spec);

    spec = g_param_spec_pointer("command-line-args",
                                "Command line arguments",
                                "The argument strings from command line",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_COMMAND_LINE_ARGS, spec);

    spec = g_param_spec_boolean("fatal-failures",
                                "Fatal failures",
                                "Treat failures as fatal problem",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FATAL_FAILURES, spec);

    spec = g_param_spec_boolean("keep-opening-modules",
                                "Keep opening modules",
                                "Keep opening loaded modules to resolve symbols "
                                "for debugging",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_KEEP_OPENING_MODULES,
                                    spec);

    spec = g_param_spec_boolean("enable-convenience-attribute-definition",
                                "Enable convenience attribute definition",
                                "Enable convenience but dangerous "
                                "'#{ATTRIBUTE_NAME}_#{TEST_NAME - 'test_' PREFIX}' "
                                "attribute set function",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class,
                                    PROP_ENABLE_CONVENIENCE_ATTRIBUTE_DEFINITION,
                                    spec);

    spec = g_param_spec_boolean("stop-before-test",
                                "Stops before a test",
                                "Set breakpoint before invoking a test",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class,
                                    PROP_STOP_BEFORE_TEST,
                                    spec);

    signals[START_RUN]
        = g_signal_new("start-run",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, start_run),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__VOID,
                       G_TYPE_NONE, 0);

    signals[READY_TEST_SUITE]
        = g_signal_new ("ready-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, ready_test_suite),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_UINT_UINT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_SUITE, G_TYPE_UINT, G_TYPE_UINT);

    signals[START_TEST_SUITE]
        = g_signal_new ("start-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, start_test_suite),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_SUITE);

    signals[READY_TEST_CASE]
        = g_signal_new ("ready-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, ready_test_case),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_UINT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, G_TYPE_UINT);

    signals[START_TEST_CASE]
        = g_signal_new ("start-test-case",
                        G_TYPE_FROM_CLASS(klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET(CutRunContextClass, start_test_case),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

    signals[READY_TEST_ITERATOR]
        = g_signal_new ("ready-test-iterator",
                        G_TYPE_FROM_CLASS(klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET(CutRunContextClass, ready_test_iterator),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_UINT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST_ITERATOR, G_TYPE_UINT);

    signals[START_TEST_ITERATOR]
        = g_signal_new ("start-test-iterator",
                        G_TYPE_FROM_CLASS(klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET(CutRunContextClass, start_test_iterator),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_ITERATOR);

    signals[START_TEST]
        = g_signal_new ("start-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, start_test),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[START_ITERATED_TEST]
        = g_signal_new("start-iterated-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, start_iterated_test),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2,
                       CUT_TYPE_ITERATED_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[PASS_ASSERTION]
        = g_signal_new ("pass-assertion",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, pass_assertion),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[SUCCESS_TEST]
        = g_signal_new ("success-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, success_test),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[FAILURE_TEST]
        = g_signal_new ("failure-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, failure_test),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[ERROR_TEST]
        = g_signal_new ("error-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, error_test),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[PENDING_TEST]
        = g_signal_new ("pending-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, pending_test),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_TEST]
        = g_signal_new ("notification-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                        G_STRUCT_OFFSET (CutRunContextClass, notification_test),
                        NULL, NULL,
                        _gcut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[OMISSION_TEST]
        = g_signal_new("omission-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, omission_test),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                       G_TYPE_NONE, 3,
                       CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                       CUT_TYPE_TEST_RESULT);

    signals[CRASH_TEST]
        = g_signal_new("crash-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, crash_test),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                       G_TYPE_NONE, 3,
                       CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                       CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_ITERATED_TEST]
        = g_signal_new("complete-iterated-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass,
                                       complete_iterated_test),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT_BOOLEAN,
                       G_TYPE_NONE,
                       3, CUT_TYPE_ITERATED_TEST, CUT_TYPE_TEST_CONTEXT,
                       G_TYPE_BOOLEAN);

    signals[COMPLETE_TEST]
        = g_signal_new("complete-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, complete_test),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT_BOOLEAN,
                       G_TYPE_NONE,
                       3, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT, G_TYPE_BOOLEAN);

    signals[SUCCESS_TEST_ITERATOR]
        = g_signal_new("success-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, success_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE,
                       2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_RESULT);

    signals[FAILURE_TEST_ITERATOR]
        = g_signal_new("failure-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, failure_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE,
                       2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_RESULT);

    signals[ERROR_TEST_ITERATOR]
        = g_signal_new("error-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, error_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE,
                       2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_RESULT);

    signals[PENDING_TEST_ITERATOR]
        = g_signal_new("pending-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, pending_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE,
                       2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_TEST_ITERATOR]
        = g_signal_new("notification-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, notification_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE,
                       2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_RESULT);

    signals[OMISSION_TEST_ITERATOR]
        = g_signal_new("omission-test-iterator",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, omission_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_RESULT);

    signals[CRASH_TEST_ITERATOR]
        = g_signal_new("crash-test-iterator",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, crash_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_TEST_ITERATOR]
        = g_signal_new("complete-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass,
                                       complete_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_BOOLEAN,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_ITERATOR, G_TYPE_BOOLEAN);

    signals[SUCCESS_TEST_CASE]
        = g_signal_new("success-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, success_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[FAILURE_TEST_CASE]
        = g_signal_new("failure-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, failure_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[ERROR_TEST_CASE]
        = g_signal_new("error-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, error_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[PENDING_TEST_CASE]
        = g_signal_new("pending-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, pending_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_TEST_CASE]
        = g_signal_new("notification-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, notification_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[OMISSION_TEST_CASE]
        = g_signal_new("omission-test-case",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, omission_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[CRASH_TEST_CASE]
        = g_signal_new("crash-test-case",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, crash_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[FAILURE_IN_TEST_CASE]
        = g_signal_new("failure-in-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, failure_in_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[ERROR_IN_TEST_CASE]
        = g_signal_new("error-in-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, error_in_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[PENDING_IN_TEST_CASE]
        = g_signal_new("pending-in-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, pending_in_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_IN_TEST_CASE]
        = g_signal_new("notification-in-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass,
                                       notification_in_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[OMISSION_IN_TEST_CASE]
        = g_signal_new("omission-in-test-case",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass,
                                       omission_in_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[CRASH_IN_TEST_CASE]
        = g_signal_new("crash-in-test-case",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass,
                                       crash_in_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_TEST_CASE]
        = g_signal_new("complete-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, complete_test_case),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_BOOLEAN,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, G_TYPE_BOOLEAN);

    signals[CRASH_TEST_SUITE]
        = g_signal_new("crash-test-suite",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, crash_test_suite),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_SUITE, CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_TEST_SUITE]
        = g_signal_new("complete-test-suite",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, complete_test_suite),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_BOOLEAN,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_SUITE, G_TYPE_BOOLEAN);

    signals[COMPLETE_RUN]
        = g_signal_new("complete-run",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, complete_run),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__BOOLEAN,
                       G_TYPE_NONE, 1, G_TYPE_BOOLEAN);

    signals[ERROR]
        = g_signal_new("error",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                       G_STRUCT_OFFSET(CutRunContextClass, error),
                       NULL, NULL,
                        g_cclosure_marshal_VOID__POINTER,
                       G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_type_class_add_private(gobject_class, sizeof(CutRunContextPrivate));
}

static void
cut_run_context_init (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    priv->n_tests = 0;
    priv->n_assertions = 0;
    priv->n_successes = 0;
    priv->n_failures = 0;
    priv->n_errors = 0;
    priv->n_pendings = 0;
    priv->n_notifications = 0;
    priv->n_omissions = 0;
    priv->elapsed = 0.0;
    priv->timer = NULL;
    priv->results = NULL;
    priv->reversed_results = NULL;
    priv->use_multi_thread = FALSE;
    priv->is_multi_thread = FALSE;
    priv->max_threads = 10;
    priv->handle_signals = TRUE;
    priv->mutex = g_mutex_new();
    priv->crashed = FALSE;
    priv->test_directory = NULL;
    priv->source_directory = NULL;
    priv->log_directory = NULL;
    priv->exclude_files = NULL;
    priv->exclude_directories = NULL;
    priv->target_test_case_names = NULL;
    priv->target_test_names = NULL;
    priv->canceled = FALSE;
    priv->test_suite = NULL;
    priv->listeners = NULL;
    priv->command_line_args = NULL;
    priv->completed = FALSE;
    priv->fatal_failures = FALSE;
    priv->keep_opening_modules = FALSE;
    priv->enable_convenience_attribute_definition = FALSE;
    priv->stop_before_test = FALSE;
}

static void
remove_listener (CutListener *listener, CutRunContext *context)
{
    cut_listener_detach_from_run_context(listener, context);
    g_object_unref(listener);
}

static void
dispose (GObject *object)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(object);

    if (priv->timer) {
        g_timer_destroy(priv->timer);
        priv->timer = NULL;
    }

    if (priv->results) {
        g_list_foreach(priv->results, (GFunc)g_object_unref, NULL);
        g_list_free(priv->results);
        priv->results = NULL;
    }

    if (priv->reversed_results) {
        g_list_free(priv->reversed_results);
        priv->reversed_results = NULL;
    }

    if (priv->mutex) {
        g_mutex_free(priv->mutex);
        priv->mutex = NULL;
    }

    if (priv->test_suite) {
        g_object_unref(priv->test_suite);
        priv->test_suite = NULL;
    }

    if (priv->listeners) {
        g_list_foreach(priv->listeners,
                       (GFunc)remove_listener, CUT_RUN_CONTEXT(object));
        g_list_free(priv->listeners);
        priv->listeners = NULL;
    }

    g_free(priv->backtrace);
    priv->backtrace = NULL;

    g_free(priv->source_directory);
    priv->source_directory = NULL;

    g_free(priv->log_directory);
    priv->log_directory = NULL;

    g_free(priv->test_directory);
    priv->test_directory = NULL;

    g_strfreev(priv->exclude_files);
    priv->exclude_files = NULL;

    g_strfreev(priv->exclude_directories);
    priv->exclude_directories = NULL;

    g_strfreev(priv->target_test_case_names);
    priv->target_test_case_names = NULL;

    g_strfreev(priv->target_test_names);
    priv->target_test_names = NULL;

    g_strfreev(priv->command_line_args);
    priv->command_line_args = NULL;

    G_OBJECT_CLASS(cut_run_context_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_N_TESTS:
        priv->n_tests = g_value_get_uint(value);
        break;
      case PROP_N_ASSERTIONS:
        priv->n_assertions = g_value_get_uint(value);
        break;
      case PROP_N_SUCCESSES:
        priv->n_successes = g_value_get_uint(value);
        break;
      case PROP_N_FAILURES:
        priv->n_failures = g_value_get_uint(value);
        break;
      case PROP_N_ERRORS:
        priv->n_errors = g_value_get_uint(value);
        break;
      case PROP_N_PENDINGS:
        priv->n_pendings = g_value_get_uint(value);
        break;
      case PROP_N_NOTIFICATIONS:
        priv->n_notifications = g_value_get_uint(value);
        break;
      case PROP_N_OMISSIONS:
        priv->n_omissions = g_value_get_uint(value);
        break;
      case PROP_USE_MULTI_THREAD:
        priv->use_multi_thread = g_value_get_boolean(value);
        break;
      case PROP_IS_MULTI_THREAD:
        priv->is_multi_thread = g_value_get_boolean(value);
        break;
      case PROP_MAX_THREADS:
        priv->max_threads = g_value_get_int(value);
        break;
      case PROP_HANDLE_SIGNALS:
        priv->handle_signals = g_value_get_boolean(value);
        break;
      case PROP_TEST_CASE_ORDER:
        priv->test_case_order = g_value_get_enum(value);
        break;
      case PROP_TEST_DIRECTORY:
        priv->test_directory = g_value_dup_string(value);
        break;
      case PROP_SOURCE_DIRECTORY:
        priv->source_directory = g_value_dup_string(value);
        break;
      case PROP_LOG_DIRECTORY:
        priv->log_directory = g_value_dup_string(value);
        break;
      case PROP_TARGET_TEST_CASE_NAMES:
        priv->target_test_case_names = g_strdupv(g_value_get_pointer(value));
        break;
      case PROP_TARGET_TEST_NAMES:
        priv->target_test_names = g_strdupv(g_value_get_pointer(value));
        break;
      case PROP_EXCLUDE_FILES:
        priv->exclude_files = g_strdupv(g_value_get_pointer(value));
        break;
      case PROP_EXCLUDE_DIRECTORIES:
        priv->exclude_directories = g_strdupv(g_value_get_pointer(value));
        break;
      case PROP_COMMAND_LINE_ARGS:
        priv->command_line_args = g_strdupv(g_value_get_pointer(value));
        break;
      case PROP_FATAL_FAILURES:
        priv->fatal_failures = g_value_get_boolean(value);
        break;
      case PROP_KEEP_OPENING_MODULES:
        priv->keep_opening_modules = g_value_get_boolean(value);
        break;
      case PROP_ENABLE_CONVENIENCE_ATTRIBUTE_DEFINITION:
        priv->enable_convenience_attribute_definition = g_value_get_boolean(value);
        break;
      case PROP_STOP_BEFORE_TEST:
        priv->stop_before_test = g_value_get_boolean(value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_N_TESTS:
        g_value_set_uint(value, priv->n_tests);
        break;
      case PROP_N_ASSERTIONS:
        g_value_set_uint(value, priv->n_assertions);
        break;
      case PROP_N_SUCCESSES:
        g_value_set_uint(value, priv->n_successes);
        break;
      case PROP_N_FAILURES:
        g_value_set_uint(value, priv->n_failures);
        break;
      case PROP_N_ERRORS:
        g_value_set_uint(value, priv->n_errors);
        break;
      case PROP_N_PENDINGS:
        g_value_set_uint(value, priv->n_pendings);
        break;
      case PROP_N_NOTIFICATIONS:
        g_value_set_uint(value, priv->n_notifications);
        break;
      case PROP_N_OMISSIONS:
        g_value_set_uint(value, priv->n_omissions);
        break;
      case PROP_USE_MULTI_THREAD:
        g_value_set_boolean(value, priv->use_multi_thread);
        break;
      case PROP_IS_MULTI_THREAD:
        g_value_set_boolean(value, priv->is_multi_thread);
        break;
      case PROP_MAX_THREADS:
        g_value_set_int(value, priv->max_threads);
        break;
      case PROP_HANDLE_SIGNALS:
        g_value_set_boolean(value, priv->handle_signals);
        break;
      case PROP_TEST_CASE_ORDER:
        g_value_set_enum(value, priv->test_case_order);
        break;
      case PROP_TEST_DIRECTORY:
        g_value_set_string(value, priv->test_directory);
        break;
      case PROP_SOURCE_DIRECTORY:
        g_value_set_string(value, priv->source_directory);
        break;
      case PROP_LOG_DIRECTORY:
        g_value_set_string(value, priv->log_directory);
        break;
      case PROP_TARGET_TEST_CASE_NAMES:
        g_value_set_pointer(value, priv->target_test_case_names);
        break;
      case PROP_TARGET_TEST_NAMES:
        g_value_set_pointer(value, priv->target_test_names);
        break;
      case PROP_EXCLUDE_FILES:
        g_value_set_pointer(value, priv->exclude_files);
        break;
      case PROP_EXCLUDE_DIRECTORIES:
        g_value_set_pointer(value, priv->exclude_directories);
        break;
      case PROP_COMMAND_LINE_ARGS:
        g_value_set_pointer(value, priv->command_line_args);
        break;
      case PROP_FATAL_FAILURES:
        g_value_set_boolean(value, priv->fatal_failures);
        break;
      case PROP_KEEP_OPENING_MODULES:
        g_value_set_boolean(value, priv->keep_opening_modules);
        break;
      case PROP_ENABLE_CONVENIENCE_ATTRIBUTE_DEFINITION:
        g_value_set_boolean(value, priv->enable_convenience_attribute_definition);
        break;
      case PROP_STOP_BEFORE_TEST:
        g_value_set_boolean(value, priv->stop_before_test);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
runner_init (CutRunnerIface *iface)
{
    parent_runner_iface = g_type_interface_peek_parent(iface);
    iface->run = runner_run;
}

static gboolean
runner_run (CutRunner *runner)
{
    if (parent_runner_iface->run)
        return parent_runner_iface->run(runner);

    return FALSE;
}

static void
start_run (CutRunContext *context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->completed = FALSE;
    if (priv->timer)
        g_timer_destroy(priv->timer);
    priv->timer = g_timer_new();
    g_mutex_unlock(priv->mutex);
}

static void
start_test (CutRunContext   *context,
            CutTest         *test,
            CutTestContext  *test_context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->n_tests++;
    g_mutex_unlock(priv->mutex);
}

static void
start_iterated_test (CutRunContext   *context,
                     CutIteratedTest *iterated_test,
                     CutTestContext  *test_context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->n_tests++;
    g_mutex_unlock(priv->mutex);
}

static void
pass_assertion (CutRunContext   *context,
                CutTest         *test,
                CutTestContext  *test_context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->n_assertions++;
    g_mutex_unlock(priv->mutex);
}

static void
register_success_result (CutRunContext *context, CutTestResult *result)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_successes++;
    g_mutex_unlock(priv->mutex);
}

static void
register_failure_result (CutRunContext *context, CutTestResult *result)
{
    CutRunContextPrivate *priv= CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_failures++;
    g_mutex_unlock(priv->mutex);
}

static void
register_error_result (CutRunContext *context, CutTestResult *result)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_errors++;
    g_mutex_unlock(priv->mutex);
}

static void
register_pending_result (CutRunContext *context, CutTestResult *result)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_pendings++;
    g_mutex_unlock(priv->mutex);
}

static void
register_notification_result (CutRunContext  *context, CutTestResult *result)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_notifications++;
    g_mutex_unlock(priv->mutex);
}

static void
register_omission_result (CutRunContext *context, CutTestResult *result)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_omissions++;
    g_mutex_unlock(priv->mutex);
}

static void
register_crash_result (CutRunContext *context, CutTestResult *result)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    priv->crashed = TRUE;
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    g_mutex_unlock(priv->mutex);
}

static void
success_test (CutRunContext   *context,
              CutTest         *test,
              CutTestContext  *test_context,
              CutTestResult   *result)
{
    register_success_result(context, result);
}

static void
failure_test (CutRunContext   *context,
              CutTest         *test,
              CutTestContext  *test_context,
              CutTestResult   *result)
{
    register_failure_result(context, result);
}

static void
error_test (CutRunContext   *context,
            CutTest         *test,
            CutTestContext  *test_context,
            CutTestResult   *result)
{
    register_error_result(context, result);
}

static void
pending_test (CutRunContext   *context,
              CutTest         *test,
              CutTestContext  *test_context,
              CutTestResult   *result)
{
    register_pending_result(context, result);
}

static void
notification_test (CutRunContext   *context,
                   CutTest         *test,
                   CutTestContext  *test_context,
                   CutTestResult   *result)
{
    register_notification_result(context, result);
}

static void
omission_test (CutRunContext   *context,
               CutTest         *test,
               CutTestContext  *test_context,
               CutTestResult   *result)
{
    register_omission_result(context, result);
}

static void
crash_test (CutRunContext   *context,
            CutTest         *test,
            CutTestContext  *test_context,
            CutTestResult   *result)
{
    register_crash_result(context, result);
}

static void
failure_in_test_case (CutRunContext   *context,
                      CutTestCase     *test,
                      CutTestResult   *result)
{
    register_failure_result(context, result);
}

static void
error_in_test_case (CutRunContext   *context,
                    CutTestCase     *test,
                    CutTestResult   *result)
{
    register_error_result(context, result);
}

static void
pending_in_test_case (CutRunContext   *context,
                      CutTestCase     *test,
                      CutTestResult   *result)
{
    register_pending_result(context, result);
}

static void
notification_in_test_case (CutRunContext   *context,
                           CutTestCase     *test,
                           CutTestResult   *result)
{
    register_notification_result(context, result);
}

static void
omission_in_test_case (CutRunContext   *context,
                       CutTestCase     *test,
                       CutTestResult   *result)
{
    register_omission_result(context, result);
}

static void
crash_in_test_case (CutRunContext   *context,
                    CutTestCase     *test,
                    CutTestResult   *result)
{
    register_crash_result(context, result);
}

static void
crash_test_suite (CutRunContext   *context,
                  CutTestSuite    *test_suite,
                  CutTestResult   *result)
{
    register_crash_result(context, result);
}

static void
complete_iterated_test (CutRunContext   *context,
                        CutIteratedTest *iterated_test,
                        CutTestContext  *test_context,
                        gboolean         success)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->elapsed += cut_test_get_elapsed(CUT_TEST(iterated_test));
    g_mutex_unlock(priv->mutex);
}

static void
complete_test (CutRunContext   *context,
               CutTest         *test,
               CutTestContext  *test_context,
               gboolean         success)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->elapsed += cut_test_get_elapsed(test);
    g_mutex_unlock(priv->mutex);
}

static void
complete_run (CutRunContext *context, gboolean success)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    if (priv->timer)
        g_timer_stop(priv->timer);
    priv->completed = TRUE;
    g_mutex_unlock(priv->mutex);
}


void
cut_run_context_set_test_directory (CutRunContext *context,
                                    const gchar *directory)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_free(priv->test_directory);
    priv->test_directory = g_strdup(directory);
}

const gchar *
cut_run_context_get_test_directory (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->test_directory;
}

void
cut_run_context_set_source_directory (CutRunContext *context,
                                      const gchar *directory)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_free(priv->source_directory);
    priv->source_directory = g_strdup(directory);
}

const gchar *
cut_run_context_get_source_directory (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->source_directory;
}

void
cut_run_context_set_log_directory (CutRunContext *context,
                                   const gchar *directory)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_free(priv->log_directory);
    priv->log_directory = g_strdup(directory);
}

const gchar *
cut_run_context_get_log_directory (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->log_directory;
}

void
cut_run_context_set_multi_thread (CutRunContext *context,
                                  gboolean use_multi_thread)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_mutex_lock(priv->mutex);
    priv->use_multi_thread = use_multi_thread;
    if (use_multi_thread)
        priv->is_multi_thread = TRUE;
    g_mutex_unlock(priv->mutex);
}

gboolean
cut_run_context_get_multi_thread (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->use_multi_thread;
}

gboolean
cut_run_context_is_multi_thread (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->is_multi_thread;
}

void
cut_run_context_set_max_threads (CutRunContext *context, gint max_threads)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    priv->max_threads = max_threads;
}

gint
cut_run_context_get_max_threads (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->max_threads;
}

void
cut_run_context_set_handle_signals (CutRunContext *context,
                                    gboolean handle_signals)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->handle_signals = handle_signals;
}

gboolean
cut_run_context_get_handle_signals (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->handle_signals;
}

void
cut_run_context_set_exclude_files (CutRunContext *context, const gchar **files)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->exclude_files);
    priv->exclude_files = g_strdupv((gchar **)files);
}

const gchar **
cut_run_context_get_exclude_files (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return (const gchar **)priv->exclude_files;
}

void
cut_run_context_set_exclude_directories (CutRunContext *context,
                                         const gchar **directories)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->exclude_directories);
    priv->exclude_directories = g_strdupv((gchar **)directories);
}

const gchar **
cut_run_context_get_exclude_directories (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return (const gchar **)priv->exclude_directories;
}

void
cut_run_context_set_target_test_case_names (CutRunContext *context,
                                            const gchar **names)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->target_test_case_names);
    priv->target_test_case_names = g_strdupv((gchar **)names);
}

const gchar **
cut_run_context_get_target_test_case_names (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return (const gchar **)priv->target_test_case_names;
}

void
cut_run_context_set_target_test_names (CutRunContext *context,
                                       const gchar **names)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->target_test_names);
    priv->target_test_names = g_strdupv((gchar **)names);
}

const gchar **
cut_run_context_get_target_test_names (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return (const gchar **)priv->target_test_names;
}

guint
cut_run_context_get_n_tests (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_tests;
}

guint
cut_run_context_get_n_assertions (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_assertions;
}

guint
cut_run_context_get_n_successes (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_successes;
}

guint
cut_run_context_get_n_failures (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_failures;
}

guint
cut_run_context_get_n_errors (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_errors;
}

guint
cut_run_context_get_n_pendings (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_pendings;
}

guint
cut_run_context_get_n_notifications (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_notifications;
}

guint
cut_run_context_get_n_omissions (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->n_omissions;
}

CutTestResultStatus
cut_run_context_get_status (CutRunContext *context)
{
    CutRunContextPrivate *priv;
    CutTestResultStatus status;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    if (priv->crashed) {
        status = CUT_TEST_RESULT_CRASH;
    } else if (priv->n_errors > 0) {
        status = CUT_TEST_RESULT_ERROR;
    } else if (priv->n_failures > 0) {
        status = CUT_TEST_RESULT_FAILURE;
    } else if (priv->n_pendings > 0) {
        status = CUT_TEST_RESULT_PENDING;
    } else if (priv->n_omissions > 0) {
        status = CUT_TEST_RESULT_OMISSION;
    } else if (priv->n_notifications > 0) {
        status = CUT_TEST_RESULT_NOTIFICATION;
    } else {
        status = CUT_TEST_RESULT_SUCCESS;
    }

    return status;
}

gdouble
cut_run_context_get_elapsed (CutRunContext *context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    if (priv->timer)
        return g_timer_elapsed(priv->timer, NULL);
    else
        return 0.0;
}

gdouble
cut_run_context_get_total_elapsed (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->elapsed;
}

const GList *
cut_run_context_get_results (CutRunContext *context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    if (priv->reversed_results)
        g_list_free(priv->reversed_results);

    priv->reversed_results = g_list_reverse(g_list_copy(priv->results));
    return priv->reversed_results;
}

gboolean
cut_run_context_is_crashed (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->crashed;
}

void
cut_run_context_cancel (CutRunContext *context)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->canceled = TRUE;
}

gboolean
cut_run_context_is_canceled (CutRunContext *context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return priv->canceled || priv->crashed;
}

CutTestSuite *
cut_run_context_create_test_suite (CutRunContext *context)
{
    CutRunContextPrivate *priv;
    CutRepository *repository;
    CutTestSuite *suite;
    const gchar **exclude_files, **exclude_directories;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    repository = cut_repository_new(priv->test_directory);
    cut_repository_set_keep_opening_modules(repository,
                                            priv->keep_opening_modules);
    cut_repository_set_enable_convenience_attribute_definition(repository,
                                                               priv->enable_convenience_attribute_definition);
    exclude_files = (const gchar **)priv->exclude_files;
    cut_repository_set_exclude_files(repository, exclude_files);
    exclude_directories = (const gchar **)priv->exclude_directories;
    cut_repository_set_exclude_directories(repository, exclude_directories);
    suite = cut_repository_create_test_suite(repository);
    g_object_unref(repository);

    return suite;
}

CutTestSuite *
cut_run_context_get_test_suite (CutRunContext *context)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    if (!priv->test_suite)
        priv->test_suite = cut_run_context_create_test_suite(context);

    return priv->test_suite;
}

void
cut_run_context_set_test_suite (CutRunContext *context, CutTestSuite *suite)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    if (priv->test_suite)
        g_object_unref(priv->test_suite);

    if (suite)
        g_object_ref(suite);
    priv->test_suite = suite;
}

void
cut_run_context_add_listener (CutRunContext *context, CutListener *listener)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    priv->listeners = g_list_prepend(priv->listeners, g_object_ref(listener));
}

void
cut_run_context_remove_listener (CutRunContext *context, CutListener *listener)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    priv->listeners = g_list_remove(priv->listeners, listener);
    remove_listener(listener, context);
}

static void
attach_listener (CutListener *listener, CutRunContext *context)
{
    cut_listener_attach_to_run_context(listener, context);
}

void
cut_run_context_attach_listeners (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_list_foreach(priv->listeners, (GFunc)attach_listener, context);
}

static void
detach_listener (CutListener *listener, CutRunContext *context)
{
    cut_listener_detach_from_run_context(listener, context);
}

void
cut_run_context_detach_listeners (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_list_foreach(priv->listeners, (GFunc)detach_listener, context);
}

gchar *
cut_run_context_build_source_filename (CutRunContext *context,
                                       const gchar *filename)
{
    const gchar *source_directory;
    gchar *source_filename;

    source_directory = cut_run_context_get_source_directory(context);
    if (source_directory) {
        source_filename = g_build_filename(source_directory,
                                           filename,
                                           NULL);
    } else {
        source_filename = g_strdup(filename);
    }
    return source_filename;
}

void
cut_run_context_set_test_case_order (CutRunContext *context, CutOrder order)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->test_case_order = order;
    g_mutex_unlock(priv->mutex);
}

CutOrder
cut_run_context_get_test_case_order (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->test_case_order;
}

static gint
compare_test_cases_by_name (gconstpointer a, gconstpointer b, gpointer user_data)
{
    CutTestCase *test_case1, *test_case2;
    const gchar *test_case_name1, *test_case_name2;
    gboolean ascending;

    test_case1 = CUT_TEST_CASE(a);
    test_case2 = CUT_TEST_CASE(b);
    test_case_name1 = cut_test_get_name(CUT_TEST(test_case1));
    test_case_name2 = cut_test_get_name(CUT_TEST(test_case2));
    ascending = *(gboolean *)user_data;

    if (ascending)
        return strcmp(test_case_name1, test_case_name2);
    else
        return strcmp(test_case_name2, test_case_name1);
}

GList *
cut_run_context_sort_test_cases (CutRunContext *context, GList *test_cases)
{
    CutRunContextPrivate *priv;
    GList *sorted_test_cases = NULL;
    gboolean ascending;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    switch (priv->test_case_order) {
      case CUT_ORDER_NONE_SPECIFIED:
        sorted_test_cases = test_cases;
        break;
      case CUT_ORDER_NAME_ASCENDING:
        ascending = TRUE;
        sorted_test_cases = g_list_sort_with_data(test_cases,
                                                  compare_test_cases_by_name,
                                                  &ascending);
        break;
      case CUT_ORDER_NAME_DESCENDING:
        ascending = FALSE;
        sorted_test_cases = g_list_sort_with_data(test_cases,
                                                  compare_test_cases_by_name,
                                                  &ascending);
        break;
    }

    return sorted_test_cases;
}

gboolean
cut_run_context_start (CutRunContext *context)
{
    gboolean success;

    CUT_RUN_CONTEXT_GET_PRIVATE(context)->elapsed = 0.0;
    cut_run_context_attach_listeners(context);
    success = cut_runner_run(CUT_RUNNER(context));
    cut_run_context_detach_listeners(context);

    return success;
}

static void
cb_complete_run (CutRunContext *context, gboolean success, gpointer user_data)
{
    g_signal_handlers_disconnect_by_func(context,
                                         G_CALLBACK(cb_complete_run),
                                         user_data);
    cut_run_context_detach_listeners(context);
}

void
cut_run_context_start_async (CutRunContext *context)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->elapsed = 0.0;
    cut_run_context_attach_listeners(context);
    g_signal_connect(context, "complete-run", G_CALLBACK(cb_complete_run), NULL);
    cut_runner_run_async(CUT_RUNNER(context));
}

gboolean
cut_run_context_emit_complete_run (CutRunContext *context, gboolean success)
{
    CutTestResultStatus status;

    status = cut_run_context_get_status(context);
    if (cut_test_result_status_is_critical(status))
        success = FALSE;
    g_signal_emit_by_name(context, "complete-run", success);

    return success;
}

void
cut_run_context_emit_error (CutRunContext *context,
                            GQuark         domain,
                            gint           code,
                            GError        *sub_error,
                            const gchar   *format,
                            ...)
{
    GString *message;
    va_list var_args;
    GError *error = NULL;

    message = g_string_new(NULL);

    va_start(var_args, format);
    g_string_append_vprintf(message, format, var_args);
    va_end(var_args);

    if (sub_error) {
        g_string_append_printf(message,
                               ": %s:%d",
                               g_quark_to_string(sub_error->domain),
                               sub_error->code);
        if (sub_error->message)
            g_string_append_printf(message, ": %s", sub_error->message);

        g_error_free(sub_error);
    }

    g_set_error_literal(&error, domain, code, message->str);
    g_string_free(message, TRUE);
    g_signal_emit(context, signals[ERROR], 0, error);

    g_error_free(error);
}

void
cut_run_context_set_command_line_args (CutRunContext *context, gchar **args)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->command_line_args);
    priv->command_line_args = g_strdupv(args);
}

const gchar **
cut_run_context_get_command_line_args (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return (const gchar **)priv->command_line_args;
}

static void
cb_delegate_ready_test_case (CutRunContext *context, CutTestCase *test_case,
                             guint n_tests, gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[READY_TEST_CASE], detail_delegate,
                  test_case, n_tests);
}

static void
cb_delegate_start_test_case (CutRunContext *context, CutTestCase *test_case,
                             gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[START_TEST_CASE], detail_delegate,
                  test_case);
}

static void
cb_delegate_ready_test_iterator (CutRunContext *context,
                                 CutTestIterator *test_iterator,
                                 guint n_tests,
                                 gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[READY_TEST_ITERATOR], detail_delegate,
                  test_iterator, n_tests);
}

static void
cb_delegate_start_test_iterator (CutRunContext *context,
                                 CutTestIterator *test_iterator,
                                 gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[START_TEST_ITERATOR], detail_delegate,
                  test_iterator);
}


static void
cb_delegate_start_test (CutRunContext *context,
                        CutTest *test, CutTestContext *test_context,
                        gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[START_TEST], detail_delegate,
                  test, test_context);
}

static void
cb_delegate_start_iterated_test (CutRunContext *context,
                                 CutIteratedTest *iterated_test,
                                 CutTestContext *test_context,
                                 gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[START_ITERATED_TEST], detail_delegate,
                  iterated_test, test_context);
}


static void
cb_delegate_pass_assertion (CutRunContext *context,
                            CutTest *test,
                            CutTestContext *test_context,
                            gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[PASS_ASSERTION], detail_delegate,
                  test, test_context);
}

static void
cb_delegate_success_test (CutRunContext *context,
                          CutTest *test,
                          CutTestContext *test_context,
                          CutTestResult *result,
                          gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[SUCCESS_TEST], detail_delegate,
                  test, test_context, result);
}

static void
cb_delegate_failure_test (CutRunContext *context,
                          CutTest *test,
                          CutTestContext *test_context,
                          CutTestResult *result,
                          gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[FAILURE_TEST], detail_delegate,
                  test, test_context, result);
}

static void
cb_delegate_error_test (CutRunContext *context,
                        CutTest *test,
                        CutTestContext *test_context,
                        CutTestResult *result,
                        gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[ERROR_TEST], detail_delegate,
                  test, test_context, result);
}

static void
cb_delegate_pending_test (CutRunContext *context,
                          CutTest *test,
                          CutTestContext *test_context,
                          CutTestResult *result,
                          gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[PENDING_TEST], detail_delegate,
                  test, test_context, result);
}

static void
cb_delegate_notification_test (CutRunContext *context,
                               CutTest *test,
                               CutTestContext *test_context,
                               CutTestResult *result,
                               gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[NOTIFICATION_TEST], detail_delegate,
                  test, test_context, result);
}

static void
cb_delegate_omission_test (CutRunContext *context,
                           CutTest *test,
                           CutTestContext *test_context,
                           CutTestResult *result,
                           gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[OMISSION_TEST], detail_delegate,
                  test, test_context, result);
}

static void
cb_delegate_crash_test (CutRunContext *context,
                        CutTest *test,
                        CutTestContext *test_context,
                        CutTestResult *result,
                        gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[CRASH_TEST], detail_delegate,
                  test, test_context, result);
}


static void
cb_delegate_complete_test (CutRunContext *context,
                           CutTest *test,
                           CutTestContext *test_context,
                           gboolean success,
                           gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[COMPLETE_TEST], detail_delegate,
                  test, test_context, success);
}

static void
cb_delegate_complete_iterated_test (CutRunContext *context,
                                    CutIteratedTest *iterated_test,
                                    CutTestContext *test_context,
                                    gboolean success,
                                    gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[COMPLETE_ITERATED_TEST],
                  detail_delegate, iterated_test, test_context, success);
}

static void
cb_delegate_success_test_iterator (CutRunContext *context,
                                   CutTestIterator *test_iterator,
                                   CutTestResult *result,
                                   gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[SUCCESS_TEST_ITERATOR], detail_delegate,
                  test_iterator, result);
}

static void
cb_delegate_failure_test_iterator (CutRunContext *context,
                                   CutTestIterator *test_iterator,
                                   CutTestResult *result,
                                   gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[FAILURE_TEST_ITERATOR], detail_delegate,
                  test_iterator, result);
}

static void
cb_delegate_error_test_iterator (CutRunContext *context,
                                 CutTestIterator *test_iterator,
                                 CutTestResult *result,
                                 gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[ERROR_TEST_ITERATOR], detail_delegate,
                  test_iterator, result);
}

static void
cb_delegate_pending_test_iterator (CutRunContext *context,
                                   CutTestIterator *test_iterator,
                                   CutTestResult *result,
                                   gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[PENDING_TEST_ITERATOR], detail_delegate,
                  test_iterator, result);
}

static void
cb_delegate_notification_test_iterator (CutRunContext *context,
                                        CutTestIterator *test_iterator,
                                        CutTestResult *result,
                                        gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[NOTIFICATION_TEST_ITERATOR],
                  detail_delegate,
                  test_iterator, result);
}

static void
cb_delegate_omission_test_iterator (CutRunContext *context,
                                    CutTestIterator *test_iterator,
                                    CutTestResult *result,
                                    gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[OMISSION_TEST_ITERATOR],
                  detail_delegate,
                  test_iterator, result);
}

static void
cb_delegate_crash_test_iterator (CutRunContext *context,
                                 CutTestIterator *test_iterator,
                                 CutTestResult *result,
                                 gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[CRASH_TEST_ITERATOR],
                  detail_delegate,
                  test_iterator, result);
}

static void
cb_delegate_complete_test_iterator (CutRunContext *context,
                                    CutTestIterator *test_iterator,
                                    gboolean success,
                                    gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[COMPLETE_TEST_ITERATOR],
                  detail_delegate, test_iterator, success);
}

static void
cb_delegate_success_test_case (CutRunContext *context,
                               CutTestCase *test_case,
                               CutTestResult *result,
                               gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[SUCCESS_TEST_CASE], detail_delegate,
                  test_case, result);
}

static void
cb_delegate_failure_test_case (CutRunContext *context,
                               CutTestCase *test_case,
                               CutTestResult *result,
                               gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[FAILURE_TEST_CASE], detail_delegate,
                  test_case, result);
}

static void
cb_delegate_error_test_case (CutRunContext *context,
                             CutTestCase *test_case,
                             CutTestResult *result,
                             gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[ERROR_TEST_CASE], detail_delegate,
                  test_case, result);
}

static void
cb_delegate_pending_test_case (CutRunContext *context,
                               CutTestCase *test_case,
                               CutTestResult *result,
                               gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[PENDING_TEST_CASE], detail_delegate,
                  test_case, result);
}

static void
cb_delegate_notification_test_case (CutRunContext *context,
                                    CutTestCase *test_case,
                                    CutTestResult *result,
                                    gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[NOTIFICATION_TEST_CASE],
                  detail_delegate,
                  test_case, result);
}

static void
cb_delegate_omission_test_case (CutRunContext *context,
                                CutTestCase *test_case,
                                CutTestResult *result,
                                gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[OMISSION_TEST_CASE], detail_delegate,
                  test_case, result);
}

static void
cb_delegate_crash_test_case (CutRunContext *context,
                             CutTestCase *test_case,
                             CutTestResult *result,
                             gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[CRASH_TEST_CASE], detail_delegate,
                  test_case, result);
}

static void
cb_delegate_complete_test_case (CutRunContext *context,
                                CutTestCase *test_case,
                                gboolean success,
                                gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[COMPLETE_TEST_CASE], detail_delegate,
                  test_case, success);
}


static void
cb_delegate_error (CutRunContext *context, GError *error, gpointer user_data)
{
    CutRunContext *other_context = user_data;
    g_signal_emit(other_context, signals[ERROR], detail_delegate,
                  error);
}


static void
cb_delegate_complete_run (CutRunContext *context, gboolean success,
                          gpointer user_data)
{
    CutRunContext *other_context = user_data;
#define DISCONNECT_DELEGATE_SIGNAL(name)                                \
    g_signal_handlers_disconnect_by_func(context,                       \
                                         cb_delegate_ ## name,          \
                                         other_context)

    DISCONNECT_DELEGATE_SIGNAL(ready_test_case);
    DISCONNECT_DELEGATE_SIGNAL(start_test_case);
    DISCONNECT_DELEGATE_SIGNAL(ready_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(start_test_iterator);

    DISCONNECT_DELEGATE_SIGNAL(start_test);
    DISCONNECT_DELEGATE_SIGNAL(start_iterated_test);

    DISCONNECT_DELEGATE_SIGNAL(pass_assertion);
    DISCONNECT_DELEGATE_SIGNAL(success_test);
    DISCONNECT_DELEGATE_SIGNAL(failure_test);
    DISCONNECT_DELEGATE_SIGNAL(error_test);
    DISCONNECT_DELEGATE_SIGNAL(pending_test);
    DISCONNECT_DELEGATE_SIGNAL(notification_test);
    DISCONNECT_DELEGATE_SIGNAL(omission_test);
    DISCONNECT_DELEGATE_SIGNAL(crash_test);

    DISCONNECT_DELEGATE_SIGNAL(complete_test);
    DISCONNECT_DELEGATE_SIGNAL(complete_iterated_test);

    DISCONNECT_DELEGATE_SIGNAL(success_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(failure_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(error_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(pending_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(notification_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(omission_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(crash_test_iterator);
    DISCONNECT_DELEGATE_SIGNAL(complete_test_iterator);

    DISCONNECT_DELEGATE_SIGNAL(success_test_case);
    DISCONNECT_DELEGATE_SIGNAL(failure_test_case);
    DISCONNECT_DELEGATE_SIGNAL(error_test_case);
    DISCONNECT_DELEGATE_SIGNAL(pending_test_case);
    DISCONNECT_DELEGATE_SIGNAL(notification_test_case);
    DISCONNECT_DELEGATE_SIGNAL(omission_test_case);
    DISCONNECT_DELEGATE_SIGNAL(crash_test_case);
    DISCONNECT_DELEGATE_SIGNAL(complete_test_case);

    DISCONNECT_DELEGATE_SIGNAL(error);

    DISCONNECT_DELEGATE_SIGNAL(complete_run);

#undef DISCONNECT_DELEGATE_SIGNAL
}


void
cut_run_context_delegate_signals (CutRunContext *context,
                                  CutRunContext *other_context)
{
    detail_delegate = g_quark_from_static_string("delegate");

#define CONNECT_DELEGATE_SIGNAL(name)                                   \
    g_signal_connect(context, #name,                                    \
                     G_CALLBACK(cb_delegate_ ## name),                  \
                     other_context)

    CONNECT_DELEGATE_SIGNAL(ready_test_case);
    CONNECT_DELEGATE_SIGNAL(start_test_case);
    CONNECT_DELEGATE_SIGNAL(ready_test_iterator);
    CONNECT_DELEGATE_SIGNAL(start_test_iterator);

    CONNECT_DELEGATE_SIGNAL(start_test);
    CONNECT_DELEGATE_SIGNAL(start_iterated_test);

    CONNECT_DELEGATE_SIGNAL(pass_assertion);
    CONNECT_DELEGATE_SIGNAL(success_test);
    CONNECT_DELEGATE_SIGNAL(failure_test);
    CONNECT_DELEGATE_SIGNAL(error_test);
    CONNECT_DELEGATE_SIGNAL(pending_test);
    CONNECT_DELEGATE_SIGNAL(notification_test);
    CONNECT_DELEGATE_SIGNAL(omission_test);
    CONNECT_DELEGATE_SIGNAL(crash_test);

    CONNECT_DELEGATE_SIGNAL(complete_test);
    CONNECT_DELEGATE_SIGNAL(complete_iterated_test);

    CONNECT_DELEGATE_SIGNAL(success_test_iterator);
    CONNECT_DELEGATE_SIGNAL(failure_test_iterator);
    CONNECT_DELEGATE_SIGNAL(error_test_iterator);
    CONNECT_DELEGATE_SIGNAL(pending_test_iterator);
    CONNECT_DELEGATE_SIGNAL(notification_test_iterator);
    CONNECT_DELEGATE_SIGNAL(omission_test_iterator);
    CONNECT_DELEGATE_SIGNAL(crash_test_iterator);
    CONNECT_DELEGATE_SIGNAL(complete_test_iterator);

    CONNECT_DELEGATE_SIGNAL(success_test_case);
    CONNECT_DELEGATE_SIGNAL(failure_test_case);
    CONNECT_DELEGATE_SIGNAL(error_test_case);
    CONNECT_DELEGATE_SIGNAL(pending_test_case);
    CONNECT_DELEGATE_SIGNAL(notification_test_case);
    CONNECT_DELEGATE_SIGNAL(omission_test_case);
    CONNECT_DELEGATE_SIGNAL(crash_test_case);
    CONNECT_DELEGATE_SIGNAL(complete_test_case);

    CONNECT_DELEGATE_SIGNAL(error);

    CONNECT_DELEGATE_SIGNAL(complete_run);
#undef CONNECT_DELEGATE_SIGNAL
}

gboolean
cut_run_context_is_completed (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->completed;
}

void
cut_run_context_set_fatal_failures (CutRunContext *context,
                                    gboolean       fatal_failures)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->fatal_failures = fatal_failures;
}

gboolean
cut_run_context_get_fatal_failures (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->fatal_failures;
}

void
cut_run_context_set_keep_opening_modules (CutRunContext *context,
                                          gboolean       keep_opening)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->keep_opening_modules = keep_opening;
}

gboolean
cut_run_context_get_keep_opening_modules (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->keep_opening_modules;
}

void
cut_run_context_set_enable_convenience_attribute_definition (CutRunContext *context,
                                                             gboolean       enable_convenience_attribute_definition)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->enable_convenience_attribute_definition =
        enable_convenience_attribute_definition;
}

gboolean
cut_run_context_get_enable_convenience_attribute_definition (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->enable_convenience_attribute_definition;
}

void
cut_run_context_set_stop_before_test (CutRunContext *context,
                                      gboolean       stop)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->stop_before_test = stop;
}

gboolean
cut_run_context_get_stop_before_test (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->stop_before_test;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
