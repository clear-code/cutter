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

guint
cut_test_iterator_get_n_tests (CutTestIterator *test_iterator)
{
    CutTestContainer *container;

    container = CUT_TEST_CONTAINER(test_iterator);
    return g_list_length(cut_test_container_get_children(container));
}

void
cut_test_iterator_add_test (CutTestIterator *test_iterator,
                            CutIteratedTest *test)
{
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_iterator),
                                CUT_TEST(test));
}

static gboolean
run_test (CutTestIterator *test_iterator, CutIteratedTest *test,
          CutTestContext *test_context, CutRunContext *run_context)
{
    CutTestCase *test_case;
    CutTestContext *original_test_context, *local_test_context;
    gboolean success = TRUE;
    gboolean is_multi_thread;

    if (cut_run_context_is_canceled(run_context))
        return TRUE;

    test_case = cut_test_context_get_test_case(test_context);

    local_test_context = cut_test_context_new(NULL, test_case, CUT_TEST(test));

    is_multi_thread = cut_run_context_is_multi_thread(run_context);
    cut_test_context_set_multi_thread(local_test_context, is_multi_thread);
    cut_test_context_set_data(local_test_context,
                              cut_test_context_get_current_data(test_context));

    original_test_context = cut_test_case_get_current_test_context(test_case);
    cut_test_case_set_current_test_context(test_case, local_test_context);

    g_signal_emit_by_name(test_iterator, "start-test", test,
                          local_test_context);

    cut_test_case_run_setup(test_case, local_test_context);
    if (cut_test_context_is_failed(local_test_context)) {
        success = FALSE;
    } else {
        cut_test_context_set_test(test_context, CUT_TEST(test));
        cut_test_context_set_test(local_test_context, CUT_TEST(test));
        success = cut_test_run(CUT_TEST(test), local_test_context, run_context);
        cut_test_context_set_test(local_test_context, NULL);
        cut_test_context_set_test(test_context, NULL);
    }
    cut_test_case_run_teardown(test_case, local_test_context);

    /* need? */
    cut_test_context_set_failed(test_context,
                                cut_test_context_is_failed(local_test_context));

    g_signal_emit_by_name(test_iterator, "complete-test", test,
                          local_test_context);

    cut_test_case_set_current_test_context(test_case, original_test_context);
    g_object_unref(local_test_context);

    return success;
}

static void
cb_test_status (CutTest *test, CutTestContext *context, CutTestResult *result,
                gpointer data)
{
    CutTestResultStatus *status = data;

    *status = MAX(*status, cut_test_result_get_status(result));
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

    if (priv->data_setup_function) {
        cut_test_context_set_jump(test_context, &jump_buffer);
        if (setjmp(jump_buffer) == 0) {
            priv->data_setup_function();
        }
    }

    if (cut_test_context_is_failed(test_context))
        return FALSE;

    cut_run_context_prepare_test_iterator(run_context, test_iterator);
    g_signal_emit_by_name(test_iterator, "ready",
                          cut_test_context_get_n_data(test_context));
    g_signal_emit_by_name(CUT_TEST(test_iterator), "start");

    while (cut_test_context_have_data(test_context)) {
        CutIteratedTest *test;
        CutTestData *test_data;

        test = cut_iterated_test_new(cut_test_get_name(CUT_TEST(test_iterator)),
                                     priv->iterated_test_function);
        if (!test)
            continue;
        cut_test_iterator_add_test(test_iterator, test);
        g_object_unref(test);

        g_signal_connect(test, "success", G_CALLBACK(cb_test_status),
                         &status);
        g_signal_connect(test, "failure", G_CALLBACK(cb_test_status),
                         &status);
        g_signal_connect(test, "error", G_CALLBACK(cb_test_status),
                         &status);
        g_signal_connect(test, "pending", G_CALLBACK(cb_test_status),
                         &status);
        g_signal_connect(test, "notification", G_CALLBACK(cb_test_status),
                         &status);
        g_signal_connect(test, "omission", G_CALLBACK(cb_test_status),
                         &status);

        if (!run_test(test_iterator, test, test_context, run_context))
            all_success = FALSE;

        g_signal_handlers_disconnect_by_func(test,
                                             G_CALLBACK(cb_test_status),
                                             &status);

        test_data = cut_test_context_get_current_data(test_context);
        cut_test_data_set_value(test_data, NULL, NULL);

        cut_test_context_shift_data(test_context);
    }

    test_case = cut_test_context_get_test_case(test_context);
    result = cut_test_result_new(status,
                                 CUT_TEST(test_iterator), test_case, NULL, NULL,
                                 NULL, NULL,
                                 NULL, NULL, 0);
    g_signal_emit_by_name(CUT_TEST(test_iterator),
                          cut_test_result_status_to_signal_name(status),
                          NULL, result);
    g_object_unref(result);
    g_signal_emit_by_name(CUT_TEST(test_iterator), "complete");

    return all_success;
}


/*
vi:nowrap:ai:expandtab:sw=4
*/
