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

#include "cut-test-suite.h"

#include "cut-test.h"
#include "cut-test-case.h"

#define CUT_TEST_SUITE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_SUITE, CutTestSuitePrivate))

typedef struct _CutTestSuitePrivate	CutTestSuitePrivate;
struct _CutTestSuitePrivate
{
};

enum
{
    PROP_0
};

G_DEFINE_TYPE (CutTestSuite, cut_test_suite, CUT_TYPE_TEST_CONTAINER)

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
cut_test_suite_class_init (CutTestSuiteClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

}

static void
cut_test_suite_init (CutTestSuite *test_suite)
{
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_test_suite_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
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
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestSuite *
cut_test_suite_new (void)
{
    return g_object_new(CUT_TYPE_TEST_SUITE, NULL);
}

static gint
compare_test_case_name (gconstpointer a, gconstpointer b)
{
    g_return_val_if_fail(CUT_IS_TEST_CASE(a), -1);

    return strcmp(cut_test_case_get_name(CUT_TEST_CASE(a)), (gchar *) b);
}

gboolean
cut_test_suite_run_test_case (CutTestSuite *suite, const gchar *name)
{
    CutTestCase *test_case;
    GList *list, *test_cases;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_cases = (GList*) cut_test_container_get_children(CUT_TEST_CONTAINER(suite));

    list = g_list_find_custom(test_cases, name, (GCompareFunc) compare_test_case_name);

    if (!list)
        return FALSE;

    test_case = CUT_TEST_CASE(list->data);

    return cut_test_run(CUT_TEST(test_case));
}

void
cut_test_suite_add_test_case (CutTestSuite *suite, CutTestCase *test_case)
{
    g_return_if_fail(CUT_IS_TEST_CASE(test_case));
    g_return_if_fail(CUT_IS_TEST_SUITE(suite));

    cut_test_container_add_test(CUT_TEST_CONTAINER(suite), CUT_TEST(test_case));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
