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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-test.h"
#include "cut-test-container.h"
#include "cut-run-context.h"
#include "cut-marshalers.h"
#include "cut-test-result.h"
#include "cut-utils.h"

#define CUT_TEST_DATA_GET_PRIVATE(obj)                                  \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_DATA, CutTestDataPrivate))

typedef struct _CutTestDataPrivate	CutTestDataPrivate;
struct _CutTestDataPrivate
{
    gchar *name;
    gpointer value;
    CutDestroyFunction destroy_function;
};

enum
{
    PROP_0,
    PROP_NAME,
    PROP_VALUE,
    PROP_DESTROY_FUNCTION
};

G_DEFINE_TYPE(CutTestData, cut_test_data, G_TYPE_OBJECT)

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
cut_test_data_class_init (CutTestDataClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("name",
                               "Test Data Name",
                               "The name of the test data",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NAME, spec);

    spec = g_param_spec_pointer("value",
                                "Test Data Value",
                                "The value of the test data",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_VALUE, spec);

    spec = g_param_spec_pointer("destroy-function",
                                "Test Data Destroy Function",
                                "The destroy function for the test data's value",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_DESTROY_FUNCTION, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestDataPrivate));
}

static void
cut_test_data_init (CutTestData *test)
{
    CutTestDataPrivate *priv = CUT_TEST_DATA_GET_PRIVATE(test);

    priv->name = NULL;
    priv->value = NULL;
    priv->destroy_function = NULL;
}

static void
free_name (CutTestDataPrivate *priv)
{
    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }
}

static void
free_value (CutTestDataPrivate *priv)
{
    if (priv->value) {
        if (priv->destroy_function)
            priv->destroy_function(priv->value);
        priv->value = NULL;
    }
    priv->destroy_function = NULL;
}

static void
dispose (GObject *object)
{
    CutTestDataPrivate *priv = CUT_TEST_DATA_GET_PRIVATE(object);

    free_name(priv);
    free_value(priv);

    G_OBJECT_CLASS(cut_test_data_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestDataPrivate *priv = CUT_TEST_DATA_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_NAME:
        cut_test_data_set_name(CUT_TEST_DATA(object), g_value_get_string(value));
        break;
      case PROP_VALUE:
        free_value(priv);
        priv->value = g_value_get_pointer(value);
        break;
      case PROP_DESTROY_FUNCTION:
        priv->destroy_function = g_value_get_pointer(value);
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
    CutTestDataPrivate *priv = CUT_TEST_DATA_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
      case PROP_VALUE:
        g_value_set_pointer(value, priv->value);
        break;
      case PROP_DESTROY_FUNCTION:
        g_value_set_pointer(value, priv->destroy_function);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestData *
cut_test_data_new (const gchar *name, gpointer value,
                   CutDestroyFunction destroy_function)
{
    return g_object_new(CUT_TYPE_TEST_DATA,
                        "name", name,
                        "value", value,
                        "destroy-function", destroy_function,
                        NULL);
}

CutTestData *
cut_test_data_new_empty (void)
{
    return cut_test_data_new(NULL, NULL, NULL);
}

const gchar *
cut_test_data_get_name (CutTestData *test_data)
{
    return CUT_TEST_DATA_GET_PRIVATE(test_data)->name;
}

void
cut_test_data_set_name (CutTestData *test_data, const gchar *name)
{
    CutTestDataPrivate *priv;

    priv = CUT_TEST_DATA_GET_PRIVATE(test_data);
    free_name(priv);

    if (name)
        priv->name = g_strdup(name);
}

gpointer
cut_test_data_get_value (CutTestData *test_data)
{
    return CUT_TEST_DATA_GET_PRIVATE(test_data)->value;
}

void
cut_test_data_set_value (CutTestData *test_data, gpointer value,
                         CutDestroyFunction destroy_function)
{
    CutTestDataPrivate *priv;

    priv = CUT_TEST_DATA_GET_PRIVATE(test_data);
    free_value(priv);

    priv->value = value;
    priv->destroy_function = destroy_function;
}

gchar *
cut_test_data_to_xml (CutTestData *test_data)
{
    GString *string;

    string = g_string_new(NULL);
    cut_test_data_to_xml_string(test_data, string, 0);
    return g_string_free(string, FALSE);
}

void
cut_test_data_to_xml_string (CutTestData *test_data, GString *string,
                             guint indent)
{
    CutTestDataPrivate *priv;

    priv = CUT_TEST_DATA_GET_PRIVATE(test_data);

    cut_utils_append_indent(string, indent);
    g_string_append_printf(string, "<test-data>\n");

    if (priv->name)
        cut_utils_append_xml_element_with_value(string, indent + 2,
                                                "name", priv->name);

    cut_utils_append_indent(string, indent);
    g_string_append_printf(string, "</test-data>\n");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
