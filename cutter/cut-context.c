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
    CutTest *test;
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

    priv->test = NULL;
    priv->output = cut_output_new();
}

static void
dispose (GObject *object)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

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

void
cut_context_increment_assertion_count (CutContext *context)
{
    CutTest *test = CUT_CONTEXT_GET_PRIVATE(context)->test;

    cut_test_increment_assertion_count(test);
}

static void
cb_start(CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_start_test(priv->output, test);
}

static void
cb_success(CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_success(priv->output, test);
}

static void
cb_failure(CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_failure(priv->output, test);
}

static void
cb_error(CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_error(priv->output, test);
}

static void
cb_pending(CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_pending(priv->output, test);
}

static void
cb_complete(CutTest *test, gpointer data)
{
    CutContext *context = data;
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->output)
        cut_output_on_complete_test(priv->output, test);
}

void
cut_context_set_test (CutContext *context, CutTest *test)
{
    CutContextPrivate *priv;

    priv = CUT_CONTEXT_GET_PRIVATE(context);
    if (priv->test) {
        g_signal_handlers_disconnect_by_func(priv->test,
                                             G_CALLBACK(cb_start),
                                             context);
        g_signal_handlers_disconnect_by_func(priv->test,
                                             G_CALLBACK(cb_success),
                                             context);
        g_signal_handlers_disconnect_by_func(priv->test,
                                             G_CALLBACK(cb_failure),
                                             context);
        g_signal_handlers_disconnect_by_func(priv->test,
                                             G_CALLBACK(cb_error),
                                             context);
        g_signal_handlers_disconnect_by_func(priv->test,
                                             G_CALLBACK(cb_pending),
                                             context);
        g_signal_handlers_disconnect_by_func(priv->test,
                                             G_CALLBACK(cb_complete),
                                             context);
    }

    if (test) {
        g_signal_connect(test, "start", G_CALLBACK(cb_start), context);
        g_signal_connect(test, "success", G_CALLBACK(cb_success), context);
        g_signal_connect(test, "failure", G_CALLBACK(cb_failure), context);
        g_signal_connect(test, "error", G_CALLBACK(cb_error), context);
        g_signal_connect(test, "pending", G_CALLBACK(cb_pending), context);
        g_signal_connect(test, "complete", G_CALLBACK(cb_complete), context);
    }

    priv->test = test;
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
}

void
cut_context_connect_test_case (CutContext *context, CutTestCase *test_case)
{
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
        cut_output_on_complete_test_suite(priv->output, test_suite);
}

void
cut_context_connect_test_suite (CutContext *context, CutTestSuite *test_suite)
{
    g_signal_connect(test_suite, "start",
                     G_CALLBACK(cb_start_test_suite), context);
    g_signal_connect(test_suite, "complete",
                     G_CALLBACK(cb_complete_test_suite), context);
}

CutTest *
cut_context_get_current_test (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->test;
}

void
cut_context_set_result (CutContext *context,
                        CutTestResultStatus status,
                        const gchar *result_message,
                        const gchar *function_name,
                        const gchar *filename,
                        guint line)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);
    CutTest *test = priv->test;

    cut_test_set_result(test, status, result_message,
                        function_name, filename,
                        line);
}

void
cut_context_output_error_log (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_on_failure(priv->output, priv->test);
}

void
cut_context_output_normal_log (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_on_success(priv->output, priv->test);
}

CutContext *
cut_context_get_current (void)
{
    return g_private_get(cut_context_private);
}

void
cut_context_set_current (CutContext *context)
{
    g_private_set(cut_context_private, context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
