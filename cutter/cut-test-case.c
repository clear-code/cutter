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
#include "cut-utils.h"

#define CUT_TEST_CASE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CASE, CutTestCasePrivate))

typedef struct _CutTestCasePrivate	CutTestCasePrivate;
struct _CutTestCasePrivate
{
    gchar *name;
    CutSetupFunction setup;
    CutTearDownFunction teardown;
    CutGetCurrentTestContextFunction get_current_test_context;
    CutSetCurrentTestContextFunction set_current_test_context;
};

enum
{
    PROP_0,
    PROP_NAME,
    PROP_SETUP_FUNCTION,
    PROP_TEAR_DOWN_FUNCTION,
    PROP_GET_CURRENT_TEST_CONTEXT_FUNCTION,
    PROP_SET_CURRENT_TEST_CONTEXT_FUNCTION
};

enum
{
    START_TEST_SIGNAL,
    COMPLETE_TEST_SIGNAL,
    LAST_SIGNAL
};

static gint cut_test_case_signals[LAST_SIGNAL] = {0};

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

    spec = g_param_spec_pointer("get-current-test-context-function",
                                "Get current test context function",
                                "The function for getting current test context",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class,
                                    PROP_GET_CURRENT_TEST_CONTEXT_FUNCTION,
                                    spec);

    spec = g_param_spec_pointer("set-current-test-context-function",
                                "Set current test context function",
                                "The function for setting current test context",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class,
                                    PROP_SET_CURRENT_TEST_CONTEXT_FUNCTION,
                                    spec);


	cut_test_case_signals[START_TEST_SIGNAL]
        = g_signal_new("start-test",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET(CutTestCaseClass, start_test),
                NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE, 1, CUT_TYPE_TEST);

	cut_test_case_signals[COMPLETE_TEST_SIGNAL]
        = g_signal_new("complete-test",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET(CutTestCaseClass, complete_test),
                NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE, 1, CUT_TYPE_TEST);

    g_type_class_add_private(gobject_class, sizeof(CutTestCasePrivate));
}

static void
cut_test_case_init (CutTestCase *test_case)
{
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(test_case);

    priv->setup = NULL;
    priv->teardown = NULL;
    priv->get_current_test_context = NULL;
    priv->set_current_test_context = NULL;
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
      case PROP_GET_CURRENT_TEST_CONTEXT_FUNCTION:
        priv->get_current_test_context = g_value_get_pointer(value);
        break;
      case PROP_SET_CURRENT_TEST_CONTEXT_FUNCTION:
        priv->set_current_test_context = g_value_get_pointer(value);
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
      case PROP_GET_CURRENT_TEST_CONTEXT_FUNCTION:
        g_value_set_pointer(value, priv->get_current_test_context);
        break;
      case PROP_SET_CURRENT_TEST_CONTEXT_FUNCTION:
        g_value_set_pointer(value, priv->set_current_test_context);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestCase *
cut_test_case_new (const gchar *name,
                   CutSetupFunction setup_function,
                   CutTearDownFunction teardown_function,
                   CutGetCurrentTestContextFunction get_current_test_context_function,
                   CutSetCurrentTestContextFunction set_current_test_context_function)
{
    return g_object_new(CUT_TYPE_TEST_CASE,
                        "name", name,
                        "setup-function", setup_function,
                        "tear-down-function", teardown_function,
                        "get-current-test-context-function",
                        get_current_test_context_function,
                        "set-current-test-context-function",
                        set_current_test_context_function,
                        NULL);
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

static gboolean
run (CutTestCase *test_case, CutTest *test, CutContext *context)
{
    CutTestCasePrivate *priv;
    CutTestContext *original_test_context, *test_context;
    gboolean success = TRUE;

    priv = CUT_TEST_CASE_GET_PRIVATE(test_case);
    if (!priv->get_current_test_context ||
        !priv->set_current_test_context) {
        g_warning("You should include <cutter.h>");
        return FALSE;
    }

    test_context = cut_test_context_new(NULL, test_case, NULL);
    original_test_context = priv->get_current_test_context();
    priv->set_current_test_context(test_context);

    g_signal_emit_by_name(test_case, "start-test", test);
    if (priv->setup)
        priv->setup();

    if (cut_test_context_is_failed(test_context)) {
        success = FALSE;
    } else {
        cut_test_context_set_test(test_context, test);
        success = cut_test_run(test, context);
        cut_test_context_set_test(test_context, NULL);
    }

    if (priv->teardown)
        priv->teardown();
    g_signal_emit_by_name(test_case, "complete-test", test);

    priv->set_current_test_context(original_test_context);
    g_object_unref(test_context);

    return success;
}

static GList *
collect_tests_with_regex (const GList *tests, gchar *pattern)
{
    GList *matched_list = NULL, *list;
    GRegex *regex;

    if (!strlen(pattern))
        return NULL;

    regex = g_regex_new(pattern, G_REGEX_EXTENDED, 0, NULL);
    for (list = (GList *)tests; list; list = g_list_next(list)) {
        gboolean match;
        CutTest *test = CUT_TEST(list->data);
        match = g_regex_match(regex, 
                              cut_test_get_name(test),
                              0, NULL);
        if (match) {
            matched_list = g_list_prepend(matched_list, test);
        }
    }
    g_regex_unref(regex);

    return matched_list;
}

static GList *
cut_test_case_collect_tests (CutTestCase *test_case, const gchar *name)
{
    GList *matched_tests = NULL;
    const GList *tests;
    gchar *pattern;

    g_return_val_if_fail(CUT_IS_TEST_CASE(test_case), NULL);

    tests = cut_test_container_get_children(CUT_TEST_CONTAINER(test_case));

    pattern = cut_utils_create_regex_pattern(name);
    matched_tests = collect_tests_with_regex(tests, pattern);
    g_free(pattern);

    return matched_tests;
}

static gboolean
cut_test_case_run_tests (CutTestCase *test_case, CutContext *context,
                         const GList *tests)
{
    const GList *list;
    gboolean all_success = TRUE;

    cut_context_start_test_case(context, test_case);
    g_signal_emit_by_name(CUT_TEST(test_case), "start");

    for (list = tests; list; list = g_list_next(list)) {
        if (!list->data)
            continue;

        if (CUT_IS_TEST(list->data)) {
            if (!run(test_case, CUT_TEST(list->data), context))
                all_success = FALSE;
        } else {
            g_warning("This object is not CutTest object");
        }
    }

    if (all_success)
        g_signal_emit_by_name(CUT_TEST(test_case), "success");

    g_signal_emit_by_name(CUT_TEST(test_case), "complete");

    return all_success;
}

gboolean
cut_test_case_run_function (CutTestCase *test_case, CutContext *context,
                            const gchar *name)
{
    GList *matched_tests;
    gboolean success = FALSE;

    g_return_val_if_fail(CUT_IS_TEST_CASE(test_case), FALSE);

    matched_tests = cut_test_case_collect_tests(test_case, name);
    if (matched_tests) {
        success = cut_test_case_run_tests(test_case, context, matched_tests);
        g_list_free(matched_tests);
    }

    return success;
}

gboolean
cut_test_case_run (CutTestCase *test_case, CutContext *context)
{
    CutTestContainer *container;
    const GList *tests;

    container = CUT_TEST_CONTAINER(test_case);
    tests = cut_test_container_get_children(container);

    return cut_test_case_run_tests(test_case, context, tests);
}

gboolean
cut_test_case_has_function (CutTestCase *test_case, const gchar *function_name)
{
    GList *matched_tests;
    gboolean found = FALSE;

    g_return_val_if_fail(CUT_IS_TEST_CASE(test_case), FALSE);

    matched_tests = cut_test_case_collect_tests(test_case, function_name);
    if (matched_tests) {
        found = TRUE;
        g_list_free(matched_tests);
    }

    return found;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
