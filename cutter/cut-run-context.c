/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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

#include "cut-run-context.h"
#include "cut-runner.h"
#include "cut-listener.h"
#include "cut-repository.h"
#include "cut-test-case.h"
#include "cut-test-result.h"

#include "cut-marshalers.h"
#include "cut-enum-types.h"

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
    GList *results;
    gboolean use_multi_thread;
    gboolean is_multi_thread;
    GMutex *mutex;
    gboolean crashed;
    gchar *backtrace;
    gchar *test_directory;
    gchar **exclude_files;
    gchar **exclude_dirs;
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
    PROP_N_SUCCESSES,
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

    START_RUN,
    READY_TEST_SUITE,
    START_TEST_SUITE,
    READY_TEST_CASE,
    START_TEST_CASE,
    START_TEST,

    PASS_ASSERTION,

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
    COMPLETE_RUN,

    CRASHED,

    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};
static CutRunnerIface *parent_runner_iface;

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

    spec = g_param_spec_enum("test-case-order",
                             "Test case order",
                             "Sort key for test case",
                             CUT_TYPE_ORDER,
                             CUT_ORDER_NONE_SPECIFIED,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_CASE_ORDER, spec);


    signals[START_RUN]
        = g_signal_new("start-run",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, start_run),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__VOID,
                       G_TYPE_NONE, 0);

    signals[READY_TEST_SUITE]
        = g_signal_new ("ready-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, ready_test_suite),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_UINT_UINT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST_SUITE, G_TYPE_UINT, G_TYPE_UINT);

    signals[START_TEST_SUITE]
        = g_signal_new ("start-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, start_test_suite),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_SUITE);

    signals[READY_TEST_CASE]
        = g_signal_new ("ready-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, ready_test_case),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_UINT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, G_TYPE_UINT);

    signals[START_TEST_CASE]
        = g_signal_new ("start-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, start_test_case),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

    signals[START_TEST]
        = g_signal_new ("start-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, start_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[PASS_ASSERTION]
        = g_signal_new ("pass-assertion",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, pass_assertion),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[SUCCESS_TEST]
        = g_signal_new ("success-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, success_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[FAILURE_TEST]
        = g_signal_new ("failure-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, failure_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[ERROR_TEST]
        = g_signal_new ("error-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, error_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[PENDING_TEST]
        = g_signal_new ("pending-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, pending_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_TEST]
        = g_signal_new ("notification-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, notification_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                        G_TYPE_NONE, 3,
                        CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                        CUT_TYPE_TEST_RESULT);

    signals[OMISSION_TEST]
        = g_signal_new("omission-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, omission_test),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                       G_TYPE_NONE, 3,
                       CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                       CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_TEST]
        = g_signal_new ("complete-test",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, complete_test),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[SUCCESS_TEST_CASE]
        = g_signal_new("success-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, success_test_case),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[FAILURE_TEST_CASE]
        = g_signal_new("failure-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, failure_test_case),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[ERROR_TEST_CASE]
        = g_signal_new("error-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, error_test_case),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[PENDING_TEST_CASE]
        = g_signal_new("pending-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, pending_test_case),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[NOTIFICATION_TEST_CASE]
        = g_signal_new("notification-test-case",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, notification_test_case),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[OMISSION_TEST_CASE]
        = g_signal_new("omission-test-case",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, omission_test_case),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, CUT_TYPE_TEST_RESULT);

    signals[COMPLETE_TEST_CASE]
        = g_signal_new ("complete-test-case",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, complete_test_case),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

    signals[COMPLETE_TEST_SUITE]
        = g_signal_new ("complete-test-suite",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, complete_test_suite),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_SUITE);

    signals[COMPLETE_RUN]
        = g_signal_new("complete-run",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutRunContextClass, complete_run),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__BOOLEAN,
                       G_TYPE_NONE, 1, G_TYPE_BOOLEAN);

    signals[CRASHED]
        = g_signal_new ("crashed",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutRunContextClass, crashed),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__STRING,
                        G_TYPE_NONE, 1, G_TYPE_STRING);

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
    priv->results = NULL;
    priv->use_multi_thread = FALSE;
    priv->is_multi_thread = FALSE;
    priv->mutex = g_mutex_new();
    priv->crashed = FALSE;
    priv->backtrace = NULL;
    priv->test_directory = NULL;
    priv->source_directory = NULL;
    priv->exclude_files = NULL;
    priv->exclude_dirs = NULL;
    priv->target_test_case_names = NULL;
    priv->target_test_names = NULL;
    priv->canceled = FALSE;
    priv->test_suite = NULL;
    priv->listeners = NULL;
}

static void
remove_listener (CutListener *listener, CutRunContext *context)
{
    cut_listener_detach_from_run_context(listener, context);
}

static void
dispose (GObject *object)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(object);

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
        g_list_foreach(priv->listeners, (GFunc)remove_listener, CUT_RUN_CONTEXT(object));
        g_list_free(priv->listeners);
        priv->listeners = NULL;
    }

    g_free(priv->backtrace);
    priv->backtrace = NULL;

    g_free(priv->source_directory);
    priv->source_directory = NULL;

    g_strfreev(priv->exclude_files);
    priv->exclude_files = NULL;

    g_strfreev(priv->exclude_dirs);
    priv->exclude_dirs = NULL;

    g_strfreev(priv->target_test_case_names);
    priv->target_test_case_names = NULL;

    g_strfreev(priv->target_test_names);
    priv->target_test_names = NULL;

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
      case PROP_TEST_CASE_ORDER:
        g_value_set_enum(value, priv->test_case_order);
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

void
cut_run_context_set_test_directory (CutRunContext *context, const gchar *directory)
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
cut_run_context_set_source_directory (CutRunContext *context, const gchar *directory)
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
cut_run_context_set_multi_thread (CutRunContext *context, gboolean use_multi_thread)
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
cut_run_context_set_exclude_files (CutRunContext *context, gchar **files)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->exclude_files);
    priv->exclude_files = g_strdupv(files);
}

gchar **
cut_run_context_get_exclude_files (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return priv->exclude_files;
}

void
cut_run_context_set_exclude_dirs (CutRunContext *context, gchar **dirs)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->exclude_dirs);
    priv->exclude_dirs = g_strdupv(dirs);
}

gchar **
cut_run_context_get_exclude_dirs (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return priv->exclude_dirs;
}

void
cut_run_context_set_target_test_case_names (CutRunContext *context, gchar **names)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->target_test_case_names);
    priv->target_test_case_names = g_strdupv(names);
}

gchar **
cut_run_context_get_target_test_case_names (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return priv->target_test_case_names;
}

void
cut_run_context_set_target_test_names (CutRunContext *context, gchar **names)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);

    g_strfreev(priv->target_test_names);
    priv->target_test_names = g_strdupv(names);
}

gchar **
cut_run_context_get_target_test_names (CutRunContext *context)
{
    CutRunContextPrivate *priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    return priv->target_test_names;
}

static void
cb_pass_assertion (CutTest *test, CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->n_assertions++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(context, signals[PASS_ASSERTION], 0, test, test_context);
}

static void
cb_success (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_successes++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(context, signals[SUCCESS_TEST], 0, test, test_context, result);
}

static void
cb_failure (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_failures++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(context, signals[FAILURE_TEST], 0, test, test_context, result);
}

static void
cb_error (CutTest *test, CutTestContext *test_context, CutTestResult *result,
          gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_errors++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(context, signals[ERROR_TEST], 0, test, test_context, result);
}

static void
cb_pending (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_pendings++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(context, signals[PENDING_TEST], 0, test, test_context, result);
}

static void
cb_notification (CutTest *test, CutTestContext *test_context,
                 CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_notifications++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(context, signals[NOTIFICATION_TEST], 0, test, test_context, result);
}

static void
cb_omission (CutTest *test, CutTestContext *test_context,
             CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->results = g_list_prepend(priv->results, g_object_ref(result));
    priv->n_omissions++;
    g_mutex_unlock(priv->mutex);

    g_signal_emit(context, signals[OMISSION_TEST], 0, test, test_context, result);
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
cut_run_context_prepare_test (CutRunContext *context, CutTest *test)
{
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    g_mutex_lock(priv->mutex);
    priv->n_tests++;
    g_mutex_unlock(priv->mutex);

    g_signal_connect(test, "pass_assertion",
                     G_CALLBACK(cb_pass_assertion), context);
    g_signal_connect(test, "success", G_CALLBACK(cb_success), context);
    g_signal_connect(test, "failure", G_CALLBACK(cb_failure), context);
    g_signal_connect(test, "error", G_CALLBACK(cb_error), context);
    g_signal_connect(test, "pending", G_CALLBACK(cb_pending), context);
    g_signal_connect(test, "notification", G_CALLBACK(cb_notification), context);
    g_signal_connect(test, "omission", G_CALLBACK(cb_omission), context);
    g_signal_connect(test, "complete", G_CALLBACK(cb_complete), context);
}

static void
cb_start_test (CutTestCase *test_case, CutTest *test,
               CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[START_TEST], 0, test, test_context);
}

static void
cb_complete_test(CutTestCase *test_case, CutTest *test,
                 CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[COMPLETE_TEST], 0, test, test_context);
}

static void
cb_success_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[SUCCESS_TEST_CASE], 0, test_case, result);
}

static void
cb_failure_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[FAILURE_TEST_CASE], 0, test_case, result);
}

static void
cb_error_test_case (CutTestCase *test_case, CutTestContext *test_context,
                    CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[ERROR_TEST_CASE], 0, test_case, result);
}

static void
cb_pending_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[PENDING_TEST_CASE], 0, test_case, result);
}

static void
cb_notification_test_case (CutTestCase *test_case, CutTestContext *test_context,
                           CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[NOTIFICATION_TEST_CASE], 0,
                  test_case, result);
}

static void
cb_omission_test_case (CutTestCase *test_case, CutTestContext *test_context,
                       CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[OMISSION_TEST_CASE], 0, test_case, result);
}

static void
cb_start_test_case(CutTestCase *test_case, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[START_TEST_CASE], 0, test_case);
}

static void
cb_ready_test_case(CutTestCase *test_case, guint n_tests, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[READY_TEST_CASE], 0, test_case, n_tests);
}

static void
cb_complete_test_case(CutTestCase *test_case, gpointer data)
{
    CutRunContext *context = data;

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

    g_signal_emit(context, signals[COMPLETE_TEST_CASE], 0, test_case);
}

void
cut_run_context_prepare_test_case (CutRunContext *context, CutTestCase *test_case)
{
#define CONNECT(name) \
    g_signal_connect(test_case, #name, G_CALLBACK(cb_ ## name), context)

    CONNECT(start_test);
    CONNECT(complete_test);
#undef CONNECT

#define CONNECT(name)                                                   \
    g_signal_connect(test_case, #name,                                  \
                     G_CALLBACK(cb_ ## name ## _test_case), context)

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
    CutRunContext *context = data;

    g_signal_emit(context, signals[READY_TEST_SUITE], 0, test_suite,
                  n_test_cases, n_tests);
}

static void
cb_start_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit(context, signals[START_TEST_SUITE], 0, test_suite);
}

static void
cb_crashed_test_suite(CutTestSuite *test_suite, const gchar *backtrace,
                      gpointer data)
{
    CutRunContext *context = data;
    CutRunContextPrivate *priv;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    priv->crashed = TRUE;
    g_free(priv->backtrace);
    priv->backtrace = g_strdup(backtrace);

    g_signal_emit(context, signals[CRASHED], 0, priv->backtrace);
}

static void
cb_complete_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutRunContext *context = data;

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

    g_signal_emit(context, signals[COMPLETE_TEST_SUITE], 0, test_suite);
}

void
cut_run_context_prepare_test_suite (CutRunContext *context, CutTestSuite *test_suite)
{
#define CONNECT(name)                                                \
    g_signal_connect(test_suite, #name,                              \
                     G_CALLBACK(cb_ ## name ## _test_suite),         \
                     context)

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
    CONNECT(crashed);
#undef CONNECT
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

const GList *
cut_run_context_get_results (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->results;
}

gboolean
cut_run_context_is_crashed (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->crashed;
}

const gchar *
cut_run_context_get_backtrace (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->backtrace;
}

void
cut_run_context_cancel (CutRunContext *context)
{
    CUT_RUN_CONTEXT_GET_PRIVATE(context)->canceled = TRUE;
}

gboolean
cut_run_context_is_canceled (CutRunContext *context)
{
    return CUT_RUN_CONTEXT_GET_PRIVATE(context)->canceled;
}

CutTestSuite *
cut_run_context_create_test_suite (CutRunContext *context)
{
    CutRunContextPrivate *priv;
    CutRepository *repository;
    CutTestSuite *suite;

    priv = CUT_RUN_CONTEXT_GET_PRIVATE(context);
    repository = cut_repository_new(priv->test_directory);
    cut_repository_set_exclude_files(repository, priv->exclude_files);
    cut_repository_set_exclude_dirs(repository, priv->exclude_dirs);
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
    remove_listener(listener, context);
    priv->listeners = g_list_remove(priv->listeners, listener);
    g_object_unref(listener);
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
cut_run_context_build_source_filename (CutRunContext *context, const gchar *filename)
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

    cut_run_context_attach_listeners(context);
    g_signal_emit_by_name(context, "start-run");
    success = cut_runner_run(CUT_RUNNER(context));
    cut_run_context_detach_listeners(context);

    return success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
