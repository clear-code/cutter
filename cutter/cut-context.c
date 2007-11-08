/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-context.h"
#include "cut-output.h"

#define CUT_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CONTEXT, CutContextPrivate))

typedef struct _CutContextPrivate	CutContextPrivate;
struct _CutContextPrivate
{
    guint n_tests;
    guint n_assertions;
    guint n_failures;
    guint n_errors;
    guint n_pendings;
    GList *results;
    CutOutput *output;
    gboolean use_multi_thread;
};

enum
{
    PROP_0,
    PROP_N_TESTS,
    PROP_N_ASSERTIONS,
    PROP_N_FAILURES,
    PROP_N_ERRORS,
    PROP_N_PENDINGS,
    PROP_USE_MULTI_THREAD
};

G_DEFINE_TYPE (CutContext, cut_context, G_TYPE_OBJECT)

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
cut_context_class_init (CutContextClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_uint("n-tests",
                             "Number of tests",
                             "The number of tests of the context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_TESTS, spec);

    spec = g_param_spec_uint("n-assertions",
                             "Number of assertions",
                             "The number of assertions of the context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_ASSERTIONS, spec);

    spec = g_param_spec_uint("n-failures",
                             "Number of failures",
                             "The number of failures of the context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_FAILURES, spec);

    spec = g_param_spec_uint("n-errors",
                             "Number of errors",
                             "The number of errors of the context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_ERRORS, spec);

    spec = g_param_spec_uint("n-pendings",
                             "Number of pendings",
                             "The number of pendings of the context",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_N_PENDINGS, spec);

    spec = g_param_spec_boolean("use-multi-thread",
                                "Use multi thread",
                                "Whether use multi thread or not in the context",
                                FALSE,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_USE_MULTI_THREAD, spec);

    g_type_class_add_private(gobject_class, sizeof(CutContextPrivate));
}

static void
cut_context_init (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    priv->n_tests = 0;
    priv->n_assertions = 0;
    priv->n_failures = 0;
    priv->n_errors = 0;
    priv->n_pendings = 0;
    priv->results = NULL;
    priv->output = cut_output_new();
    priv->use_multi_thread = FALSE;
}

static void
dispose (GObject *object)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

    if (priv->results) {
        g_list_foreach(priv->results, (GFunc)g_object_unref, NULL);
        g_list_free(priv->results);
        priv->results = NULL;
    }

    if (priv->output) {
        g_object_unref(priv->output);
        priv->output = NULL;
    }

    G_OBJECT_CLASS(cut_context_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

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
      case PROP_USE_MULTI_THREAD:
        priv->use_multi_thread = g_value_get_boolean(value);
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
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

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
      case PROP_USE_MULTI_THREAD:
        g_value_set_boolean(value, priv->use_multi_thread);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutContext *
cut_context_new (void)
{
    return g_object_new(CUT_TYPE_CONTEXT, NULL);
}

void
cut_context_set_verbose_level (CutContext *context, CutVerboseLevel level)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_verbose_level(priv->output, level);
}

void
cut_context_set_verbose_level_by_name (CutContext *context, const gchar *name)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_verbose_level_by_name(priv->output, name);
}


void
cut_context_set_source_directory (CutContext *context, const gchar *directory)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_source_directory(priv->output, directory);
}

void
cut_context_set_use_color (CutContext *context, gboolean use_color)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_use_color(priv->output, use_color);
}

void
cut_context_set_multi_thread (CutContext *context, gboolean use_multi_thread)
{
    CUT_CONTEXT_GET_PRIVATE(context)->use_multi_thread = use_multi_thread;
}

gboolean
cut_context_get_multi_thread (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->use_multi_thread;
}

typedef struct _TestCallBackInfo
{
    CutContext *context;
    CutTestResult *result;
} TestCallBackInfo;

static void
cb_pass_assertion(CutTest *test, gpointer data)
{
    TestCallBackInfo *info = data;
    CutContext *context;
    CutContextPrivate *priv;

    context = info->context;
    priv = CUT_CONTEXT_GET_PRIVATE(context);
    priv->n_assertions++;
}

static void
cb_success(CutTest *test, gpointer data)
{
    TestCallBackInfo *info = data;
    CutContext *context;
    CutContextPrivate *priv;

    context = info->context;
    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_success(priv->output, test);
}

static void
cb_failure(CutTest *test, CutTestResult *result, gpointer data)
{
    TestCallBackInfo *info = data;
    CutContext *context;
    CutContextPrivate *priv;

    context = info->context;
    info->result = g_object_ref(result);
    priv = CUT_CONTEXT_GET_PRIVATE(context);
    priv->n_failures++;
    if (priv->output)
        cut_output_on_failure(priv->output, test, info->result);
}

static void
cb_error(CutTest *test, CutTestResult *result, gpointer data)
{
    TestCallBackInfo *info = data;
    CutContext *context;
    CutContextPrivate *priv;

    context = info->context;
    info->result = g_object_ref(result);
    priv = CUT_CONTEXT_GET_PRIVATE(context);
    priv->n_errors++;
    if (priv->output)
        cut_output_on_error(priv->output, test, info->result);
}

static void
cb_pending(CutTest *test, CutTestResult *result, gpointer data)
{
    TestCallBackInfo *info = data;
    CutContext *context;
    CutContextPrivate *priv;

    context = info->context;
    info->result = g_object_ref(result);
    priv = CUT_CONTEXT_GET_PRIVATE(context);
    priv->n_pendings++;
    if (priv->output)
        cut_output_on_pending(priv->output, test, info->result);
}

static void
cb_complete (CutTest *test, gpointer data)
{
    TestCallBackInfo *info = data;
    CutContext *context;
    CutContextPrivate *priv;

    context = info->context;
    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (info->result) {
        priv->results = g_list_prepend(priv->results, info->result);
    }

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
                                         G_CALLBACK(cb_complete),
                                         data);

    g_object_unref(info->context);
    g_free(info);
}

void
cut_context_start_test (CutContext *context, CutTest *test)
{
    TestCallBackInfo *info;

    info = g_new0(TestCallBackInfo, 1);
    info->context = context;
    g_object_ref(context);
    CUT_CONTEXT_GET_PRIVATE(context)->n_tests++;

    g_signal_connect(test, "pass_assertion",
                     G_CALLBACK(cb_pass_assertion), info);
    g_signal_connect(test, "success", G_CALLBACK(cb_success), info);
    g_signal_connect(test, "failure", G_CALLBACK(cb_failure), info);
    g_signal_connect(test, "error", G_CALLBACK(cb_error), info);
    g_signal_connect(test, "pending", G_CALLBACK(cb_pending), info);
    g_signal_connect(test, "complete", G_CALLBACK(cb_complete), info);
}

static void
cb_start_test(CutTestCase *test_case, CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_start_test(priv->output, test_case, test);
}

static void
cb_complete_test(CutTestCase *test_case, CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_complete_test(priv->output, test_case, test, NULL);
}

static void
cb_start_test_case(CutTestCase *test_case, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_start_test_case(priv->output, test_case);
}

static void
cb_complete_test_case(CutTestCase *test_case, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_complete_test_case(priv->output, test_case);


    g_signal_handlers_disconnect_by_func(test_case,
                                         G_CALLBACK(cb_start_test), data);
    g_signal_handlers_disconnect_by_func(test_case,
                                         G_CALLBACK(cb_complete_test), data);

    g_signal_handlers_disconnect_by_func(test_case,
                                         G_CALLBACK(cb_start_test_case), data);
    g_signal_handlers_disconnect_by_func(test_case,
                                         G_CALLBACK(cb_complete_test_case),
                                         data);
}

void
cut_context_start_test_case (CutContext *context, CutTestCase *test_case)
{
    g_signal_connect(test_case, "start-test",
                     G_CALLBACK(cb_start_test), context);
    g_signal_connect(test_case, "complete-test",
                     G_CALLBACK(cb_complete_test), context);

    g_signal_connect(test_case, "start",
                     G_CALLBACK(cb_start_test_case), context);
    g_signal_connect(test_case, "complete",
                     G_CALLBACK(cb_complete_test_case), context);
}

static void
cb_start_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_start_test_suite(priv->output, test_suite);
}

static void
cb_complete_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_complete_test_suite(priv->output, context, test_suite);

    g_signal_handlers_disconnect_by_func(test_suite,
                                         G_CALLBACK(cb_start_test_suite), data);
    g_signal_handlers_disconnect_by_func(test_suite,
                                         G_CALLBACK(cb_complete_test_suite),
                                         data);
}

void
cut_context_start_test_suite (CutContext *context, CutTestSuite *test_suite)
{
    g_signal_connect(test_suite, "start",
                     G_CALLBACK(cb_start_test_suite), context);
    g_signal_connect(test_suite, "complete",
                     G_CALLBACK(cb_complete_test_suite), context);
}


guint
cut_context_get_n_tests (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->n_tests;
}

guint
cut_context_get_n_assertions (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->n_assertions;
}

guint
cut_context_get_n_failures (CutContext *context)
{
        return CUT_CONTEXT_GET_PRIVATE(context)->n_failures;
}

guint
cut_context_get_n_errors (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->n_errors;
}

guint
cut_context_get_n_pendings (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->n_pendings;
}

const GList *
cut_context_get_results (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->results;
};

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
