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
#include "cut-context.h"
#include "cut-context-private.h"

#define CUT_TEST_CASE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CASE, CutTestCasePrivate))

typedef struct _CutTestCasePrivate	CutTestCasePrivate;
struct _CutTestCasePrivate
{
    gchar *name;
    CutSetupFunction setup;
    CutTearDownFunction teardown;
};

enum
{
    PROP_0,
    PROP_NAME,
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

static gboolean real_run   (CutTest         *test);

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

    spec = g_param_spec_string("name",
                               "name",
                               "The name of the test case",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_NAME, spec);

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
    priv->teardown = NULL;
    priv->name = NULL;
}

static void
dispose (GObject *object)
{
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(object);

    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

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
      case PROP_NAME:
        if (priv->name)
            g_free(priv->name);
        priv->name = g_value_dup_string(value);
        break;
      case PROP_SETUP_FUNCTION:
        priv->setup = g_value_get_pointer(value);
        break;
      case PROP_TEAR_DOWN_FUNCTION:
        priv->teardown = g_value_get_pointer(value);
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
      case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
      case PROP_SETUP_FUNCTION:
        g_value_set_pointer(value, priv->setup);
        break;
      case PROP_TEAR_DOWN_FUNCTION:
        g_value_set_pointer(value, priv->teardown);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestCase *
cut_test_case_new (const gchar *name,
                   CutSetupFunction setup_function,
                   CutTearDownFunction teardown_function)
{
    return g_object_new(CUT_TYPE_TEST_CASE,
                        "name", name,
                        "setup-function", setup_function,
                        "tear-down-function", teardown_function, NULL);
}

guint
cut_test_case_get_test_count (CutTestCase *test_case)
{
    GList *tests;

    tests = (GList*) cut_test_container_get_children(CUT_TEST_CONTAINER(test_case));

    return g_list_length(tests);
}

const gchar *
cut_test_case_get_name (CutTestCase *test_case)
{
    return CUT_TEST_CASE_GET_PRIVATE(test_case)->name;
}

void
cut_test_case_add_test (CutTestCase *test_case, CutTest *test)
{
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_case), test);
}

gboolean
cut_test_case_run_function (CutTestCase *test_case, const gchar *name)
{
    return FALSE;
}

static gboolean
real_run (CutTest *test)
{
    CutTestCasePrivate *priv;
    GList *list, *tests;
    guint assertion_count;
    gboolean all_success = TRUE;

    g_return_val_if_fail(CUT_IS_TEST_CASE(test), FALSE);

    tests = (GList*) cut_test_container_get_children(CUT_TEST_CONTAINER(test));
    priv = CUT_TEST_CASE_GET_PRIVATE(test);

    for (list = tests; list; list = g_list_next(list)) {
        if (!list->data)
            continue;
        if (CUT_IS_TEST(list->data)) {
            CutTest *test = CUT_TEST(list->data);
            gboolean success;

            cut_context_set_test(cut_context_get_current(), test);

            if (priv->setup)
                priv->setup();

            success = cut_test_run(test);
            if (!success) {
                cut_context_output_error_log(cut_context_get_current());
                all_success = FALSE;
            } else {
                cut_context_output_normal_log(cut_context_get_current());
            }
            assertion_count = cut_test_get_assertion_count(test);

            if (priv->teardown)
                priv->teardown();
        } else {
            g_warning("This object is not CutTest object");
        }
    }

    return all_success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
