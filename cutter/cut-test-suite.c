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
#include "cut-utils.h"

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
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET(CutTestSuiteClass, start_test_case),
                NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

	cut_test_suite_signals[COMPLETE_TEST_CASE_SIGNAL]
        = g_signal_new("complete-test-case",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST,
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

typedef struct _RunTestInfo
{
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutContext *context;
} RunTestInfo;

static gpointer
run (gpointer data)
{
    RunTestInfo *info = data;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutContext *context;
    gboolean success;

    test_suite = info->test_suite;
    test_case = info->test_case;
    context = info->context;

    g_signal_emit_by_name(test_suite, "start-test-case", test_case);
    success = cut_test_case_run(test_case, context);
    g_signal_emit_by_name(test_suite, "complete-test-case", test_case);

    g_object_unref(test_suite);
    g_object_unref(test_case);
    g_object_unref(context);
    g_free(info);

    return GINT_TO_POINTER(success);
}

static gboolean
cut_test_suite_run_test_cases (CutTestSuite *suite, CutContext *context,
                               const GList *tests)
{
    const GList *list;
    GList *node, *threads = NULL;
    gboolean use_thread;
    gboolean all_success = TRUE;

    cut_context_start_test_suite(context, suite);
    g_signal_emit_by_name(CUT_TEST(suite), "start");

    use_thread = cut_context_get_multi_thread(context);
    for (list = tests; list; list = g_list_next(list)) {
        if (!list->data)
            continue;
        if (CUT_IS_TEST_CASE(list->data)) {
            RunTestInfo *info;
            gboolean need_no_thread_run = TRUE;

            info = g_new0(RunTestInfo, 1);
            info->test_suite = g_object_ref(suite);
            info->test_case = g_object_ref(CUT_TEST_CASE(list->data));
            info->context = g_object_ref(context);

            if (use_thread) {
                GThread *thread;
                GError *error = NULL;

                thread = g_thread_create(run, info, TRUE, &error);
                if (error) {
                    g_warning("%s(%d)", error->message, error->code);
                    g_error_free(error);
                } else {
                    need_no_thread_run = FALSE;
                    threads = g_list_append(threads, thread);
                }
            }

            if (need_no_thread_run) {
                if (!run(info))
                    all_success = FALSE;
            }

        } else {
            g_warning("This object is not test case!");
        }
    }

    for (node = threads; node; node = g_list_next(node)) {
        GThread *thread = node->data;

        if (!GPOINTER_TO_INT(g_thread_join(thread)))
            all_success = FALSE;
    }

    if (all_success)
        g_signal_emit_by_name(CUT_TEST(suite), "success");

    g_signal_emit_by_name(CUT_TEST(suite), "complete");

    return all_success;
}

gboolean
cut_test_suite_run (CutTestSuite *suite, CutContext *context)
{
    CutTestContainer *container;
    const GList *tests;

    container = CUT_TEST_CONTAINER(suite);
    tests = cut_test_container_get_children(container);

    return cut_test_suite_run_test_cases(suite, context, tests);
}

static GList *
collect_test_cases_with_regex (const GList *tests, gchar *pattern)
{
    GList *matched_list = NULL, *list;
    GRegex *regex;

    if (!strlen(pattern))
        return NULL;

    regex = g_regex_new(pattern, G_REGEX_EXTENDED, 0, NULL);
    for (list = (GList *)tests; list; list = g_list_next(list)) {
        gboolean match;
        CutTestCase *test_case = CUT_TEST_CASE(list->data);
        match = g_regex_match(regex, 
                              cut_test_get_name(CUT_TEST(test_case)),
                              0, NULL);
        if (match) {
            matched_list = g_list_prepend(matched_list, test_case);
        }
    }
    g_regex_unref(regex);

    return matched_list;
}

static GList *
cut_test_suite_collect_test_cases (CutTestSuite *suite, const gchar *name)
{
    GList *matched_tests = NULL;
    const GList *tests;
    gchar *pattern;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), NULL);

    tests = cut_test_container_get_children(CUT_TEST_CONTAINER(suite));

    pattern = cut_utils_create_regex_pattern(name);
    matched_tests = collect_test_cases_with_regex(tests, pattern);
    g_free(pattern);

    return matched_tests;
}

gboolean
cut_test_suite_run_test_case (CutTestSuite *suite, CutContext *context,
                              const gchar *test_case_name)
{
    const GList *test_cases;
    GList *matched_tests;
    gchar *pattern;
    gboolean success = FALSE;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_cases = cut_test_container_get_children(CUT_TEST_CONTAINER(suite));

    pattern = cut_utils_create_regex_pattern(test_case_name);
    matched_tests = collect_test_cases_with_regex(test_cases, pattern);
    if (matched_tests) {
        success = cut_test_suite_run_test_cases(suite, context, matched_tests);
        g_list_free(matched_tests);
    }
    g_free(pattern);

    return success;
}

static gboolean
cut_test_suite_run_function_in_test_cases (CutTestSuite *suite,
                                           CutContext *context,
                                           const gchar *function_name,
                                           const GList *test_cases)
{
    gboolean all_success = TRUE;
    const GList *list;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    cut_context_start_test_suite(context, suite);
    g_signal_emit_by_name(CUT_TEST(suite), "start");

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
cut_test_suite_run_test_function (CutTestSuite *suite, CutContext *context,
                                  const gchar *function_name)
{
    const GList *test_cases;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_cases = cut_test_container_get_children(CUT_TEST_CONTAINER(suite));
    return cut_test_suite_run_function_in_test_cases(suite, context, 
                                              function_name, test_cases);
}

gboolean
cut_test_suite_run_test_function_in_test_case (CutTestSuite *suite,
                                               CutContext   *context,
                                               const gchar *function_name,
                                               const gchar *test_case_name)
{
    GList *test_cases;
    gboolean success = FALSE;

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_cases = cut_test_suite_collect_test_cases(suite, test_case_name);
    if (!test_cases)
        return FALSE;

    success = cut_test_suite_run_function_in_test_cases(suite,
                                                        context,
                                                        function_name,
                                                        test_cases);

    g_list_free(test_cases);

    return success;
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
