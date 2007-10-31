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
    guint n_assertion;
};

enum
{
    PROP_0,
    PROP_TEST_FUNCTION
};

G_DEFINE_ABSTRACT_TYPE (CutTest, cut_test, G_TYPE_OBJECT)

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

    g_type_class_add_private(gobject_class, sizeof(CutTestPrivate));
}

static void
cut_test_init (CutTest *container)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(container);

    priv->test_function = NULL;
    priv->n_assertion = 0;
}

static void
dispose (GObject *object)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

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

    return priv->test_function();
}

gboolean
cut_test_run (CutTest *test)
{
    CutTestClass *class = CUT_TEST_GET_CLASS(test);

    return class->run(test);
}

guint
cut_test_get_assertion_count (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->n_assertion;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
