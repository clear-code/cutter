/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2009  Kouhei Sutou <kou@clear-code.com>
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
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-test-result.h"
#include "cut-enum-types.h"
#include "cut-test.h"
#include "cut-test-iterator.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"
#include "cut-stream-parser.h"
#include "cut-backtrace-entry.h"
#include "cut-utils.h"
#include "cut-diff.h"

#define CUT_TEST_RESULT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_RESULT, CutTestResultPrivate))

typedef struct _CutTestResultPrivate	CutTestResultPrivate;
struct _CutTestResultPrivate
{
    CutTestResultStatus status;
    CutTest *test;
    CutTestIterator *test_iterator;
    CutTestCase *test_case;
    CutTestSuite *test_suite;
    CutTestData *test_data;
    gchar *message;
    gboolean user_set_message;
    gchar *user_message;
    gchar *system_message;
    GList *backtrace;
    GTimeVal start_time;
    gdouble elapsed;
    gchar *expected;
    gchar *actual;
    gchar *diff;
    gchar *folded_diff;
    gboolean user_set_diff;
    gboolean user_set_folded_diff;
};

enum
{
    PROP_0,
    PROP_STATUS,
    PROP_TEST,
    PROP_TEST_ITERATOR,
    PROP_TEST_CASE,
    PROP_TEST_SUITE,
    PROP_TEST_DATA,
    PROP_USER_MESSAGE,
    PROP_SYSTEM_MESSAGE,
    PROP_BACKTRACE,
    PROP_ELAPSED,
    PROP_EXPECTED,
    PROP_ACTUAL,
    PROP_DIFF,
    PROP_FOLDED_DIFF,
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
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_STATUS, spec);

    spec = g_param_spec_object("test",
                               "CutTest object",
                               "A CutTest object",
                               CUT_TYPE_TEST,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST, spec);

    spec = g_param_spec_object("test-iterator",
                               "CutTestIterator object",
                               "A CutTestIterator object",
                               CUT_TYPE_TEST_ITERATOR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_ITERATOR, spec);

    spec = g_param_spec_object("test-case",
                               "CutTestCase object",
                               "A CutTestCase object",
                               CUT_TYPE_TEST_CASE,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_CASE, spec);

    spec = g_param_spec_object("test-suite",
                               "CutTestSuite object",
                               "A CutTestSuite object",
                               CUT_TYPE_TEST_SUITE,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_SUITE, spec);

    spec = g_param_spec_object("test-data",
                               "CutTestData object",
                               "A CutTestData object",
                               CUT_TYPE_TEST_DATA,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_DATA, spec);

    spec = g_param_spec_string("user-message",
                               "User Message",
                               "The message from user of the result",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_USER_MESSAGE, spec);

    spec = g_param_spec_string("system-message",
                               "System Message",
                               "The message from system of the result",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SYSTEM_MESSAGE, spec);

    spec = g_param_spec_pointer("backtrace",
                                "backtrace",
                                "The backtrace of the result",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BACKTRACE, spec);

    spec = g_param_spec_double("elapsed",
                               "Elapsed time",
                               "The time of the result",
                               0, G_MAXDOUBLE, 0,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ELAPSED, spec);

    spec = g_param_spec_string("expected",
                               "Expected object",
                               "The inspected string of expected object",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_EXPECTED, spec);

    spec = g_param_spec_string("actual",
                               "Actual object",
                               "The inspected string of actual object",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ACTUAL, spec);

    spec = g_param_spec_string("diff",
                               "Difference",
                               "The difference between expected object "
                               "and actual object",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DIFF, spec);

    spec = g_param_spec_string("folded-diff",
                               "Folded difference",
                               "The difference between folded expected object "
                               "and folded actual object",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FOLDED_DIFF, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestResultPrivate));
}

static void
cut_test_result_init (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    priv->status = CUT_TEST_RESULT_SUCCESS;
    priv->test = NULL;
    priv->test_iterator = NULL;
    priv->test_case = NULL;
    priv->test_suite = NULL;
    priv->test_data = NULL;
    priv->message = NULL;
    priv->user_set_message = FALSE;
    priv->user_message = NULL;
    priv->system_message = NULL;
    priv->backtrace = NULL;
    priv->start_time.tv_sec = 0;
    priv->start_time.tv_usec = 0;
    priv->elapsed = 0.0;
    priv->expected = NULL;
    priv->actual = NULL;
    priv->diff = NULL;
    priv->folded_diff= NULL;
    priv->user_set_diff = FALSE;
    priv->user_set_folded_diff = FALSE;
}

static void
dispose (GObject *object)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(object);

    if (priv->test) {
        g_object_unref(priv->test);
        priv->test = NULL;
    }

    if (priv->test_iterator) {
        g_object_unref(priv->test_iterator);
        priv->test_iterator = NULL;
    }

    if (priv->test_case) {
        g_object_unref(priv->test_case);
        priv->test_case = NULL;
    }

    if (priv->test_suite) {
        g_object_unref(priv->test_suite);
        priv->test_suite = NULL;
    }

    if (priv->test_data) {
        g_object_unref(priv->test_data);
        priv->test_data = NULL;
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

    if (priv->backtrace) {
        g_list_foreach(priv->backtrace, (GFunc)g_object_unref, NULL);
        g_list_free(priv->backtrace);
        priv->backtrace = NULL;
    }

    if (priv->expected) {
        g_free(priv->expected);
        priv->expected = NULL;
    }

    if (priv->actual) {
        g_free(priv->actual);
        priv->actual = NULL;
    }

    if (priv->diff) {
        g_free(priv->diff);
        priv->diff = NULL;
    }

    if (priv->folded_diff) {
        g_free(priv->folded_diff);
        priv->folded_diff = NULL;
    }

    G_OBJECT_CLASS(cut_test_result_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestResult *result;
    CutTestResultPrivate *priv;

    result = CUT_TEST_RESULT(object);
    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    switch (prop_id) {
    case PROP_STATUS:
        priv->status = g_value_get_enum(value);
        break;
    case PROP_TEST:
        cut_test_result_set_test(result, g_value_get_object(value));
        break;
    case PROP_TEST_ITERATOR:
        cut_test_result_set_test_iterator(result,
                                          g_value_get_object(value));
        break;
    case PROP_TEST_CASE:
        cut_test_result_set_test_case(result, g_value_get_object(value));
        break;
    case PROP_TEST_SUITE:
        cut_test_result_set_test_suite(result, g_value_get_object(value));
        break;
    case PROP_TEST_DATA:
        cut_test_result_set_test_data(result, g_value_get_object(value));
        break;
    case PROP_USER_MESSAGE:
        cut_test_result_set_user_message(result, g_value_get_string(value));
        break;
    case PROP_SYSTEM_MESSAGE:
        cut_test_result_set_system_message(result, g_value_get_string(value));
        break;
    case PROP_BACKTRACE:
        cut_test_result_set_backtrace(result, g_value_get_pointer(value));
        break;
    case PROP_ELAPSED:
        priv->elapsed = g_value_get_double(value);
        break;
    case PROP_EXPECTED:
        cut_test_result_set_expected(result, g_value_get_string(value));
        break;
    case PROP_ACTUAL:
        cut_test_result_set_actual(result, g_value_get_string(value));
        break;
    case PROP_DIFF:
        cut_test_result_set_diff(result, g_value_get_string(value));
        break;
    case PROP_FOLDED_DIFF:
        cut_test_result_set_folded_diff(result, g_value_get_string(value));
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
    CutTestResult *result;
    CutTestResultPrivate *priv;

    result = CUT_TEST_RESULT(object);
    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    switch (prop_id) {
    case PROP_STATUS:
        g_value_set_enum(value, priv->status);
        break;
    case PROP_TEST:
        g_value_set_object(value, G_OBJECT(priv->test));
        break;
    case PROP_TEST_ITERATOR:
        g_value_set_object(value, G_OBJECT(priv->test_iterator));
        break;
    case PROP_TEST_CASE:
        g_value_set_object(value, G_OBJECT(priv->test_case));
        break;
    case PROP_TEST_SUITE:
        g_value_set_object(value, G_OBJECT(priv->test_suite));
        break;
    case PROP_TEST_DATA:
        g_value_set_object(value, G_OBJECT(priv->test_data));
        break;
    case PROP_USER_MESSAGE:
        g_value_set_string(value, priv->user_message);
        break;
    case PROP_SYSTEM_MESSAGE:
        g_value_set_string(value, priv->system_message);
        break;
    case PROP_BACKTRACE:
        g_value_set_pointer(value, priv->backtrace);
        break;
    case PROP_ELAPSED:
        g_value_set_double(value, priv->elapsed);
        break;
    case PROP_EXPECTED:
        g_value_set_string(value, priv->expected);
        break;
    case PROP_ACTUAL:
        g_value_set_string(value, priv->actual);
        break;
    case PROP_DIFF:
        g_value_set_string(value, cut_test_result_get_diff(result));
        break;
    case PROP_FOLDED_DIFF:
        g_value_set_string(value, cut_test_result_get_folded_diff(result));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestResult *
cut_test_result_new (CutTestResultStatus  status,
                     CutTest             *test,
                     CutTestIterator     *test_iterator,
                     CutTestCase         *test_case,
                     CutTestSuite        *test_suite,
                     CutTestData         *test_data,
                     const gchar         *user_message,
                     const gchar         *system_message,
                     const GList         *backtrace)
{
    return g_object_new(CUT_TYPE_TEST_RESULT,
                        "status", status,
                        "test", test,
                        "test-iterator", test_iterator,
                        "test-case", test_case,
                        "test-suite", test_suite,
                        "test-data", test_data,
                        "user-message", user_message,
                        "system-message", system_message,
                        "backtrace", backtrace,
                        NULL);
}

CutTestResult *
cut_test_result_new_empty (void)
{
    return cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                               NULL, NULL, NULL, NULL, NULL,
                               NULL, NULL, NULL);
}

static void
collect_result (CutStreamParser *parser, CutTestResult *result,
                gpointer user_data)
{
    CutTestResult **result_store = (CutTestResult **)user_data;

    *result_store = result;
    g_object_ref(*result_store);
}

CutTestResult *
cut_test_result_new_from_xml (const gchar *xml, gssize length, GError **error)
{
    CutStreamParser *parser;
    CutTestResult *result = NULL;

    if (!xml)
        return NULL;

    parser = cut_test_result_parser_new();
    g_signal_connect(parser, "result",
                     G_CALLBACK(collect_result), (gpointer)(&result));

    cut_stream_parser_parse(parser, xml, length, error);

    g_signal_handlers_disconnect_by_func(parser,
                                         collect_result,
                                         (gpointer)&result);
    g_object_unref(parser);

    return result;
}

CutTestResultStatus
cut_test_result_get_status (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->status;
}

static void
reset_message (CutTestResultPrivate *priv)
{
    if (priv->user_set_message)
        return;

    if (priv->message) {
        g_free(priv->message);
        priv->message = NULL;
    }
}

const gchar *
cut_test_result_get_message (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (!priv->message) {
        GString *message;
        const gchar *diff, *folded_diff;

        message = g_string_new(NULL);
        if (priv->user_message)
            g_string_append(message, priv->user_message);

        if (priv->system_message) {
            if (message->len > 0)
                g_string_append(message, "\n");
            g_string_append(message, priv->system_message);
        }

        if (priv->expected) {
            if (message->len > 0)
                g_string_append(message, "\n");
            g_string_append_printf(message, "expected: <%s>", priv->expected);
        }

        if (priv->actual) {
            if (message->len > 0)
                g_string_append(message, "\n");
            g_string_append_printf(message, "  actual: <%s>", priv->actual);
        }

        diff = cut_test_result_get_diff(result);
        if (diff) {
            if (message->len > 0)
                g_string_append(message, "\n\n");
            g_string_append(message, "diff:\n");
            g_string_append(message, diff);
        }

        folded_diff = cut_test_result_get_folded_diff(result);
        if (folded_diff) {
            if (message->len > 0)
                g_string_append(message, "\n\n");
            g_string_append(message, "folded diff:\n");
            g_string_append(message, folded_diff);
        }

        if (message->len > 0) {
            priv->message = g_string_free(message, FALSE);
        } else {
            g_string_free(message, TRUE);
        }
    }

    return priv->message;
}

CutTest *
cut_test_result_get_test (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->test;
}

CutTestIterator *
cut_test_result_get_test_iterator (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->test_iterator;
}

CutTestCase *
cut_test_result_get_test_case (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->test_case;
}

const gchar *
cut_test_result_get_test_name (CutTestResult *result)
{
    CutTestResultPrivate *priv;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test)
        return cut_test_get_full_name(priv->test);

    return NULL;
}

const gchar *
cut_test_result_get_test_iterator_name (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_iterator)
        return cut_test_get_name(CUT_TEST(priv->test_iterator));
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

CutTestSuite *
cut_test_result_get_test_suite (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->test_suite;
}

const gchar *
cut_test_result_get_test_suite_name (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_suite)
        return cut_test_get_name(CUT_TEST(priv->test_suite));
    return NULL;
}

CutTestData *
cut_test_result_get_test_data (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->test_data;
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

const GList *
cut_test_result_get_backtrace (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->backtrace;
}

void
cut_test_result_get_start_time (CutTestResult *result, GTimeVal *start_time)
{
    memcpy(start_time, &(CUT_TEST_RESULT_GET_PRIVATE(result)->start_time),
           sizeof(GTimeVal));
}

gdouble
cut_test_result_get_elapsed (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->elapsed;
}

const gchar *
cut_test_result_get_expected (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->expected;
}

const gchar *
cut_test_result_get_actual (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->actual;
}

const gchar *
cut_test_result_get_diff (CutTestResult *result)
{
    CutTestResultPrivate *priv;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->diff)
        return priv->diff;

    if (priv->expected && priv->actual) {
        priv->diff = cut_diff_readable(priv->expected, priv->actual);
        if (!cut_diff_is_interested(priv->diff)) {
            g_free(priv->diff);
            priv->diff = NULL;
        }
    }

    return priv->diff;
}

const gchar *
cut_test_result_get_folded_diff (CutTestResult *result)
{
    CutTestResultPrivate *priv;
    const gchar *diff;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->folded_diff)
        return priv->folded_diff;

    diff = cut_test_result_get_diff(result);
    if (cut_diff_need_fold(diff)) {
        priv->folded_diff =
            cut_diff_folded_readable(priv->expected, priv->actual);
    }

    return priv->folded_diff;
}

gchar *
cut_test_result_to_xml (CutTestResult *result)
{
    GString *string;

    string = g_string_new(NULL);
    cut_test_result_to_xml_string(result, string, 0);
    return g_string_free(string, FALSE);
}

static const gchar *
result_status_to_name (CutTestResultStatus status)
{
    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        return "success";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        return "notification";
        break;
      case CUT_TEST_RESULT_OMISSION:
        return "omission";
        break;
      case CUT_TEST_RESULT_PENDING:
        return "pending";
        break;
      case CUT_TEST_RESULT_FAILURE:
        return "failure";
        break;
      case CUT_TEST_RESULT_ERROR:
        return "error";
        break;
      case CUT_TEST_RESULT_CRASH:
        return "crash";
        break;
      default:
        return "unknown status";
        break;
    }
}

static void
append_backtrace_to_string (GString *string, CutTestResult *result, guint indent)
{
    CutTestResultPrivate *priv;
    GList *node;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);
    if (priv->backtrace == NULL)
        return;

    cut_utils_append_indent(string, indent);
    g_string_append(string, "<backtrace>\n");
    for (node = priv->backtrace; node; node = g_list_next(node)) {
        CutBacktraceEntry *entry = node->data;

        cut_backtrace_entry_to_xml_string(entry, string, indent + 2);
    }
    cut_utils_append_indent(string, indent);
    g_string_append(string, "</backtrace>\n");
}

static void
append_test_result_to_string (GString *string, CutTestResult *result,
                              guint indent)
{
    CutTestResultStatus status;
    GTimeVal start_time;
    gchar *elapsed_string, *start_time_string;
    const gchar *message, *expected, *actual, *diff, *folded_diff;

    status = cut_test_result_get_status(result);
    message = cut_test_result_get_message(result);
    expected = cut_test_result_get_expected(result);
    actual = cut_test_result_get_actual(result);
    diff = cut_test_result_get_diff(result);
    folded_diff = cut_test_result_get_folded_diff(result);

    cut_utils_append_xml_element_with_value(string, indent, "status",
                                            result_status_to_name(status));
    if (message)
        cut_utils_append_xml_element_with_value(string, indent,
                                                "detail", message);
    if (status != CUT_TEST_RESULT_SUCCESS)
        append_backtrace_to_string(string, result, indent);

    cut_test_result_get_start_time(result, &start_time);
    start_time_string = g_time_val_to_iso8601(&start_time);
    cut_utils_append_xml_element_with_value(string, indent, "start-time",
                                            start_time_string);
    g_free(start_time_string);

    elapsed_string =
        cut_utils_double_to_string(cut_test_result_get_elapsed(result));
    cut_utils_append_xml_element_with_value(string, indent, "elapsed",
                                            elapsed_string);
    g_free(elapsed_string);

    if (expected)
        cut_utils_append_xml_element_with_value(string, indent,
                                                "expected", expected);
    if (actual)
        cut_utils_append_xml_element_with_value(string, indent,
                                                "actual", actual);
    if (diff)
        cut_utils_append_xml_element_with_value(string, indent,
                                                "diff", diff);
    if (folded_diff)
        cut_utils_append_xml_element_with_value(string, indent,
                                                "folded-diff", folded_diff);
}

void
cut_test_result_to_xml_string (CutTestResult *result, GString *string,
                               guint indent)
{
    CutTestCase *test_case;
    CutTestIterator *test_iterator;
    CutTest *test;
    CutTestData *test_data;

    cut_utils_append_indent(string, indent);
    g_string_append(string, "<result>\n");

    test_case = cut_test_result_get_test_case(result);
    if (test_case)
        cut_test_to_xml_string(CUT_TEST(test_case), string, indent + 2);
    test_iterator = cut_test_result_get_test_iterator(result);
    if (test_iterator)
        cut_test_to_xml_string(CUT_TEST(test_iterator), string, indent + 2);
    test = cut_test_result_get_test(result);
    if (test)
        cut_test_to_xml_string(test, string, indent + 2);
    test_data = cut_test_result_get_test_data(result);
    if (test_data)
        cut_test_data_to_xml_string(test_data, string, indent + 2);
    append_test_result_to_string(string, result, indent + 2);

    cut_utils_append_indent(string, indent);
    g_string_append(string, "</result>\n");
}

const gchar *
cut_test_result_status_to_signal_name (CutTestResultStatus status)
{
    const gchar *signal_name = NULL;

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        signal_name = "success";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        signal_name = "notification";
        break;
      case CUT_TEST_RESULT_OMISSION:
        signal_name = "omission";
        break;
      case CUT_TEST_RESULT_PENDING:
        signal_name = "pending";
        break;
      case CUT_TEST_RESULT_FAILURE:
        signal_name = "failure";
        break;
      case CUT_TEST_RESULT_ERROR:
        signal_name = "error";
        break;
      case CUT_TEST_RESULT_CRASH:
        signal_name = "crash";
        break;
      default:
        signal_name = "invalid status";
        break;
    }

    return signal_name;
}

gboolean
cut_test_result_status_is_critical (CutTestResultStatus status)
{
    return status > CUT_TEST_RESULT_OMISSION;
}

void
cut_test_result_set_status (CutTestResult *result, CutTestResultStatus status)
{
    CUT_TEST_RESULT_GET_PRIVATE(result)->status = status;
}

void
cut_test_result_set_test (CutTestResult *result, CutTest *test)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test) {
        g_object_unref(priv->test);
        priv->test = NULL;
    }
    if (test)
        priv->test = g_object_ref(test);
}

void
cut_test_result_set_test_iterator (CutTestResult *result,
                                   CutTestIterator *test_iterator)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_iterator) {
        g_object_unref(priv->test_iterator);
        priv->test_iterator = NULL;
    }
    if (test_iterator)
        priv->test_iterator = g_object_ref(test_iterator);
}

void
cut_test_result_set_test_case (CutTestResult *result, CutTestCase *test_case)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_case) {
        g_object_unref(priv->test_case);
        priv->test_case = NULL;
    }
    if (test_case)
        priv->test_case = g_object_ref(test_case);
}

void
cut_test_result_set_test_suite (CutTestResult *result, CutTestSuite *test_suite)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_suite) {
        g_object_unref(priv->test_suite);
        priv->test_suite = NULL;
    }
    if (test_suite)
        priv->test_suite = g_object_ref(test_suite);
}

void
cut_test_result_set_test_data (CutTestResult *result, CutTestData *test_data)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->test_data) {
        g_object_unref(priv->test_data);
        priv->test_data = NULL;
    }
    if (test_data)
        priv->test_data = g_object_ref(test_data);
}

void
cut_test_result_set_user_message (CutTestResult *result,
                                  const gchar *user_message)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->user_message) {
        g_free(priv->user_message);
        priv->user_message = NULL;
    }
    if (user_message && user_message[0])
        priv->user_message = g_strdup(user_message);

    reset_message(priv);
}

void
cut_test_result_set_message (CutTestResult *result,
                             const gchar *message)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->message) {
        g_free(priv->message);
        priv->message = NULL;
    }

    if (message) {
        priv->message = g_strdup(message);
        priv->user_set_message = TRUE;
    } else {
        priv->user_set_message = FALSE;
    }
}

void
cut_test_result_set_system_message (CutTestResult *result,
                                    const gchar *system_message)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->system_message) {
        g_free(priv->system_message);
        priv->system_message = NULL;
    }
    if (system_message && system_message[0])
        priv->system_message = g_strdup(system_message);

    reset_message(priv);
}

void
cut_test_result_set_backtrace (CutTestResult *result, const GList *backtrace)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->backtrace) {
        g_list_foreach(priv->backtrace, (GFunc)g_object_unref, NULL);
        g_list_free(priv->backtrace);
        priv->backtrace = NULL;
    }
    priv->backtrace = g_list_copy((GList *)backtrace);
    g_list_foreach(priv->backtrace, (GFunc)g_object_ref, NULL);
}

void
cut_test_result_set_start_time (CutTestResult *result, GTimeVal *start_time)
{
    memcpy(&(CUT_TEST_RESULT_GET_PRIVATE(result)->start_time), start_time,
           sizeof(GTimeVal));
}

void
cut_test_result_set_elapsed (CutTestResult *result,
                             gdouble elapsed)
{
    CUT_TEST_RESULT_GET_PRIVATE(result)->elapsed = elapsed;
}

static void
reset_diff (CutTestResultPrivate *priv)
{
    gboolean need_message_regeneration = FALSE;

    if (!priv->user_set_diff) {
        if (priv->diff)
            g_free(priv->diff);
        priv->diff = NULL;
        need_message_regeneration = TRUE;
    }

    if (!priv->user_set_folded_diff) {
        if (priv->folded_diff)
            g_free(priv->folded_diff);
        priv->folded_diff = NULL;
        need_message_regeneration = TRUE;
    }

    if (need_message_regeneration)
        reset_message(priv);
}

void
cut_test_result_set_expected (CutTestResult *result, const gchar *expected)
{
    CutTestResultPrivate *priv;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->expected) {
        g_free(priv->expected);
        priv->expected = NULL;
    }

    if (expected && expected[0])
        priv->expected = g_strdup(expected);

    if (priv->expected)
        reset_diff(priv);
}

void
cut_test_result_set_actual (CutTestResult *result, const gchar *actual)
{
    CutTestResultPrivate *priv;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->actual) {
        g_free(priv->actual);
        priv->actual = NULL;
    }

    if (actual && actual[0])
        priv->actual = g_strdup(actual);

    if (priv->actual)
        reset_diff(priv);
}

void
cut_test_result_set_diff (CutTestResult *result, const gchar *diff)
{
    CutTestResultPrivate *priv;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->diff) {
        g_free(priv->diff);
        priv->diff = NULL;
    }

    if (diff && diff[0]) {
        priv->diff = g_strdup(diff);
        priv->user_set_diff = TRUE;
    } else {
        priv->user_set_diff = FALSE;
    }
}

void
cut_test_result_set_folded_diff (CutTestResult *result, const gchar *folded_diff)
{
    CutTestResultPrivate *priv;

    priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    if (priv->folded_diff) {
        g_free(priv->folded_diff);
        priv->folded_diff = NULL;
    }

    if (folded_diff && folded_diff[0]) {
        priv->folded_diff = g_strdup(folded_diff);
        priv->user_set_folded_diff = TRUE;
    } else {
        priv->user_set_folded_diff = FALSE;
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
