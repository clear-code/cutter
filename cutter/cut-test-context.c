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

#include "cut-test-context.h"

#define CUT_TEST_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CONTEXT, CutTestContextPrivate))

typedef struct _CutTestContextPrivate	CutTestContextPrivate;
struct _CutTestContextPrivate
{
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTest *test;
};

enum
{
    PROP_0,
    PROP_TEST_SUITE,
    PROP_TEST_CASE,
    PROP_TEST
};

G_DEFINE_TYPE (CutTestContext, cut_test_context, G_TYPE_OBJECT)

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
cut_test_context_class_init (CutTestContextClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    g_type_class_add_private(gobject_class, sizeof(CutTestContextPrivate));
}

static void
cut_test_context_init (CutTestContext *context)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    priv->test_suite = NULL;
    priv->test_case = NULL;
    priv->test = NULL;
}

static void
dispose (GObject *object)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(object);

    if (priv->test_suite) {
        g_object_unref(priv->test_suite);
        priv->test_suite = NULL;
    }

    if (priv->test_case) {
        g_object_unref(priv->test_case);
        priv->test_case = NULL;
    }

    if (priv->test) {
        g_object_unref(priv->test);
        priv->test = NULL;
    }

    G_OBJECT_CLASS(cut_test_context_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestContext *context = CUT_TEST_CONTEXT(object);

    switch (prop_id) {
      case PROP_TEST_SUITE:
        cut_test_context_set_test_suite(context, g_value_get_object(value));
        break;
      case PROP_TEST_CASE:
        cut_test_context_set_test_case(context, g_value_get_object(value));
        break;
      case PROP_TEST:
        cut_test_context_set_test(context, g_value_get_object(value));
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
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_TEST_SUITE:
        g_value_set_object(value, priv->test_suite);
        break;
      case PROP_TEST_CASE:
        g_value_set_object(value, priv->test_case);
        break;
      case PROP_TEST:
        g_value_set_object(value, priv->test);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestContext *
cut_test_context_new (CutTestSuite *test_suite, CutTestCase *test_case,
                      CutTest *test)
{
    return g_object_new(CUT_TYPE_TEST_CONTEXT,
                        "test-suite", test_suite,
                        "test-case", test_case,
                        "test", test,
                        NULL);
}

CutTestSuite *
cut_test_context_get_test_suite (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->test_suite;
}

void
cut_test_context_set_test_suite (CutTestContext *context, CutTestSuite *test_suite)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test_suite)
        g_object_unref(priv->test_suite);
    if (test_suite)
        g_object_ref(test_suite);
    priv->test_suite = test_suite;
}

CutTestCase *
cut_test_context_get_test_case (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->test_case;
}

void
cut_test_context_set_test_case (CutTestContext *context, CutTestCase *test_case)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test_case)
        g_object_unref(priv->test_case);
    if (test_case)
        g_object_ref(test_case);
    priv->test_case = test_case;
}

CutTest *
cut_test_context_get_test (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->test;
}

void
cut_test_context_set_test (CutTestContext *context, CutTest *test)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test)
        g_object_unref(priv->test);
    if (test)
        g_object_ref(test);
    priv->test = test;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
