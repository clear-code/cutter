/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

#include "cut-test-result.h"
#include "cut-enum-types.h"
#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"

#define CUT_TEST_RESULT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_RESULT, CutTestResultPrivate))

typedef struct _CutTestResultPrivate	CutTestResultPrivate;
struct _CutTestResultPrivate
{
    CutTestResultStatus status;
    CutTest *test;
    CutTestCase *test_case;
    CutTestSuite *test_suite;
    gchar *message;
    gchar *user_message;
    gchar *system_message;
    gchar *function_name;
    gchar *filename;
    guint line;
    gdouble elapsed;
};

enum
{
    PROP_0,
    PROP_STATUS,
    PROP_TEST,
    PROP_TEST_CASE,
    PROP_TEST_SUITE,
    PROP_USER_MESSAGE,
    PROP_SYSTEM_MESSAGE,
    PROP_FUNCTION_NAME,
    PROP_FILENAME,
    PROP_LINE,
    PROP_ELAPSED,
};


G_DEFINE_TYPE (CutTestResult, cut_test_result, G_TYPE_OBJECT)

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
cut_test_result_class_init (CutTestResultClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_enum("status",
                             "Status",
                             "The status of the result",
                             CUT_TYPE_TEST_RESULT_STATUS,
                             CUT_TEST_RESULT_SUCCESS,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_STATUS, spec);

    spec = g_param_spec_object("test",
                               "CutTest object",
                               "A CutTest object",
                               CUT_TYPE_TEST,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST, spec);

    spec = g_param_spec_object("test-case",
                               "CutTestCase object",
                               "A CutTestCase object",
                               CUT_TYPE_TEST_CASE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_CASE, spec);

    spec = g_param_spec_object("test-suite",
                               "CutTestSuite object",
                               "A CutTestSuite object",
                               CUT_TYPE_TEST_SUITE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_SUITE, spec);

    spec = g_param_spec_string("user-message",
                               "User Message",
                               "The message from user of the result",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_USER_MESSAGE, spec);

    spec = g_param_spec_string("system-message",
                               "System Message",
                               "The message from system of the result",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SYSTEM_MESSAGE, spec);

    spec = g_param_spec_string("function-name",
                               "Function name",
                               "The function name of the result",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_FUNCTION_NAME, spec);

    spec = g_param_spec_string("filename",
                               "Filename",
                               "The filename of the result",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_FILENAME, spec);

    spec = g_param_spec_uint("line",
                             "Line number",
                             "The line number of the result",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LINE, spec);

    spec = g_param_spec_double("elapsed",
                               "Elapsed time",
                               "The time of the result",
                               0, G_MAXDOUBLE, 0,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ELAPSED, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestResultPrivate));
}

static void
cut_test_result_init (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    priv->status = CUT_TEST_RESULT_SUCCESS;
    priv->test = NULL;
    priv->test_case = NULL;
    priv->test_suite = NULL;
    priv->message = NULL;
    priv->user_message = NULL;
    priv->system_message = NULL;
    priv->function_name = NULL;
    priv->filename = NULL;
    priv->line = 0;
    priv->elapsed = 0.0;
}

static void
dispose (GObject *object)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(object);

    if (priv->test) {
        g_object_unref(priv->test);
        priv->test = NULL;
    }

    if (priv->test_case) {
        g_object_unref(priv->test_case);
        priv->test_case = NULL;
    }

    if (priv->test_suite) {
        g_object_unref(priv->test_suite);
        priv->test_suite = NULL;
    }

    if (priv->message) {
        g_free(priv->message);
        priv->message = NULL;
    }

    if (priv->user_message) {
        g_free(priv->user_message);
        priv->user_message = NULL;
    }

    if (priv->system_message) {
        g_free(priv->system_message);
        priv->system_message = NULL;
    }

    if (priv->function_name) {
        g_free(priv->function_name);
        priv->function_name = NULL;
    }

    if (priv->filename) {
        g_free(priv->filename);
        priv->filename = NULL;
    }

    G_OBJECT_CLASS(cut_test_result_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_STATUS:
        priv->status = g_value_get_enum(value);
        break;
      case PROP_TEST:
        if (priv->test)
            g_object_unref(priv->test);
        if (g_value_get_object(value))
            priv->test = g_object_ref(g_value_get_object(value));
        break;
      case PROP_TEST_CASE:
        if (priv->test_case)
            g_object_unref(priv->test_case);
        if (g_value_get_object(value))
            priv->test_case = g_object_ref(g_value_get_object(value));
        break;
      case PROP_TEST_SUITE:
        if (priv->test_suite)
            g_object_unref(priv->test_suite);
        if (g_value_get_object(value))
            priv->test_suite = g_object_ref(g_value_get_object(value));
        break;
      case PROP_USER_MESSAGE:
        if (priv->user_message)
            g_free(priv->user_message);
        priv->user_message = g_value_dup_string(value);
        if (priv->message)
            g_free(priv->message);
        priv->message = NULL;
      case PROP_SYSTEM_MESSAGE:
        if (priv->system_message)
            g_free(priv->system_message);
        priv->system_message = g_value_dup_string(value);
        if (priv->message)
            g_free(priv->message);
        priv->message = NULL;
      case PROP_FUNCTION_NAME:
        if (priv->function_name)
            g_free(priv->function_name);
        priv->function_name = g_value_dup_string(value);
      case PROP_FILENAME:
        if (priv->filename)
            g_free(priv->filename);
        priv->filename = g_value_dup_string(value);
        break;
      case PROP_LINE:
        priv->line = g_value_get_uint(value);
        break;
      case PROP_ELAPSED:
        priv->elapsed = g_value_get_double(value);
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
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_STATUS:
        g_value_set_enum(value, priv->status);
        break;
      case PROP_TEST:
        g_value_set_object(value, G_OBJECT(priv->test));
        break;
      case PROP_TEST_CASE:
        g_value_set_object(value, G_OBJECT(priv->test_case));
        break;
      case PROP_TEST_SUITE:
        g_value_set_object(value, G_OBJECT(priv->test_suite));
        break;
      case PROP_USER_MESSAGE:
        g_value_set_string(value, priv->user_message);
        break;
      case PROP_SYSTEM_MESSAGE:
        g_value_set_string(value, priv->system_message);
        break;
      case PROP_FUNCTION_NAME:
        g_value_set_string(value, priv->function_name);
        break;
      case PROP_FILENAME:
        g_value_set_string(value, priv->filename);
        break;
      case PROP_LINE:
        g_value_set_uint(value, priv->line);
        break;
      case PROP_ELAPSED:
        g_value_set_double(value, priv->elapsed);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestResult *
cut_test_result_new (CutTestResultStatus status,
                     CutTest *test,
                     CutTestCase *test_case,
                     CutTestSuite *test_suite,
                     const gchar *user_message,
                     const gchar *system_message,
                     const gchar *function_name,
                     const gchar *filename,
                     guint line)
{
    return g_object_new(CUT_TYPE_TEST_RESULT,
                        "status", status,
                        "test", test,
                        "test-case", test_case,
                        "test-suite", test_suite,
                        "user-message", user_message,
                        "system-message", system_message,
                        "function-name", function_name,
                        "filename", filename,
                        "line", line,
                        NULL);
}

static CutTestResult *
parse_xml (const gchar *xml, gssize len)
{
    return g_object_new(CUT_TYPE_TEST_RESULT, NULL);
}

CutTestResult *
cut_test_result_new_from_xml (const gchar *xml, gssize len)
{
    gsize length = len;

    if (length < 0)
        length = strlen(xml);

    return parse_xml(xml, length);
}

CutTestResultStatus
cut_test_result_get_status (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->status;
}

const gchar *
cut_test_result_get_message (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (!priv->message && (priv->user_message || priv->system_message)) {
        if (priv->user_message && priv->system_message) {
            priv->message = g_strdup_printf("%s\n%s",
                                            priv->user_message,
                                            priv->system_message);
        } else {
            priv->message = g_strdup(priv->user_message ?
                                     priv->user_message :
                                     priv->system_message);
        }
    }

    return priv->message;
}

CutTest *
cut_test_result_get_test (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->test;
}

CutTestCase *
cut_test_result_get_test_case (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->test_case;
}

const gchar *
cut_test_result_get_test_name (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test)
        return cut_test_get_name(priv->test);
    return NULL;
}

const gchar *
cut_test_result_get_test_case_name (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_case)
        return cut_test_get_name(CUT_TEST(priv->test_case));
    return NULL;
}

const gchar *
cut_test_result_get_test_suite_name (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_suite)
        return cut_test_get_name(CUT_TEST(priv->test_suite));
    return NULL;
}

const gchar *
cut_test_result_get_user_message (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->user_message;
}

const gchar *
cut_test_result_get_system_message (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->system_message;
}

const gchar *
cut_test_result_get_function_name (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->function_name;
}

const gchar *
cut_test_result_get_filename (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->filename;
}

guint
cut_test_result_get_line (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->line;
}

gdouble
cut_test_result_get_elapsed (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->elapsed;
}

static void
append_indent (GString *string, guint indent)
{
    guint i;
    for (i = 0; i < indent; i++)
        g_string_append_c(string, ' ');

}

static void
append_element_with_value (GString *string, guint indent, const gchar *element_name, const gchar *value)
{
    gchar *escaped;
    append_indent(string, indent);
    escaped = g_markup_printf_escaped("<%s>%s</%s>\n",
                                      element_name,
                                      value,
                                      element_name);
    g_string_append(string, escaped);
    g_free(escaped);
}

static const gchar *
result_status_to_name (CutTestResultStatus status)
{
    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        return "success";
        break;
      case CUT_TEST_RESULT_FAILURE:
        return "failure";
        break;
      case CUT_TEST_RESULT_ERROR:
        return "error";
        break;
      case CUT_TEST_RESULT_PENDING:
        return "pending";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        return "notification";
        break;
      default:
        return "unknown status";
        break;
    }
}

static void
append_element_valist (GString *string, guint indent, const gchar *element_name, va_list var_args)
{
    const gchar *name;

    name = element_name;

    while (name) {
        const gchar *value = va_arg(var_args, gchar *);
        if (value)
            append_element_with_value(string, indent, name, value);
        name = va_arg(var_args, gchar *);
    }
}

static void
append_element_with_children (GString *string, guint indent,
                              const gchar *element_name,
                              const gchar *first_child_element, ...)
{
    gchar *escaped;
    va_list var_args;

    escaped = g_markup_escape_text(element_name, -1);
    append_indent(string, indent);
    g_string_append_printf(string, "<%s>\n", escaped);

    va_start(var_args, first_child_element);
    append_element_valist(string, indent + 2, first_child_element, var_args);
    va_end(var_args);

    append_indent(string, indent);
    g_string_append_printf(string, "</%s>\n", escaped);
    g_free(escaped);
}

static void
append_backtrace_to_string (GString *string, CutTestResult *result)
{
    gchar *line_string, *info_string;

    line_string = g_strdup_printf("%d", cut_test_result_get_line(result));
    info_string = g_strdup_printf("%s()", cut_test_result_get_function_name(result));

    append_element_with_children(string, 4, "backtrace",
                                 "file", cut_test_result_get_filename(result),
                                 "line", line_string,
                                 "info", info_string,
                                 NULL);
    g_free(line_string);
    g_free(info_string);
}

static void
append_test_result_to_string (GString *string, CutTestResult *result)
{
    CutTestResultStatus status;
    gchar *elapsed_string;
    const gchar *message;

    elapsed_string = g_strdup_printf("%f", cut_test_result_get_elapsed(result));
    message = cut_test_result_get_message(result);
    status = cut_test_result_get_status(result);

    append_element_with_value(string, 4, "status", result_status_to_name(status));
    if (message)
        append_element_with_value(string, 4, "detail", message);
    if (status != CUT_TEST_RESULT_SUCCESS)
        append_backtrace_to_string(string, result);
    append_element_with_value(string, 4, "elapsed", elapsed_string);
    g_free(elapsed_string);
}

static void
append_attribute (const gchar *key, const gchar *value, GString *string)
{
    guint indent = 6;

    if (!strcmp(key, "description"))
        return;

    append_indent(string, indent);
    g_string_append(string, "<option>\n");

    append_element_with_value(string, indent + 2, "name", key);
    append_element_with_value(string, indent + 2, "value", value);

    append_indent(string, indent);
    g_string_append(string, "</option>\n");
}

static void
append_test_info_to_string (GString *string, const gchar *element_name, CutTest *test)
{
    gchar *escaped;
    const gchar *description, *name;
    guint indent = 4;
    GHashTable *attributes;

    if (!test)
        return;

    escaped = g_markup_escape_text(element_name, -1);
    append_indent(string, indent);
    g_string_append_printf(string, "<%s>\n", escaped);

    name = cut_test_get_name(test);
    if (name)
        append_element_with_value(string, indent + 2, "name", name);

    description = cut_test_get_description(test);
    if (description)
        append_element_with_value(string, indent + 2, "description", description);

    attributes = (GHashTable *)cut_test_get_attributes(test);
    if (attributes)
        g_hash_table_foreach(attributes, (GHFunc)append_attribute, string);

    append_indent(string, indent);
    g_string_append_printf(string, "</%s>\n", escaped);
    g_free(escaped);
}

gchar *
cut_test_result_to_xml (CutTestResult *result)
{
    GString *xml = g_string_new("");

    g_string_append(xml, "  <result>\n");
    append_test_info_to_string(xml, "test-case",
                               CUT_TEST(cut_test_result_get_test_case(result)));
    append_test_info_to_string(xml, "test", cut_test_result_get_test(result));
    append_test_result_to_string(xml, result);
    g_string_append(xml, "  </result>\n");

    return g_string_free(xml, FALSE);
}

const gchar *
cut_test_result_status_to_signal_name (CutTestResultStatus status)
{
    const gchar *signal_name = NULL;

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        signal_name = "success";
        break;
      case CUT_TEST_RESULT_FAILURE:
        signal_name = "failure";
        break;
      case CUT_TEST_RESULT_ERROR:
        signal_name = "error";
        break;
      case CUT_TEST_RESULT_PENDING:
        signal_name = "pending";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        signal_name = "notification";
        break;
    }

    return signal_name;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
