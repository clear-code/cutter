/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include "cut-test.h"
#include "cut-test-container.h"
#include "cut-runner.h"
#include "cut-marshalers.h"
#include "cut-test-result.h"

#define CUT_TEST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST, CutTestPrivate))

typedef struct _CutTestPrivate	CutTestPrivate;
struct _CutTestPrivate
{
    gchar *name;
    CutTestFunction test_function;
    GTimer *timer;
    GHashTable *attributes;
};

enum
{
    PROP_0,
    PROP_NAME,
    PROP_TEST_FUNCTION
};

enum
{
    START,
    PASS_ASSERTION,
    SUCCESS,
    FAILURE,
    ERROR,
    PENDING,
    NOTIFICATION,
    COMPLETE,
    CRASHED,
    LAST_SIGNAL
};

static gint cut_test_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (CutTest, cut_test, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static gdouble      real_get_elapsed  (CutTest  *test);

static void
cut_test_class_init (CutTestClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->get_elapsed = real_get_elapsed;

    spec = g_param_spec_string("name",
                               "Test name",
                               "The name of the test",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_NAME, spec);

    spec = g_param_spec_pointer("test-function",
                                "Test Function",
                                "The function for test",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_FUNCTION, spec);

    cut_test_signals[START]
        = g_signal_new ("start",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, start),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__VOID,
                        G_TYPE_NONE, 0);

    cut_test_signals[PASS_ASSERTION]
        = g_signal_new ("pass-assertion",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, pass_assertion),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CONTEXT);

    cut_test_signals[SUCCESS]
        = g_signal_new ("success",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, success),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[FAILURE]
        = g_signal_new ("failure",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, failure),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[ERROR]
        = g_signal_new ("error",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, error),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[PENDING]
        = g_signal_new ("pending",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, pending),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[NOTIFICATION]
        = g_signal_new ("notification",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, notification),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[COMPLETE]
        = g_signal_new ("complete",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, complete),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__VOID,
                        G_TYPE_NONE, 0);

    cut_test_signals[CRASHED]
        = g_signal_new ("crashed",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, crashed),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__STRING,
                        G_TYPE_NONE, 1, G_TYPE_STRING);

    g_type_class_add_private(gobject_class, sizeof(CutTestPrivate));
}

static void
cut_test_init (CutTest *test)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);

    priv->test_function = NULL;
    priv->timer = NULL;
    priv->attributes = g_hash_table_new_full(g_str_hash, g_str_equal,
                                             g_free, g_free);
}

static void
dispose (GObject *object)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

    priv->test_function = NULL;

    if (priv->timer) {
        g_timer_destroy(priv->timer);
        priv->timer = NULL;
    }

    if (priv->attributes) {
        g_hash_table_unref(priv->attributes);
        priv->attributes = NULL;
    }

    G_OBJECT_CLASS(cut_test_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_NAME:
        if (priv->name)
            g_free(priv->name);
        priv->name = g_value_dup_string(value);
        break;
      case PROP_TEST_FUNCTION:
        priv->test_function = g_value_get_pointer(value);
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
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
      case PROP_TEST_FUNCTION:
        g_value_set_pointer(value, priv->test_function);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTest *
cut_test_new (const gchar *name, CutTestFunction function)
{
    return g_object_new(CUT_TYPE_TEST,
                        "name", name,
                        "test-function", function,
                        NULL);
}

gboolean
cut_test_run (CutTest *test, CutTestContext *test_context, CutRunner *runner)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);
    gboolean success = TRUE;
    jmp_buf jump_buffer;

    if (!priv->test_function)
        return FALSE;

    cut_runner_start_test(runner, test);

    g_signal_emit_by_name(test, "start");

    cut_test_context_set_jump(test_context, &jump_buffer);
    if (setjmp(jump_buffer) == 0) {
        if (priv->timer) {
            g_timer_start(priv->timer);
        } else {
            priv->timer = g_timer_new();
        }
        priv->test_function();
        success = TRUE;
    } else {
        success = FALSE;
    }
    g_timer_stop(priv->timer);

    if (success) {
        CutTestResult *result;
        result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                     test,
                                     cut_test_context_get_test_case(test_context),
                                     NULL,
                                     NULL, NULL, 
                                     NULL, NULL, 0);
        g_object_set(result, "elapsed", cut_test_get_elapsed(test), NULL);
        g_signal_emit_by_name(test, "success", test_context, result);
        g_object_unref(result);
    }

    g_signal_emit_by_name(test, "complete");

    return success;
}

const gchar *
cut_test_get_name (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->name;
}

const gchar *
cut_test_get_description (CutTest *test)
{
    return cut_test_get_attribute(test, "description");
}

static gdouble
real_get_elapsed (CutTest *test)
{
    CutTestPrivate *priv;

    priv = CUT_TEST_GET_PRIVATE(test);
    if (priv->timer)
        return g_timer_elapsed(priv->timer, NULL);
    else
        return 0.0;
}

gdouble
cut_test_get_elapsed (CutTest *test)
{
    return CUT_TEST_GET_CLASS(test)->get_elapsed(test);
}

const gchar *
cut_test_get_attribute (CutTest *test, const gchar *name)
{
    return g_hash_table_lookup(CUT_TEST_GET_PRIVATE(test)->attributes, name);
}

void
cut_test_set_attribute (CutTest *test, const gchar *name, const gchar *value)
{
    g_hash_table_replace(CUT_TEST_GET_PRIVATE(test)->attributes,
                         g_strdup(name),
                         g_strdup(value));
}

const GHashTable *
cut_test_get_attributes (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->attributes;
}

void
cut_test_stop_timer (CutTest *test)
{
    CutTestPrivate *priv;

    priv = CUT_TEST_GET_PRIVATE(test);

    if (priv->timer)
        g_timer_stop(priv->timer);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
