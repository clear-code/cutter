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

#include "cut-test.h"
#include "cut-test-container.h"

#define CUT_TEST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST, CutTestPrivate))

typedef struct _CutTestPrivate	CutTestPrivate;
struct _CutTestPrivate
{
    gchar *function_name;
    CutTestFunction test_function;
    guint assertion_count;
    CutTestResult *result;
    GTimer *timer;
};

enum
{
    PROP_0,
    PROP_FUNCTION_NAME,
    PROP_TEST_FUNCTION,
    PROP_ASSERTION_COUNT
};

enum
{
    START_SIGNAL,
    SUCCESS_SIGNAL,
    FAILURE_SIGNAL,
    ERROR_SIGNAL,
    PENDING_SIGNAL,
    COMPLETE_SIGNAL,
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

static gboolean real_run   (CutTest         *test);
static gdouble  real_get_elapsed  (CutTest  *test);

static void
cut_test_class_init (CutTestClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->run = real_run;
    klass->get_elapsed = real_get_elapsed;

    spec = g_param_spec_string("function-name",
                               "Fcuntion name",
                               "The function name of the test",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_FUNCTION_NAME, spec);

    spec = g_param_spec_pointer("test-function",
                                "Test Function",
                                "The function for test",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_FUNCTION, spec);

    spec = g_param_spec_uint("assertion-count",
                             "Assertion Count",
                             "The number of assertion.",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ASSERTION_COUNT, spec);

	cut_test_signals[START_SIGNAL]
        = g_signal_new ("start",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET (CutTestClass, start),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

	cut_test_signals[SUCCESS_SIGNAL]
        = g_signal_new ("success",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET (CutTestClass, success),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

	cut_test_signals[FAILURE_SIGNAL]
        = g_signal_new ("failure",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET (CutTestClass, failure),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

	cut_test_signals[ERROR_SIGNAL]
        = g_signal_new ("error",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET (CutTestClass, error),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

	cut_test_signals[PENDING_SIGNAL]
        = g_signal_new ("pending",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET (CutTestClass, pending),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

	cut_test_signals[COMPLETE_SIGNAL]
        = g_signal_new ("complete",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET (CutTestClass, complete),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

    g_type_class_add_private(gobject_class, sizeof(CutTestPrivate));
}

static void
cut_test_init (CutTest *container)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(container);

    priv->test_function = NULL;
    priv->assertion_count = 0;
    priv->result = NULL;
    priv->timer = g_timer_new();
}

static void
cut_test_result_free (CutTestResult *result)
{
    if (!result)
        return;

    if (result->message)
        g_free(result->message);
    if (result->function_name)
        g_free(result->function_name);
    if (result->filename)
        g_free(result->filename);

    g_free(result);
}

static void
dispose (GObject *object)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    if (priv->function_name) {
        g_free(priv->function_name);
        priv->function_name = NULL;
    }
    if (priv->result) {
        cut_test_result_free(priv->result);
        priv->result = NULL;
    }
    priv->test_function = NULL;

    if (priv->timer) {
        g_timer_destroy(priv->timer);
        priv->timer = NULL;
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
      case PROP_FUNCTION_NAME:
        if (priv->function_name)
            g_free(priv->function_name);
        priv->function_name = g_value_dup_string(value);
        break;
      case PROP_TEST_FUNCTION:
        priv->test_function = g_value_get_pointer(value);
        break;
      case PROP_ASSERTION_COUNT:
        priv->assertion_count = g_value_get_uint(value);
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
      case PROP_FUNCTION_NAME:
        g_value_set_string(value, priv->function_name);
        break;
      case PROP_TEST_FUNCTION:
        g_value_set_pointer(value, priv->test_function);
        break;
      case PROP_ASSERTION_COUNT:
        g_value_set_uint(value, priv->assertion_count);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTest *
cut_test_new (const gchar *function_name, CutTestFunction function)
{
    return g_object_new(CUT_TYPE_TEST,
                        "function-name", function_name,
                        "test-function", function,
                        NULL);
}

static gboolean
real_run (CutTest *test)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);
    gboolean success;
    const gchar *status_signal_name = NULL;

    g_timer_start(priv->timer);
    priv->test_function();
    g_timer_stop(priv->timer);
    success = priv->result ? FALSE : TRUE;

    if (success) {
        status_signal_name = "success";
    } else {
        switch (priv->result->status) {
          case CUT_TEST_RESULT_SUCCESS:
            g_assert("must not happen");
            break;
          case CUT_TEST_RESULT_FAILURE:
            status_signal_name = "failure";
            break;
          case CUT_TEST_RESULT_ERROR:
            status_signal_name = "error";
            break;
          case CUT_TEST_RESULT_PENDING:
            status_signal_name = "pending";
            break;
        }
    }
    g_signal_emit_by_name(test, status_signal_name);

    return success;
}

gboolean
cut_test_run (CutTest *test)
{
    gboolean success;
    CutTestClass *class = CUT_TEST_GET_CLASS(test);

    if (!CUT_IS_TEST_CONTAINER(test)) {
        if (!CUT_TEST_GET_PRIVATE(test)->test_function)
            return FALSE;
    }    

    g_signal_emit_by_name(test, "start");
    success = class->run(test);
    g_signal_emit_by_name(test, "complete");

    return success;
}

void
cut_test_increment_assertion_count (CutTest *test)
{
    CUT_TEST_GET_PRIVATE(test)->assertion_count++;
}

void
cut_test_set_result (CutTest *test,
                     CutTestResultStatus status,
                     const gchar *result_message,
                     const gchar *function_name,
                     const gchar *filename,
                     guint line)
{
    CutTestResult *result;
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);

    result = g_new0(CutTestResult, 1);

    result->status = status;
    result->message = g_strdup(result_message);
    result->function_name = g_strdup(function_name);
    result->filename = g_strdup(filename);
    result->line = line;

    if (priv->result)
        cut_test_result_free(priv->result);
    priv->result = result;
}

const gchar *
cut_test_get_function_name (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->function_name;
}

guint
cut_test_get_assertion_count (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->assertion_count;
}

const CutTestResult *
cut_test_get_result (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->result;
}

static gdouble
real_get_elapsed (CutTest *test)
{
    return g_timer_elapsed(CUT_TEST_GET_PRIVATE(test)->timer, NULL);
}

gdouble
cut_test_get_elapsed (CutTest *test)
{
    return CUT_TEST_GET_CLASS(test)->get_elapsed(test);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
