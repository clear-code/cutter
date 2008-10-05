/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2008  Kouhei Sutou <kou@cozmixng.org>
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

#define CUT_TEST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST, CutTestPrivate))

typedef struct _CutTestPrivate	CutTestPrivate;
struct _CutTestPrivate
{
    gchar *name;
    gchar *full_name;
    gchar *element_name;
    CutTestFunction test_function;
    GTimer *timer;
    GTimeVal start_time;
    gdouble elapsed;
    GHashTable *attributes;
};

enum
{
    PROP_0,
    PROP_NAME,
    PROP_ELEMENT_NAME,
    PROP_TEST_FUNCTION
};

enum
{
    START,
    PASS_ASSERTION,
    SUCCESS,
    FAILURE,
    ERROR,
    PENDING,
    NOTIFICATION,
    OMISSION,
    COMPLETE,
    CRASHED,
    LAST_SIGNAL
};

static gint cut_test_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (CutTest, cut_test, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void         start        (CutTest  *test,
                                  CutTestContext *test_context);
static gdouble      get_elapsed  (CutTest  *test);
static void         set_elapsed  (CutTest  *test, gdouble elapsed);
static gboolean     run          (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);
static gboolean     is_available (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);
static void         invoke       (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);

static void
cut_test_class_init (CutTestClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->start = start;
    klass->get_elapsed = get_elapsed;
    klass->set_elapsed = set_elapsed;
    klass->run = run;
    klass->is_available = is_available;
    klass->invoke = invoke;

    spec = g_param_spec_string("name",
                               "Test name",
                               "The name of the test",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NAME, spec);

    spec = g_param_spec_string("element-name",
                               "Element name",
                               "The element name of the test",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_ELEMENT_NAME, spec);

    spec = g_param_spec_pointer("test-function",
                                "Test Function",
                                "The function for test",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_FUNCTION, spec);

    cut_test_signals[START]
        = g_signal_new ("start",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_FIRST,
                        G_STRUCT_OFFSET (CutTestClass, start),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CONTEXT);

    cut_test_signals[PASS_ASSERTION]
        = g_signal_new ("pass-assertion",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, pass_assertion),
                        NULL, NULL,
                        g_cclosure_marshal_VOID__OBJECT,
                        G_TYPE_NONE, 1, CUT_TYPE_TEST_CONTEXT);

    cut_test_signals[SUCCESS]
        = g_signal_new ("success",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, success),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[FAILURE]
        = g_signal_new ("failure",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, failure),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[ERROR]
        = g_signal_new ("error",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, error),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[PENDING]
        = g_signal_new ("pending",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, pending),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[NOTIFICATION]
        = g_signal_new ("notification",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET (CutTestClass, notification),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[OMISSION]
        = g_signal_new("omission",
                        G_TYPE_FROM_CLASS(klass),
                        G_SIGNAL_RUN_LAST,
                        G_STRUCT_OFFSET(CutTestClass, omission),
                        NULL, NULL,
                        _cut_marshal_VOID__OBJECT_OBJECT,
                        G_TYPE_NONE, 2,
                        CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_signals[COMPLETE]
        = g_signal_new("complete",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET (CutTestClass, complete),
                       NULL, NULL,
                       _cut_marshal_VOID__OBJECT_BOOLEAN,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST_CONTEXT, G_TYPE_BOOLEAN);

    cut_test_signals[CRASHED]
        = g_signal_new("crashed",
                       G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET (CutTestClass, crashed),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__STRING,
                       G_TYPE_NONE, 1, G_TYPE_STRING);

    g_type_class_add_private(gobject_class, sizeof(CutTestPrivate));
}

static void
cut_test_init (CutTest *test)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);

    priv->full_name = NULL;

    priv->test_function = NULL;
    priv->timer = NULL;
    priv->start_time.tv_sec = 0;
    priv->start_time.tv_usec = 0;
    priv->elapsed = -1.0;
    priv->attributes = g_hash_table_new_full(g_str_hash, g_str_equal,
                                             g_free, g_free);
}

static void
free_full_name (CutTestPrivate *priv)
{
    if (priv->full_name) {
        g_free(priv->full_name);
        priv->full_name = NULL;
    }
}

static void
dispose (GObject *object)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

    if (priv->element_name) {
        g_free(priv->element_name);
        priv->element_name = NULL;
    }

    priv->test_function = NULL;

    if (priv->timer) {
        g_timer_destroy(priv->timer);
        priv->timer = NULL;
    }

    if (priv->attributes) {
        g_hash_table_unref(priv->attributes);
        priv->attributes = NULL;
    }

    free_full_name(priv);

    G_OBJECT_CLASS(cut_test_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_NAME:
        cut_test_set_name(CUT_TEST(object), g_value_get_string(value));
        break;
      case PROP_ELEMENT_NAME:
        if (priv->element_name) {
            g_free(priv->element_name);
            priv->element_name = NULL;
        }
        priv->element_name = g_value_dup_string(value);
        break;
      case PROP_TEST_FUNCTION:
        priv->test_function = g_value_get_pointer(value);
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
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
      case PROP_ELEMENT_NAME:
        g_value_set_string(value, priv->element_name);
        break;
      case PROP_TEST_FUNCTION:
        g_value_set_pointer(value, priv->test_function);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTest *
cut_test_new (const gchar *name, CutTestFunction function)
{
    return g_object_new(CUT_TYPE_TEST,
                        "element-name", "test",
                        "name", name,
                        "test-function", function,
                        NULL);
}

CutTest *
cut_test_new_empty (void)
{
    return cut_test_new(NULL, NULL);
}

static void
start (CutTest *test, CutTestContext *test_context)
{
    GTimeVal value;
    g_get_current_time(&value);
    cut_test_set_start_time(test, &value);
}

static gboolean
is_available (CutTest *test, CutTestContext *test_context,
              CutRunContext *run_context)
{
    return CUT_TEST_GET_PRIVATE(test)->test_function != NULL;
}

static void
invoke (CutTest *test, CutTestContext *test_context, CutRunContext *run_context)
{
    CUT_TEST_GET_PRIVATE(test)->test_function();
}

static gboolean
run (CutTest *test, CutTestContext *test_context, CutRunContext *run_context)
{
    CutTestClass *klass;
    CutTestPrivate *priv;
    gboolean success = TRUE;
    jmp_buf jump_buffer;

    priv = CUT_TEST_GET_PRIVATE(test);
    klass = CUT_TEST_GET_CLASS(test);

    if (!klass->is_available(test, test_context, run_context))
        return FALSE;

    g_signal_emit_by_name(test, "start", test_context);

    cut_test_context_set_jump(test_context, &jump_buffer);
    if (setjmp(jump_buffer) == 0) {
        if (priv->timer) {
            g_timer_start(priv->timer);
        } else {
            priv->timer = g_timer_new();
        }
        klass->invoke(test, test_context, run_context);
    }
    g_timer_stop(priv->timer);

    success = !cut_test_context_is_failed(test_context);
    if (success) {
        CutTestCase *test_case;
        CutTestIterator *test_iterator;
        CutTestResult *result;
        CutTestData *data = NULL;

        test_case = cut_test_context_get_test_case(test_context);
        test_iterator = cut_test_context_get_test_iterator(test_context);
        if (CUT_IS_ITERATED_TEST(test))
            data = cut_iterated_test_get_data(CUT_ITERATED_TEST(test));
        result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                     test, test_iterator, test_case,
                                     NULL, data,
                                     NULL, NULL,
                                     NULL, 0, NULL);
        cut_test_emit_result_signal(test, test_context, result);
        g_object_unref(result);
    }

    g_signal_emit_by_name(test, "complete", test_context, success);

    return success;
}

gboolean
cut_test_run (CutTest *test, CutTestContext *test_context,
              CutRunContext *run_context)
{
    return CUT_TEST_GET_CLASS(test)->run(test, test_context, run_context);
}

const gchar *
cut_test_get_name (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->name;
}

void
cut_test_set_name (CutTest *test, const gchar *name)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);

    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

    free_full_name(priv);

    if (name)
        priv->name = g_strdup(name);
}

const gchar *
cut_test_get_full_name (CutTest *test)
{
    CutTestPrivate *priv;
    CutTestData *data = NULL;

    priv = CUT_TEST_GET_PRIVATE(test);
    if (priv->full_name)
        return priv->full_name;

    if (CUT_IS_ITERATED_TEST(test))
        data = cut_iterated_test_get_data(CUT_ITERATED_TEST(test));

    if (priv->name && data) {
        priv->full_name = g_strconcat(priv->name,
                                      " (",
                                      cut_test_data_get_name(data),
                                      ")",
                                      NULL);
    } else if (priv->name) {
        priv->full_name = g_strdup(priv->name);
    } else if (data) {
        priv->full_name = g_strconcat(" (",
                                      cut_test_data_get_name(data),
                                      ")",
                                      NULL);
    }

    return priv->full_name;
}

const gchar *
cut_test_get_description (CutTest *test)
{
    return cut_test_get_attribute(test, "description");
}

void
cut_test_get_start_time (CutTest *test, GTimeVal *start_time)
{
    memcpy(start_time, &(CUT_TEST_GET_PRIVATE(test)->start_time),
           sizeof(GTimeVal));
}

void
cut_test_set_start_time (CutTest *test, GTimeVal *start_time)
{
    memcpy(&(CUT_TEST_GET_PRIVATE(test)->start_time), start_time,
           sizeof(GTimeVal));
}

static gdouble
get_elapsed (CutTest *test)
{
    CutTestPrivate *priv = CUT_TEST_GET_PRIVATE(test);

    if (!(priv->elapsed < 0.0))
        return priv->elapsed;

    if (priv->timer)
        return g_timer_elapsed(priv->timer, NULL);
    else
        return 0.0;
}

gdouble
cut_test_get_elapsed (CutTest *test)
{
    return CUT_TEST_GET_CLASS(test)->get_elapsed(test);
}

static void
set_elapsed (CutTest *test, gdouble elapsed)
{
    CUT_TEST_GET_PRIVATE(test)->elapsed = elapsed;
}

void
cut_test_set_elapsed (CutTest *test, gdouble elapsed)
{
    return CUT_TEST_GET_CLASS(test)->set_elapsed(test, elapsed);
}

const gchar *
cut_test_get_attribute (CutTest *test, const gchar *name)
{
    return g_hash_table_lookup(CUT_TEST_GET_PRIVATE(test)->attributes, name);
}

void
cut_test_set_attribute (CutTest *test, const gchar *name, const gchar *value)
{
    g_hash_table_replace(CUT_TEST_GET_PRIVATE(test)->attributes,
                         g_strdup(name),
                         g_strdup(value));
}

GHashTable *
cut_test_get_attributes (CutTest *test)
{
    return CUT_TEST_GET_PRIVATE(test)->attributes;
}

gchar *
cut_test_to_xml (CutTest *test)
{
    GString *string;

    string = g_string_new(NULL);
    cut_test_to_xml_string(test, string, 0);
    return g_string_free(string, FALSE);
}

typedef struct _AppendAttributeInfo {
    GString *string;
    guint indent;
} AppendAttributeInfo;

static void
append_attribute (const gchar *key, const gchar *value,
                  AppendAttributeInfo *info)
{
    if (strcmp(key, "description") == 0)
        return;

    cut_utils_append_indent(info->string, info->indent);
    g_string_append(info->string, "<option>\n");

    cut_utils_append_xml_element_with_value(info->string, info->indent + 2,
                                            "name", key);
    cut_utils_append_xml_element_with_value(info->string, info->indent + 2,
                                            "value", value);

    cut_utils_append_indent(info->string, info->indent);
    g_string_append(info->string, "</option>\n");
}

void
cut_test_to_xml_string (CutTest *test, GString *string, guint indent)
{
    CutTestPrivate *priv;
    gchar *escaped, *start_time, *elapsed;
    const gchar *description, *name;
    GHashTable *attributes;

    priv = CUT_TEST_GET_PRIVATE(test);

    escaped = g_markup_escape_text(priv->element_name, -1);
    cut_utils_append_indent(string, indent);
    g_string_append_printf(string, "<%s>\n", escaped);

    name = cut_test_get_name(test);
    if (name)
        cut_utils_append_xml_element_with_value(string, indent + 2,
                                                "name", name);

    description = cut_test_get_description(test);
    if (description)
        cut_utils_append_xml_element_with_value(string, indent + 2,
                                                "description", description);

    start_time = g_time_val_to_iso8601(&(priv->start_time));
    cut_utils_append_xml_element_with_value(string, indent + 2,
                                            "start-time", start_time);
    g_free(start_time);

    elapsed = g_strdup_printf("%f", cut_test_get_elapsed(test));
    cut_utils_append_xml_element_with_value(string, indent + 2,
                                            "elapsed", elapsed);
    g_free(elapsed);

    attributes = cut_test_get_attributes(test);
    if (attributes) {
        AppendAttributeInfo info;

        info.string = string;
        info.indent = indent + 2;
        g_hash_table_foreach(attributes, (GHFunc)append_attribute, &info);
    }

    cut_utils_append_indent(string, indent);
    g_string_append_printf(string, "</%s>\n", escaped);
    g_free(escaped);
}

void
cut_test_emit_result_signal (CutTest *test,
                             CutTestContext *test_context,
                             CutTestResult *result)
{
    CutTestPrivate *priv;
    const gchar *status_signal_name = NULL;
    CutTestResultStatus status;

    priv = CUT_TEST_GET_PRIVATE(test);
    cut_test_result_set_start_time(result, &(priv->start_time));
    cut_test_result_set_elapsed(result, cut_test_get_elapsed(test));

    status = cut_test_result_get_status(result);
    status_signal_name = cut_test_result_status_to_signal_name(status);
    g_signal_emit_by_name(test, status_signal_name, test_context, result);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
