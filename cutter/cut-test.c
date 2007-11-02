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

#define CUT_TEST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST, CutTestPrivate))

typedef struct _CutTestPrivate	CutTestPrivate;
struct _CutTestPrivate
{
    CutTestFunction test_function;
    guint assertion_count;
    CutTestError *error;
};

enum
{
    PROP_0,
    PROP_TEST_FUNCTION,
    PROP_ASSERTION_COUNT
};

enum
{
	START_SIGNAL,
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
    priv->error = NULL;
}

static void
cut_test_error_free (CutTestError *error)
{
    if (!error)
        return;

    if (error->message)
        g_free(error->message);
    if (error->function_name)
        g_free(error->function_name);
    if (error->filename)
        g_free(error->filename);

    g_free(error);
}

static void
dispose (GObject *object)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    if (priv->error) {
        cut_test_error_free(priv->error);
        priv->error = NULL;
    }
    priv->test_function = NULL;

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
cut_test_new (CutTestFunction function)
{
    return g_object_new(CUT_TYPE_TEST,
                        "test-function", function,
                        NULL);
}

static gboolean
real_run (CutTest *test)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);

    if (!priv->test_function)
        return FALSE;
    
    g_signal_emit_by_name(test, "start");
    priv->test_function();
    g_signal_emit_by_name(test, "complete");

    return priv->error ? FALSE : TRUE;
}

gboolean
cut_test_run (CutTest *test)
{
    CutTestClass *class = CUT_TEST_GET_CLASS(test);

    return class->run(test);
}

void
cut_test_increment_assertion_count (CutTest *test)
{
    CUT_TEST_GET_PRIVATE(test)->assertion_count++;
}

void
cut_test_set_error (CutTest *test,
                    const gchar *error_message,
                    const gchar *function_name,
                    const gchar *filename,
                    guint line)
{
    CutTestError *error;
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);

    error = g_new0(CutTestError, 1);

    error->message = g_strdup(error_message);
    error->function_name = g_strdup(function_name);
    error->filename = g_strdup(filename);
    error->line = line;

    if (priv->error)
        cut_test_error_free(priv->error);
    priv->error = error;
}

guint
cut_test_get_assertion_count (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->assertion_count;
}

const CutTestError *
cut_test_get_error (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->error;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
