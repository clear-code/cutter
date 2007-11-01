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

#include "cut-test-case.h"

#include "cut-test.h"

#define CUT_TEST_CASE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CASE, CutTestCasePrivate))

typedef struct _CutTestCasePrivate	CutTestCasePrivate;
struct _CutTestCasePrivate
{
    CutSetupFunction setup;
    CutTearDownFunction tear_down;
};

enum
{
    PROP_0,
    PROP_SETUP_FUNCTION,
    PROP_TEAR_DOWN_FUNCTION
};

G_DEFINE_TYPE (CutTestCase, cut_test_case, CUT_TYPE_TEST_CONTAINER)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void real_run       (CutTest         *test,
                            CutTestError   **error);

static void
cut_test_case_class_init (CutTestCaseClass *klass)
{
    GObjectClass *gobject_class;
    CutTestClass *test_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);
    test_class = CUT_TEST_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    test_class->run = real_run;

    spec = g_param_spec_pointer("setup-function",
                                "Setup Function",
                                "The function for setup",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SETUP_FUNCTION, spec);

    spec = g_param_spec_pointer("tear-down-function",
                                "TearDown Function",
                                "The function for tear down",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEAR_DOWN_FUNCTION, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestCasePrivate));
}

static void
cut_test_case_init (CutTestCase *test_case)
{
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(test_case);

    priv->setup = NULL;
    priv->tear_down = NULL;
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_test_case_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SETUP_FUNCTION:
        priv->setup = g_value_get_pointer(value);
        break;
      case PROP_TEAR_DOWN_FUNCTION:
        priv->tear_down = g_value_get_pointer(value);
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
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SETUP_FUNCTION:
        g_value_set_pointer(value, priv->setup);
        break;
      case PROP_TEAR_DOWN_FUNCTION:
        g_value_set_pointer(value, priv->tear_down);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestCase *
cut_test_case_new (CutSetupFunction setup_function, CutTearDownFunction tear_down_function)
{
    return g_object_new(CUT_TYPE_TEST_CASE,
                        "setup-function", setup_function,
                        "tear-down-function", tear_down_function, NULL);
}

guint
cut_test_case_get_test_count (CutTestCase *test_case)
{
    return 0;
}

static void
real_run (CutTest *test, CutTestError **error)
{
    CutTestCasePrivate *priv;

    g_return_if_fail(CUT_IS_TEST_CASE(test));

    priv = CUT_TEST_CASE_GET_PRIVATE(test);

    if (priv->setup)
        priv->setup();

    CUT_TEST_CLASS(cut_test_case_parent_class)->run(test, error);

    if (priv->tear_down)
        priv->tear_down();
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
