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
#include "cut-context.h"

#define CUT_TEST_SUITE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_SUITE, CutTestSuitePrivate))

typedef struct _CutTestSuitePrivate	CutTestSuitePrivate;
struct _CutTestSuitePrivate
{
};

enum
{
    PROP_0
};

enum
{
    START_TEST_CASE_SIGNAL,
    COMPLETE_TEST_CASE_SIGNAL,
    LAST_SIGNAL
};

static gint cut_test_suite_signals[LAST_SIGNAL] = {0};

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

	cut_test_suite_signals[START_TEST_CASE_SIGNAL]
        = g_signal_new("start-test-case",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET(CutTestSuiteClass, start_test_case),
                NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

	cut_test_suite_signals[COMPLETE_TEST_CASE_SIGNAL]
        = g_signal_new("complete-test-case",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                G_STRUCT_OFFSET(CutTestSuiteClass, complete_test_case),
                NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);
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

gboolean
cut_test_suite_run (CutTestSuite *suite, CutContext *context)
{

    CutTestContainer *container;
    const GList *list, *tests;
    gboolean all_success = TRUE;

    cut_context_start_test_suite(context, suite);
    g_signal_emit_by_name(CUT_TEST(suite), "start");

    container = CUT_TEST_CONTAINER(suite);
    tests = cut_test_container_get_children(container);
    for (list = tests; list; list = g_list_next(list)) {
        if (!list->data)
            continue;
        if (CUT_IS_TEST_CASE(list->data)) {
            CutTestCase *test_case = CUT_TEST_CASE(list->data);

            g_signal_emit_by_name(suite, "start-test-case", test_case);
            if (!cut_test_case_run(test_case, context))
                all_success = FALSE;
            g_signal_emit_by_name(suite, "complete-test-case", test_case);
        } else {
            g_warning("This object is not test case!");
        }
    }

    if (all_success) {
        g_signal_emit_by_name(CUT_TEST(suite), "success");
    } else {
        g_signal_emit_by_name(CUT_TEST(suite), "failure");
    }

    g_signal_emit_by_name(CUT_TEST(suite), "complete");

    return all_success;
}

static gint
compare_test_case_name (gconstpointer a, gconstpointer b)
{
    g_return_val_if_fail(CUT_IS_TEST_CASE(a), -1);

    return strcmp(cut_test_case_get_name(CUT_TEST_CASE(a)), (gchar *) b);
}

static CutTestCase *
cut_test_suite_find_test_case (CutTestSuite *suite, const gchar *test_case_name)
{
    GList *list, *test_cases;

    test_cases = (GList*) cut_test_container_get_children(CUT_TEST_CONTAINER(suite));

    list = g_list_find_custom(test_cases, test_case_name,
                              (GCompareFunc) compare_test_case_name);

    if (!list)
        return NULL;

    return CUT_TEST_CASE(list->data);
}

gboolean
cut_test_suite_run_test_case (CutTestSuite *suite, CutContext *context,
                              const gchar *test_case_name)
{
    CutTestCase *test_case;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_case = cut_test_suite_find_test_case(suite, test_case_name);
    if (!test_case)
        return FALSE;

    return cut_test_case_run(test_case, context);
}

gboolean
cut_test_suite_run_test_function (CutTestSuite *suite, CutContext *context,
                                  const gchar *function_name)
{
    gboolean all_success = FALSE;
    const GList *list, *test_cases;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    cut_context_start_test_suite(context, suite);
    g_signal_emit_by_name(CUT_TEST(suite), "start");

    test_cases = cut_test_container_get_children(CUT_TEST_CONTAINER(suite));
    for (list = test_cases; list; list = g_list_next(list)) {
        if (!list->data)
            continue;
        if (CUT_IS_TEST_CASE(list->data)) {
            CutTestCase *test_case = CUT_TEST_CASE(list->data);
            if (cut_test_case_has_function(test_case, function_name)) {
                if (!cut_test_case_run_function(test_case,
                                                context,
                                                function_name))
                    all_success = FALSE;
            }
        }
    }
    g_signal_emit_by_name(CUT_TEST(suite), "complete");

    return all_success;
}

gboolean
cut_test_suite_run_test_function_in_test_case (CutTestSuite *suite,
                                               CutContext   *context,
                                               const gchar *function_name,
                                               const gchar *test_case_name)
{
    CutTestCase *test_case;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_case = cut_test_suite_find_test_case(suite, test_case_name);
    if (!test_case)
        return FALSE;

    return cut_test_case_run_function(test_case, context, function_name);
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
