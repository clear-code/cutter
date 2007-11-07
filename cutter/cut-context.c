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
#include "cut-context-private.h"
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
};

enum
{
    PROP_0
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

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

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
}

static void
dispose (GObject *object)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

    if (priv->results) {
        g_list_foreach(priv->results, (GFunc)g_free, NULL);
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
    if (info->result)
        g_object_unref(info->result);
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
        cut_output_on_complete_test_suite(priv->output, test_suite, NULL);

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

CutTest *
cut_context_get_current_test (CutContext *context)
{
    return NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
