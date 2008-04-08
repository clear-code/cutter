/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include "cut-runner.h"
#include "cut-repository.h"
#include "cut-test-case.h"
#include "cut-test-result.h"

#include "cut-marshalers.h"
#include "cut-enum-types.h"

#define CUT_RUNNER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_RUNNER, CutRunnerPrivate))

typedef struct _CutRunnerPrivate	CutRunnerPrivate;
struct _CutRunnerPrivate
{
    guint n_tests;
    guint n_assertions;
    guint n_failures;
    guint n_errors;
    guint n_pendings;
    guint n_notifications;
    guint n_omissions;
    GList *results;
    gboolean use_multi_thread;
    gboolean is_multi_thread;
    GMutex *mutex;
    gboolean crashed;
    gchar *stack_trace;
    gchar *test_directory;
    gchar *source_directory;
    gchar **target_test_case_names;
    gchar **target_test_names;
    gboolean canceled;
    CutTestSuite *test_suite;
    GList *listeners;
    CutOrder test_case_order;
};

enum
{
    PROP_0,
    PROP_N_TESTS,
    PROP_N_ASSERTIONS,
    PROP_N_FAILURES,
    PROP_N_ERRORS,
    PROP_N_PENDINGS,
    PROP_N_NOTIFICATIONS,
    PROP_N_OMISSIONS,
    PROP_USE_MULTI_THREAD,
    PROP_IS_MULTI_THREAD,
    PROP_TEST_CASE_ORDER
};

enum
{
    START_SIGNAL,

    READY_TEST_SUITE,
    START_TEST_SUITE,
    READY_TEST_CASE,
    START_TEST_CASE,
    START_TEST,

    PASS_TEST,
    SUCCESS_TEST,
    FAILURE_TEST,
    ERROR_TEST,
    PENDING_TEST,
    NOTIFICATION_TEST,
    OMISSION_TEST,

    SUCCESS_TEST_CASE,
    FAILURE_TEST_CASE,
    ERROR_TEST_CASE,
    PENDING_TEST_CASE,
    NOTIFICATION_TEST_CASE,
    OMISSION_TEST_CASE,

    COMPLETE_TEST,
    COMPLETE_TEST_CASE,
    COMPLETE_TEST_SUITE,

    CRASHED,

    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (CutRunner, cut_runner, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
cut_runner_class_init (CutRunnerClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_uint("n-tests",
                             "Number of tests",
                             "The number of tests of the runner",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_TESTS, spec);

    spec = g_param_spec_uint("n-assertions",
                             "Number of assertions",
                             "The number of assertions of the runner",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_ASSERTIONS, spec);

    spec = g_param_spec_uint("n-failures",
                             "Number of failures",
                             "The number of failures of the runner",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_FAILURES, spec);

    spec = g_param_spec_uint("n-errors",
                             "Number of errors",
                             "The number of errors of the runner",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_ERRORS, spec);

    spec = g_param_spec_uint("n-pendings",
                             "Number of pendings",
                             "The number of pendings of the runner",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_PENDINGS, spec);

    spec = g_param_spec_uint("n-notifications",
                             "Number of notifications",
                             "The number of notifications of the runner",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_NOTIFICATIONS, spec);

    spec = g_param_spec_uint("n-omissions",
                             "Number of omissions",
                             "The number of omissions of the runner",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_OMISSIONS, spec);

    spec = g_param_spec_boolean("use-multi-thread",
                                "Use multi thread",
                                "Whether use multi thread or not in the runner",
                                FALSE,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_USE_MULTI_THREAD, spec);

    spec = g_param_spec_boolean("is-multi-thread",
                                "Is multi thread?",
                                "Whether the runner is running tests with multi thread",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_IS_MULTI_THREAD, spec);

    spec = g_param_spec_enum("test-case-order",
                             "Test case order",
                             "Sort key for test case",
                             CUT_TYPE_ORDER,
                             CUT_ORDER_NONE_SPECIFIED,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_CASE_ORDER, spec);


    signals[READY_TEST_SUITE]
        = g_signal_new ("ready-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, ready_test_suite),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_UINT_UINT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_SUITE, G_TYPE_UINT, G_TYPE_UINT);

    signals[START_TEST_SUITE]
        = g_signal_new ("start-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, start_test_suite),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_SUITE);

    signals[READY_TEST_CASE]
        = g_signal_new ("ready-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, ready_test_case),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_UINT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, G_TYPE_UINT);

    signals[START_TEST_CASE]
        = g_signal_new ("start-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, start_test_case),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

    signals[START_TEST]
        = g_signal_new ("start-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, start_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[PASS_TEST]
        = g_signal_new ("pass-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, pass_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[SUCCESS_TEST]
        = g_signal_new ("success-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, success_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[FAILURE_TEST]
        = g_signal_new ("failure-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, failure_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[ERROR_TEST]
        = g_signal_new ("error-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, error_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[PENDING_TEST]
        = g_signal_new ("pending-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, pending_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_TEST]
        = g_signal_new ("notification-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, notification_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[OMISSION_TEST]
        = g_signal_new("omission-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunnerClass, omission_test),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                       G_TYPE_NONE, 3,
                       CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                       CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_TEST]
        = g_signal_new ("complete-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, complete_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[SUCCESS_TEST_CASE]
        = g_signal_new ("success-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, success_test_case),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[FAILURE_TEST_CASE]
        = g_signal_new ("failure-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, failure_test_case),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[ERROR_TEST_CASE]
        = g_signal_new ("error-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, error_test_case),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[PENDING_TEST_CASE]
        = g_signal_new ("pending-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, pending_test_case),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_TEST_CASE]
        = g_signal_new ("notification-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, notification_test_case),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[OMISSION_TEST_CASE]
        = g_signal_new("omission-test-case",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunnerClass, omission_test_case),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                       G_TYPE_NONE, 3,
                       CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_CONTEXT,
                       CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_TEST_CASE]
        = g_signal_new ("complete-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, complete_test_case),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

    signals[COMPLETE_TEST_SUITE]
        = g_signal_new ("complete-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, complete_test_suite),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_SUITE);

    signals[CRASHED]
        = g_signal_new ("crashed",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunnerClass, crashed),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__STRING,
                        G_TYPE_NONE, 1, G_TYPE_STRING);

    g_type_class_add_private(gobject_class, sizeof(CutRunnerPrivate));
}

static void
cut_runner_init (CutRunner *runner)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    priv->n_tests = 0;
    priv->n_assertions = 0;
    priv->n_failures = 0;
    priv->n_errors = 0;
    priv->n_pendings = 0;
    priv->n_notifications = 0;
    priv->n_omissions = 0;
    priv->results = NULL;
    priv->use_multi_thread = FALSE;
    priv->is_multi_thread = FALSE;
    priv->mutex = g_mutex_new();
    priv->crashed = FALSE;
    priv->stack_trace = NULL;
    priv->test_directory = NULL;
    priv->source_directory = NULL;
    priv->target_test_case_names = NULL;
    priv->target_test_names = NULL;
    priv->canceled = FALSE;
    priv->test_suite = NULL;
    priv->listeners = NULL;
}

static void
remove_listener (CutListener *listener, CutRunner *runner)
{
    cut_listener_detach_from_runner(listener, runner);
}

static void
dispose (GObject *object)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(object);

    if (priv->results) {
        g_list_foreach(priv->results, (GFunc)g_object_unref, NULL);
        g_list_free(priv->results);
        priv->results = NULL;
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
        g_list_foreach(priv->listeners, (GFunc)remove_listener, CUT_RUNNER(object));
        g_list_free(priv->listeners);
        priv->listeners = NULL;
    }

    g_free(priv->stack_trace);
    priv->stack_trace = NULL;

    g_free(priv->source_directory);
    priv->source_directory = NULL;

    g_strfreev(priv->target_test_case_names);
    priv->target_test_case_names = NULL;

    g_strfreev(priv->target_test_names);
    priv->target_test_names = NULL;

    G_OBJECT_CLASS(cut_runner_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_N_TESTS:
        priv->n_tests = g_value_get_uint(value);
        break;
      case PROP_N_ASSERTIONS:
        priv->n_assertions = g_value_get_uint(value);
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
      case PROP_TEST_CASE_ORDER:
        priv->test_case_order = g_value_get_enum(value);
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
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_N_TESTS:
        g_value_set_uint(value, priv->n_tests);
        break;
      case PROP_N_ASSERTIONS:
        g_value_set_uint(value, priv->n_assertions);
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
      case PROP_TEST_CASE_ORDER:
        g_value_set_enum(value, priv->test_case_order);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutRunner *
cut_runner_new (void)
{
    return g_object_new(CUT_TYPE_RUNNER, NULL);
}

CutRunner *
cut_runner_copy (CutRunner *runner)
{
    CutRunnerPrivate *priv, *copied_priv;
    CutRunner *copied;

    copied = cut_runner_new();

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    copied_priv = CUT_RUNNER_GET_PRIVATE(copied);

    copied_priv->use_multi_thread = priv->use_multi_thread;
    copied_priv->is_multi_thread = priv->is_multi_thread;

    copied_priv->test_directory = g_strdup(priv->test_directory);
    copied_priv->source_directory = g_strdup(priv->source_directory);

    copied_priv->target_test_case_names =
        g_strdupv(priv->target_test_case_names);
    copied_priv->target_test_names = g_strdupv(priv->target_test_names);

    return copied;
}

void
cut_runner_set_test_directory (CutRunner *runner, const gchar *directory)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    g_free(priv->test_directory);
    priv->test_directory = g_strdup(directory);
}

const gchar *
cut_runner_get_test_directory (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->test_directory;
}

void
cut_runner_set_source_directory (CutRunner *runner, const gchar *directory)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    g_free(priv->source_directory);
    priv->source_directory = g_strdup(directory);
}

const gchar *
cut_runner_get_source_directory (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->source_directory;
}

void
cut_runner_set_multi_thread (CutRunner *runner, gboolean use_multi_thread)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    g_mutex_lock(priv->mutex);
    priv->use_multi_thread = use_multi_thread;
    if (use_multi_thread)
        priv->is_multi_thread = TRUE;
    g_mutex_unlock(priv->mutex);
}

gboolean
cut_runner_get_multi_thread (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->use_multi_thread;
}

gboolean
cut_runner_is_multi_thread (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->is_multi_thread;
}

void
cut_runner_set_target_test_case_names (CutRunner *runner,
                                        const gchar **names)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    g_strfreev(priv->target_test_case_names);
    priv->target_test_case_names = g_strdupv((gchar **)names);
}

const gchar **
cut_runner_get_target_test_case_names (CutRunner *runner)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);
    return (const gchar **)(priv->target_test_case_names);
}

void
cut_runner_set_target_test_names (CutRunner *runner,
                                   const gchar **names)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    g_strfreev(priv->target_test_names);
    priv->target_test_names = g_strdupv((gchar **)names);
}

const gchar **
cut_runner_get_target_test_names (CutRunner *runner)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);
    return (const gchar **)(priv->target_test_names);
}

static void
cb_pass_assertion (CutTest *test, CutTestContext *test_context, gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->n_assertions++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(runner, signals[PASS_TEST], 0, test, test_context);
}

static void
cb_success (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    g_mutex_unlock(priv->mutex);

    g_signal_emit(runner, signals[SUCCESS_TEST], 0, test, test_context, result);
}

static void
cb_failure (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_failures++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(runner, signals[FAILURE_TEST], 0, test, test_context, result);
}

static void
cb_error (CutTest *test, CutTestContext *test_context, CutTestResult *result,
          gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_errors++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(runner, signals[ERROR_TEST], 0, test, test_context, result);
}

static void
cb_pending (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_pendings++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(runner, signals[PENDING_TEST], 0, test, test_context, result);
}

static void
cb_notification (CutTest *test, CutTestContext *test_context,
                 CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_notifications++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(runner, signals[NOTIFICATION_TEST], 0, test, test_context, result);
}

static void
cb_omission (CutTest *test, CutTestContext *test_context,
             CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_omissions++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(runner, signals[OMISSION_TEST], 0, test, test_context, result);
}

static void
cb_complete (CutTest *test, gpointer data)
{
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_pass_assertion),
                                         data);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_success),
                                         data);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_failure),
                                         data);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_error),
                                         data);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_pending),
                                         data);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_notification),
                                         data);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_omission),
                                         data);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_complete),
                                         data);
}

void
cut_runner_start_test (CutRunner *runner, CutTest *test)
{
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->n_tests++;
    g_mutex_unlock(priv->mutex);

    g_signal_connect(test, "pass_assertion",
                     G_CALLBACK(cb_pass_assertion), runner);
    g_signal_connect(test, "success", G_CALLBACK(cb_success), runner);
    g_signal_connect(test, "failure", G_CALLBACK(cb_failure), runner);
    g_signal_connect(test, "error", G_CALLBACK(cb_error), runner);
    g_signal_connect(test, "pending", G_CALLBACK(cb_pending), runner);
    g_signal_connect(test, "notification", G_CALLBACK(cb_notification), runner);
    g_signal_connect(test, "omission", G_CALLBACK(cb_omission), runner);
    g_signal_connect(test, "complete", G_CALLBACK(cb_complete), runner);
}

static void
cb_start_test (CutTestCase *test_case, CutTest *test,
               CutTestContext *test_context, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[START_TEST], 0, test, test_context);
}

static void
cb_complete_test(CutTestCase *test_case, CutTest *test,
                 CutTestContext *test_context, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[COMPLETE_TEST], 0, test, test_context);
}

static void
cb_success_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[SUCCESS_TEST_CASE], 0,
                  test_case, test_context, result);
}

static void
cb_failure_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[FAILURE_TEST_CASE], 0,
                  test_case, test_context, result);
}

static void
cb_error_test_case (CutTestCase *test_case, CutTestContext *test_context,
                    CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[ERROR_TEST_CASE], 0,
                  test_case, test_context, result);
}

static void
cb_pending_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[PENDING_TEST_CASE], 0,
                  test_case, test_context, result);
}

static void
cb_notification_test_case (CutTestCase *test_case, CutTestContext *test_context,
                           CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[NOTIFICATION_TEST_CASE], 0,
                  test_case, test_context, result);
}

static void
cb_omission_test_case (CutTestCase *test_case, CutTestContext *test_context,
                       CutTestResult *result, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[OMISSION_TEST_CASE], 0,
                  test_case, test_context, result);
}

static void
cb_start_test_case(CutTestCase *test_case, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[START_TEST_CASE], 0, test_case);
}

static void
cb_ready_test_case(CutTestCase *test_case, guint n_tests, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[READY_TEST_CASE], 0, test_case, n_tests);
}

static void
cb_complete_test_case(CutTestCase *test_case, gpointer data)
{
    CutRunner *runner = data;

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_case,                     \
                                         G_CALLBACK(cb_ ## name),       \
                                         data)

    DISCONNECT(start_test);
    DISCONNECT(complete_test);
#undef DISCONNECT

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_case,                     \
                                         G_CALLBACK(cb_ ## name ## _test_case), \
                                         data)

    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);
    DISCONNECT(omission);

    DISCONNECT(ready);
    DISCONNECT(start);
    DISCONNECT(complete);
#undef DISCONNECT

    g_signal_emit(runner, signals[COMPLETE_TEST_CASE], 0, test_case);
}

void
cut_runner_start_test_case (CutRunner *runner, CutTestCase *test_case)
{
#define CONNECT(name) \
    g_signal_connect(test_case, #name, G_CALLBACK(cb_ ## name), runner)

    CONNECT(start_test);
    CONNECT(complete_test);
#undef CONNECT

#define CONNECT(name)                                                   \
    g_signal_connect(test_case, #name,                                  \
                     G_CALLBACK(cb_ ## name ## _test_case), runner)

    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(omission);

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
#undef CONNECT
}

static void
cb_ready_test_suite(CutTestSuite *test_suite, guint n_test_cases,
                    guint n_tests, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[READY_TEST_SUITE], 0, test_suite,
                  n_test_cases, n_tests);
}

static void
cb_start_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutRunner *runner = data;

    g_signal_emit(runner, signals[START_TEST_SUITE], 0, test_suite);
}

static void
cb_crashed_test_suite(CutTestSuite *test_suite, const gchar *stack_trace,
                      gpointer data)
{
    CutRunner *runner = data;
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    priv->crashed = TRUE;
    g_free(priv->stack_trace);
    priv->stack_trace = g_strdup(stack_trace);

    g_signal_emit(runner, signals[CRASHED], 0, priv->stack_trace);
}

static void
cb_complete_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutRunner *runner = data;

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_suite,                    \
                                         G_CALLBACK(cb_ ##              \
                                                    name ##             \
                                                    _test_suite),       \
                                         data)

    DISCONNECT(ready);
    DISCONNECT(start);
    DISCONNECT(complete);
    DISCONNECT(crashed);
#undef DISCONNECT

    g_signal_emit(runner, signals[COMPLETE_TEST_SUITE], 0, test_suite);
}

void
cut_runner_start_test_suite (CutRunner *runner, CutTestSuite *test_suite)
{
#define CONNECT(name)                                                \
    g_signal_connect(test_suite, #name,                              \
                     G_CALLBACK(cb_ ## name ## _test_suite),         \
                     runner)

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
    CONNECT(crashed);
#undef CONNECT
}


guint
cut_runner_get_n_tests (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->n_tests;
}

guint
cut_runner_get_n_assertions (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->n_assertions;
}

guint
cut_runner_get_n_failures (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->n_failures;
}

guint
cut_runner_get_n_errors (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->n_errors;
}

guint
cut_runner_get_n_pendings (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->n_pendings;
}

guint
cut_runner_get_n_notifications (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->n_notifications;
}

guint
cut_runner_get_n_omissions (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->n_omissions;
}

const GList *
cut_runner_get_results (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->results;
}

gboolean
cut_runner_is_crashed (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->crashed;
}

const gchar *
cut_runner_get_stack_trace (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->stack_trace;
}

void
cut_runner_cancel (CutRunner *runner)
{
    CUT_RUNNER_GET_PRIVATE(runner)->canceled = TRUE;
}

gboolean
cut_runner_is_canceled (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->canceled;
}

CutTestSuite *
cut_runner_create_test_suite (CutRunner *runner)
{
    CutRunnerPrivate *priv;
    CutRepository *repository;
    CutTestSuite *suite;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    repository = cut_repository_new(priv->test_directory);
    suite = cut_repository_create_test_suite(repository);
    g_object_unref(repository);

    return suite;
}

CutTestSuite *
cut_runner_get_test_suite (CutRunner *runner)
{
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    if (!priv->test_suite)
        priv->test_suite = cut_runner_create_test_suite(runner);

    return priv->test_suite;
}

void
cut_runner_set_test_suite (CutRunner *runner, CutTestSuite *suite)
{
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    if (priv->test_suite)
        g_object_unref(priv->test_suite);

    if (suite)
        g_object_ref(suite);
    priv->test_suite = suite;
}

static void
attach_listener (CutListener *listener, CutRunner *runner)
{
    cut_listener_attach_to_runner(listener, runner);
}

static void
attach_all_listeners (CutRunner *runner)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    g_list_foreach(priv->listeners, (GFunc)attach_listener, runner);
}

static void
detach_listener (CutListener *listener, CutRunner *runner)
{
    cut_listener_detach_from_runner(listener, runner);
}

static void
detach_all_listeners (CutRunner *runner)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    g_list_foreach(priv->listeners, (GFunc)detach_listener, runner);
}

gboolean
cut_runner_run (CutRunner *runner)
{
    CutTestSuite *suite;
    gboolean success;

    suite = cut_runner_get_test_suite(runner);
    attach_all_listeners(runner);
    success = cut_test_suite_run(suite, runner);
    detach_all_listeners(runner);

    return success;
}

void
cut_runner_add_listener (CutRunner *runner,
                         CutListener *listener)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    priv->listeners = g_list_prepend(priv->listeners, g_object_ref(listener));
}

void
cut_runner_remove_listener (CutRunner *runner, CutListener *listener)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);
    remove_listener(listener, runner);
    priv->listeners = g_list_remove(priv->listeners, listener);
    g_object_unref(listener);
}

gchar *
cut_runner_build_source_filename (CutRunner *runner, const gchar *filename)
{
    const gchar *source_directory;
    gchar *source_filename;

    source_directory = cut_runner_get_source_directory(runner);
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
cut_runner_set_test_case_order (CutRunner *runner, CutOrder order)
{
    CutRunnerPrivate *priv;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
    g_mutex_lock(priv->mutex);
    priv->test_case_order = order;
    g_mutex_unlock(priv->mutex);
}

CutOrder
cut_runner_get_test_case_order (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->test_case_order;
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
cut_runner_sort_test_cases (CutRunner *runner, GList *test_cases)
{
    CutRunnerPrivate *priv;
    GList *sorted_test_cases = NULL;
    gboolean ascending;

    priv = CUT_RUNNER_GET_PRIVATE(runner);
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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
