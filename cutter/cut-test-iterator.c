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

#include "cut-test-iterator.h"

#include "cut-run-context.h"

#include "cut-marshalers.h"
#include "cut-test-result.h"

#include "../gcutter/gcut-error.h"

#define CUT_TEST_ITERATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_ITERATOR, CutTestIteratorPrivate))

typedef struct _CutTestIteratorPrivate	CutTestIteratorPrivate;
struct _CutTestIteratorPrivate
{
    CutIteratedTestFunction iterated_test_function;
    CutDataSetupFunction data_setup_function;
};

enum
{
    PROP_0,
    PROP_ITERATED_TEST_FUNCTION,
    PROP_DATA_SETUP_FUNCTION
};

enum
{
    READY_SIGNAL,
    START_TEST_SIGNAL,
    COMPLETE_TEST_SIGNAL,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE(CutTestIterator, cut_test_iterator, CUT_TYPE_TEST_CONTAINER)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static gboolean run        (CutTest        *test,
                            CutTestContext *test_context,
                            CutRunContext  *run_context);

static void
cut_test_iterator_class_init (CutTestIteratorClass *klass)
{
    GObjectClass *gobject_class;
    CutTestClass *test_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);
    test_class = CUT_TEST_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    test_class->run = run;

    spec = g_param_spec_pointer("iterated-test-function",
                                "Iterated Test Function",
                                "The function for iterated test",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_ITERATED_TEST_FUNCTION,
                                    spec);

    spec = g_param_spec_pointer("data-setup-function",
                                "Data Setup Function",
                                "The function for data setup",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_DATA_SETUP_FUNCTION,
                                    spec);

    signals[READY_SIGNAL]
        = g_signal_new("ready",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestIteratorClass, ready),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__UINT,
                       G_TYPE_NONE, 1, G_TYPE_UINT);

    signals[START_TEST_SIGNAL]
        = g_signal_new("start-test",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET(CutTestIteratorClass, start_test),
                NULL, NULL,
                _cut_marshal_VOID__OBJECT_OBJECT,
                G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    signals[COMPLETE_TEST_SIGNAL]
        = g_signal_new("complete-test",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET(CutTestIteratorClass, complete_test),
                NULL, NULL,
                _cut_marshal_VOID__OBJECT_OBJECT,
                G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    g_type_class_add_private(gobject_class, sizeof(CutTestIteratorPrivate));
}

static void
cut_test_iterator_init (CutTestIterator *test_iterator)
{
    CutTestIteratorPrivate *priv = CUT_TEST_ITERATOR_GET_PRIVATE(test_iterator);

    priv->iterated_test_function = NULL;
    priv->data_setup_function = NULL;
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_test_iterator_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestIteratorPrivate *priv = CUT_TEST_ITERATOR_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_ITERATED_TEST_FUNCTION:
        priv->iterated_test_function = g_value_get_pointer(value);
        break;
      case PROP_DATA_SETUP_FUNCTION:
        priv->data_setup_function = g_value_get_pointer(value);
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
    CutTestIteratorPrivate *priv = CUT_TEST_ITERATOR_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_ITERATED_TEST_FUNCTION:
        g_value_set_pointer(value, priv->iterated_test_function);
        break;
      case PROP_DATA_SETUP_FUNCTION:
        g_value_set_pointer(value, priv->data_setup_function);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestIterator *
cut_test_iterator_new (const gchar *name,
                       CutIteratedTestFunction test_function,
                       CutDataSetupFunction data_setup_function)
{
    return g_object_new(CUT_TYPE_TEST_ITERATOR,
                        "element-name", "test-iterator",
                        "name", name,
                        "iterated-test-function", test_function,
                        "data-setup-function", data_setup_function,
                        NULL);
}

CutTestIterator *
cut_test_iterator_new_empty (void)
{
    return cut_test_iterator_new(NULL, NULL, NULL);
}

void
cut_test_iterator_add_test (CutTestIterator *test_iterator,
                            CutIteratedTest *test)
{
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_iterator),
                                CUT_TEST(test));
}

typedef struct _RunTestInfo
{
    CutRunContext *run_context;
    CutTestCase *test_case;
    CutTestIterator *test_iterator;
    CutIteratedTest *iterated_test;
    CutTestContext *test_context;
    CutTestContext *parent_test_context;
    CutTestContext *original_test_context;
} RunTestInfo;

static void
run_test_without_thread (gpointer data, gpointer user_data)
{
    RunTestInfo *info = data;
    CutRunContext *run_context;
    CutTestCase *test_case;
    CutTestIterator *test_iterator;
    CutIteratedTest *iterated_test;
    CutTestContext *test_context, *parent_test_context, *original_test_context;
    gboolean *success = user_data;

    run_context = info->run_context;
    test_case = info->test_case;
    test_iterator = info->test_iterator;
    iterated_test = info->iterated_test;
    test_context = info->test_context;
    parent_test_context = info->parent_test_context;
    original_test_context = info->original_test_context;

    if (cut_run_context_is_canceled(run_context))
        return;

    cut_test_case_set_current_test_context(test_case, test_context);

    g_signal_emit_by_name(test_iterator, "start-test",
                          iterated_test, test_context);

    cut_test_case_run_setup(test_case, test_context);
    if (cut_test_context_is_failed(test_context)) {
        *success = FALSE;
    } else {
        if (!cut_test_run(CUT_TEST(iterated_test), test_context, run_context))
            *success = FALSE;
    }
    cut_test_case_run_teardown(test_case, test_context);

    cut_test_context_set_failed(parent_test_context,
                                cut_test_context_is_failed(test_context));

    g_signal_emit_by_name(test_iterator, "complete-test",
                          iterated_test, test_context);

    cut_test_context_set_test(test_context, NULL);
    cut_test_context_set_test(parent_test_context, NULL);
    cut_test_case_set_current_test_context(test_case, original_test_context);

    g_object_unref(run_context);
    g_object_unref(test_case);
    g_object_unref(test_iterator);
    g_object_unref(iterated_test);
    g_object_unref(test_context);
    g_object_unref(parent_test_context);
    g_object_unref(original_test_context);
    g_free(info);
}

static void
run_test_with_thread_support (CutTestIterator *test_iterator,
                              CutIteratedTest *iterated_test,
                              CutTestContext *test_context,
                              CutRunContext *run_context,
                              GThreadPool *thread_pool,
                              gboolean *success)
{
    RunTestInfo *info;
    CutTest *test;
    CutTestCase *test_case;
    CutTestContext *original_test_context, *local_test_context;
    gboolean is_multi_thread;
    gboolean need_no_thread_run = TRUE;
    const gchar *multi_thread_attribute;

    if (cut_run_context_is_canceled(run_context))
        return;

    test = CUT_TEST(iterated_test);
    test_case = cut_test_context_get_test_case(test_context);
    local_test_context = cut_test_context_new(run_context,
                                              NULL, test_case, test_iterator,
                                              test);
    is_multi_thread = cut_run_context_is_multi_thread(run_context);
    multi_thread_attribute = cut_test_get_attribute(CUT_TEST(test_iterator),
                                                    "multi-thread");
    if (multi_thread_attribute &&
        g_str_equal(multi_thread_attribute, "false"))
        is_multi_thread = FALSE;
    cut_test_context_set_multi_thread(local_test_context, is_multi_thread);
    cut_test_context_set_data(local_test_context,
                              cut_iterated_test_get_data(iterated_test));

    original_test_context = cut_test_case_get_current_test_context(test_case);

    cut_test_context_set_test(test_context, test);
    cut_test_context_set_test(local_test_context, test);

    info = g_new0(RunTestInfo, 1);
    info->run_context = g_object_ref(run_context);
    info->test_case = g_object_ref(test_case);
    info->test_iterator = g_object_ref(test_iterator);
    info->iterated_test = g_object_ref(iterated_test);
    info->test_context = local_test_context;
    info->parent_test_context = g_object_ref(test_context);
    info->original_test_context = g_object_ref(original_test_context);
    if (is_multi_thread && thread_pool) {
        GError *error = NULL;

        g_thread_pool_push(thread_pool, info, &error);
        if (error) {
            gchar *inspected;

            inspected = gcut_error_inspect(error);
            g_warning("%s", inspected);
            g_free(inspected);
            g_error_free(error);
        } else {
            need_no_thread_run = FALSE;
        }
    }

    if (need_no_thread_run)
        run_test_without_thread(info, success);
}

static void
cb_test_status (CutTest *test, CutTestContext *context, CutTestResult *result,
                gpointer data)
{
    CutTestResultStatus *status = data;

    *status = MAX(*status, cut_test_result_get_status(result));
}

static void
run_iterated_tests (CutTest *test, CutTestContext *test_context,
                    CutRunContext *run_context,
                    CutTestResultStatus *status, gboolean *all_success)
{
    CutTestIteratorPrivate *priv;
    CutTestIterator *test_iterator;
    CutTestContainer *test_container;
    GError *error = NULL;
    GList *node, *iterated_tests = NULL, *filtered_tests = NULL;
    const gchar **test_names;
    guint n_tests;
    GThreadPool *thread_pool = NULL;

    thread_pool = g_thread_pool_new(run_test_without_thread,
                                    all_success,
                                    cut_run_context_get_max_threads(run_context),
                                    FALSE,
                                    &error);
    if (error) {
        gchar *inspected;

        inspected = gcut_error_inspect(error);
        g_warning("%s", inspected);
        g_free(inspected);
        g_error_free(error);
        return;
    }

    priv = CUT_TEST_ITERATOR_GET_PRIVATE(test);
    test_iterator = CUT_TEST_ITERATOR(test);
    while (cut_test_context_have_data(test_context)) {
        CutIteratedTest *iterated_test;
        CutTestData *test_data;

        test_data = cut_test_context_get_current_data(test_context);
        iterated_test = cut_iterated_test_new(cut_test_get_name(test),
                                              priv->iterated_test_function,
                                              test_data);
        cut_test_iterator_add_test(test_iterator, iterated_test);
        g_object_unref(iterated_test);

        iterated_tests = g_list_prepend(iterated_tests, iterated_test);

        g_signal_connect(iterated_test, "success",
                         G_CALLBACK(cb_test_status), status);
        g_signal_connect(iterated_test, "failure",
                         G_CALLBACK(cb_test_status), status);
        g_signal_connect(iterated_test, "error",
                         G_CALLBACK(cb_test_status), status);
        g_signal_connect(iterated_test, "pending",
                         G_CALLBACK(cb_test_status), status);
        g_signal_connect(iterated_test, "notification",
                         G_CALLBACK(cb_test_status), status);
        g_signal_connect(iterated_test, "omission",
                         G_CALLBACK(cb_test_status), status);

        cut_test_context_shift_data(test_context);
    }

    test_container = CUT_TEST_CONTAINER(test);
    test_names = cut_run_context_get_target_test_names(run_context);
    filtered_tests = cut_test_container_filter_children(test_container,
                                                        test_names);

    cut_run_context_prepare_test_iterator(run_context, test_iterator);
    n_tests = cut_test_container_get_n_tests(test_container, run_context);
    g_signal_emit_by_name(test_iterator, "ready", n_tests);
    g_signal_emit_by_name(test, "start");

    for (node = filtered_tests; node; node = g_list_next(node)) {
        CutIteratedTest *iterated_test = node->data;

        run_test_with_thread_support(test_iterator, iterated_test,
                                     test_context, run_context,
                                     thread_pool, all_success);
    }
    g_list_free(filtered_tests);

    if (thread_pool)
        g_thread_pool_free(thread_pool, FALSE, TRUE);

    for (node = iterated_tests; node; node = g_list_next(node)) {
        CutIteratedTest *iterated_test = node->data;

        g_signal_handlers_disconnect_by_func(iterated_test,
                                             G_CALLBACK(cb_test_status),
                                             status);
        cut_iterated_test_clear_data(iterated_test);
    }
    g_list_free(iterated_tests);
}

static gboolean
run (CutTest *test, CutTestContext *test_context, CutRunContext *run_context)
{
    CutTestIterator *test_iterator;
    CutTestIteratorPrivate *priv;
    CutTestResult *result;
    CutTestCase *test_case;
    CutTestResultStatus status = CUT_TEST_RESULT_SUCCESS;
    gboolean all_success = TRUE;
    jmp_buf jump_buffer;

    test_iterator = CUT_TEST_ITERATOR(test);
    g_return_val_if_fail(CUT_IS_TEST_ITERATOR(test_iterator), FALSE);

    priv = CUT_TEST_ITERATOR_GET_PRIVATE(test_iterator);
    g_return_val_if_fail(priv->data_setup_function != NULL, FALSE);

    cut_test_context_set_test_iterator(test_context, test_iterator);
    if (priv->data_setup_function) {
        cut_test_context_set_jump(test_context, &jump_buffer);
        if (setjmp(jump_buffer) == 0) {
            priv->data_setup_function();
        }
    }

    if (cut_test_context_is_failed(test_context)) {
        cut_test_context_set_test_iterator(test_context, NULL);
        return FALSE;
    }

    run_iterated_tests(test, test_context, run_context,
                       &status, &all_success);

    test_case = cut_test_context_get_test_case(test_context);
    result = cut_test_result_new(status,
                                 NULL, test_iterator, test_case, NULL, NULL,
                                 NULL, NULL,
                                 NULL, NULL, 0);
    cut_test_emit_result_signal(CUT_TEST(test_iterator), test_context, result);
    g_object_unref(result);
    g_signal_emit_by_name(CUT_TEST(test_iterator), "complete");

    cut_test_context_set_test_iterator(test_context, NULL);

    return all_success;
}


/*
vi:nowrap:ai:expandtab:sw=4
*/
