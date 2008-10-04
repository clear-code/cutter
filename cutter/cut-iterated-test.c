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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-iterated-test.h"
#include "cut-test-container.h"
#include "cut-run-context.h"
#include "cut-marshalers.h"
#include "cut-test-result.h"
#include "cut-utils.h"

#define CUT_ITERATED_TEST_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_ITERATED_TEST, CutIteratedTestPrivate))

typedef struct _CutIteratedTestPrivate	CutIteratedTestPrivate;
struct _CutIteratedTestPrivate
{
    CutIteratedTestFunction iterated_test_function;
    CutTestData *data;
};

enum
{
    PROP_0,
    PROP_ITERATED_TEST_FUNCTION,
    PROP_DATA
};

G_DEFINE_TYPE(CutIteratedTest, cut_iterated_test, CUT_TYPE_TEST)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static gboolean     is_available (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);
static void         invoke       (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);

static void
cut_iterated_test_class_init (CutIteratedTestClass *klass)
{
    GObjectClass *gobject_class;
    CutTestClass *test_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    test_class = CUT_TEST_CLASS(klass);
    test_class->is_available = is_available;
    test_class->invoke = invoke;

    spec = g_param_spec_pointer("iterated-test-function",
                                "Iterated Test Function",
                                "The function for iterated test",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_ITERATED_TEST_FUNCTION,
                                    spec);

    spec = g_param_spec_object("data",
                               "Data",
                               "The test data for iterated test",
                               CUT_TYPE_TEST_DATA,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DATA, spec);

    g_type_class_add_private(gobject_class, sizeof(CutIteratedTestPrivate));
}

static void
cut_iterated_test_init (CutIteratedTest *test)
{
    CutIteratedTestPrivate *priv = CUT_ITERATED_TEST_GET_PRIVATE(test);

    priv->iterated_test_function = NULL;
    priv->data = NULL;
}

static void
dispose (GObject *object)
{
    CutIteratedTestPrivate *priv = CUT_ITERATED_TEST_GET_PRIVATE(object);

    priv->iterated_test_function = NULL;

    if (priv->data) {
        g_object_unref(priv->data);
        priv->data = NULL;
    }

    G_OBJECT_CLASS(cut_iterated_test_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutIteratedTestPrivate *priv = CUT_ITERATED_TEST_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_ITERATED_TEST_FUNCTION:
        priv->iterated_test_function = g_value_get_pointer(value);
        break;
      case PROP_DATA:
        cut_iterated_test_set_data(CUT_ITERATED_TEST(object),
                                   g_value_get_object(value));
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
    CutIteratedTestPrivate *priv = CUT_ITERATED_TEST_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_ITERATED_TEST_FUNCTION:
        g_value_set_pointer(value, priv->iterated_test_function);
        break;
      case PROP_DATA:
        g_value_set_object(value, priv->data);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutIteratedTest *
cut_iterated_test_new (const gchar *name, CutIteratedTestFunction function,
                       CutTestData *data)
{
    return g_object_new(CUT_TYPE_ITERATED_TEST,
                        "element-name", "iterated-test",
                        "name", name,
                        "iterated-test-function", function,
                        "data", data,
                        NULL);
}

CutIteratedTest *
cut_iterated_test_new_empty (void)
{
    return cut_iterated_test_new(NULL, NULL, NULL);
}

CutTestData *
cut_iterated_test_get_data (CutIteratedTest *test)
{
    return CUT_ITERATED_TEST_GET_PRIVATE(test)->data;
}

void
cut_iterated_test_set_data (CutIteratedTest *test, CutTestData *data)
{
    CutIteratedTestPrivate *priv;

    priv = CUT_ITERATED_TEST_GET_PRIVATE(test);
    if (priv->data)
        g_object_unref(priv->data);
    priv->data = data;
    if (priv->data)
        g_object_ref(priv->data);
}

void
cut_iterated_test_clear_data (CutIteratedTest *test)
{
    CutIteratedTestPrivate *priv;

    priv = CUT_ITERATED_TEST_GET_PRIVATE(test);
    if (priv->data)
        cut_test_data_set_value(priv->data, NULL, NULL);
}

static gboolean
is_available (CutTest *test, CutTestContext *test_context,
              CutRunContext *run_context)
{
    CutIteratedTestPrivate *priv;

    priv = CUT_ITERATED_TEST_GET_PRIVATE(test);
    return priv->iterated_test_function && priv->data;
}

static void
invoke (CutTest *test, CutTestContext *test_context, CutRunContext *run_context)
{
    CutIteratedTestPrivate *priv;
    gconstpointer value;

    priv = CUT_ITERATED_TEST_GET_PRIVATE(test);

    value = cut_test_data_get_value(priv->data);
    return priv->iterated_test_function(value);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
