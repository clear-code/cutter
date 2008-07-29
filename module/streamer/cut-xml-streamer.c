/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
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
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-streamer.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-enum-types.h>
#include <cutter/cut-utils.h>

#define CUT_TYPE_XML_STREAMER            cut_type_xml_streamer
#define CUT_XML_STREAMER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_XML_STREAMER, CutXMLStreamer))
#define CUT_XML_STREAMER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_XML_STREAMER, CutXMLStreamerClass))
#define CUT_IS_XML_STREAMER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_XML_STREAMER))
#define CUT_IS_XML_STREAMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_XML_STREAMER))
#define CUT_XML_STREAMER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_XML_STREAMER, CutXMLStreamerClass))

typedef struct _CutXMLStreamer CutXMLStreamer;
typedef struct _CutXMLStreamerClass CutXMLStreamerClass;

struct _CutXMLStreamer
{
    CutStreamer   object;
    CutRunContext    *run_context;
    GMutex *mutex;
    CutStreamFunction stream_function;
    gpointer stream_function_user_data;
    GDestroyNotify stream_function_user_data_destroy_function;
};

struct _CutXMLStreamerClass
{
    CutStreamerClass parent_class;
};

enum
{
    PROP_0,
    PROP_RUN_CONTEXT,
    PROP_STREAM_FUNCTION,
    PROP_STREAM_FUNCTION_USER_DATA,
    PROP_STREAM_FUNCTION_USER_DATA_DESTROY_FUNCTION
};

static GType cut_type_xml_streamer = 0;
static CutStreamerClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void attach_to_run_context             (CutListener *listener,
                                          CutRunContext   *run_context);
static void detach_from_run_context           (CutListener *listener,
                                          CutRunContext   *run_context);

static void
class_init (CutXMLStreamerClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;
    CutStreamerClass *streamer_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    streamer_class = CUT_STREAMER_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("cut-run-context",
                               "CutRunContext object",
                               "A CutRunContext object",
                               CUT_TYPE_RUN_CONTEXT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RUN_CONTEXT, spec);

    spec = g_param_spec_pointer("stream-function",
                                "Stream function",
                                "A function to stream data",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_STREAM_FUNCTION, spec);

    spec = g_param_spec_pointer("stream-function-user-data",
                                "Stream function user data",
                                "A user data to use with stream function",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class,
                                    PROP_STREAM_FUNCTION_USER_DATA, spec);

    spec = g_param_spec_pointer("stream-function-user-data-destroy-function",
                                "Destroy function for stream function user data",
                                "A function to destroy user data",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class,
                                    PROP_STREAM_FUNCTION_USER_DATA_DESTROY_FUNCTION,
                                    spec);
}

static void
init (CutXMLStreamer *streamer)
{
    streamer->run_context = NULL;
    streamer->mutex = g_mutex_new();
    streamer->stream_function = NULL;
    streamer->stream_function_user_data = NULL;
    streamer->stream_function_user_data_destroy_function = NULL;
}

static void
listener_init (CutListenerClass *listener)
{
    listener->attach_to_run_context   = attach_to_run_context;
    listener->detach_from_run_context = detach_from_run_context;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutXMLStreamerClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutXMLStreamer),
            0,
            (GInstanceInitFunc) init,
        };

    static const GInterfaceInfo listener_info =
        {
            (GInterfaceInitFunc) listener_init,
            NULL,
            NULL
        };

    cut_type_xml_streamer =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_STREAMER,
                                    "CutXMLStreamer",
                                    &info, 0);

    g_type_module_add_interface(type_module,
                                cut_type_xml_streamer,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_xml_streamer)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_xml_streamer));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_XML_STREAMER, first_property, var_args);
}

static void
dispose (GObject *object)
{
    CutXMLStreamer *streamer = CUT_XML_STREAMER(object);

    if (streamer->run_context) {
        g_object_unref(streamer->run_context);
        streamer->run_context = NULL;
    }

    if (streamer->mutex) {
        g_mutex_free(streamer->mutex);
        streamer->mutex = NULL;
    }

    if (streamer->stream_function_user_data) {
        if (streamer->stream_function_user_data_destroy_function)
            streamer->stream_function_user_data_destroy_function(streamer->stream_function_user_data);
        streamer->stream_function_user_data = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutXMLStreamer *streamer = CUT_XML_STREAMER(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        attach_to_run_context(CUT_LISTENER(streamer),
                              CUT_RUN_CONTEXT(g_value_get_object(value)));
        break;
      case PROP_STREAM_FUNCTION:
        streamer->stream_function = g_value_get_pointer(value);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA:
        streamer->stream_function_user_data = g_value_get_pointer(value);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA_DESTROY_FUNCTION:
        streamer->stream_function_user_data_destroy_function =
            g_value_get_pointer(value);
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
    CutXMLStreamer *streamer = CUT_XML_STREAMER(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        g_value_set_object(value, G_OBJECT(streamer->run_context));
        break;
      case PROP_STREAM_FUNCTION:
        g_value_set_pointer(value, streamer->stream_function);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA:
        g_value_set_pointer(value, streamer->stream_function_user_data);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA_DESTROY_FUNCTION:
        g_value_set_pointer(value, streamer->stream_function_user_data_destroy_function);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
stream (CutXMLStreamer *streamer, const gchar *format, ...)
{
    GError *error = NULL;
    gchar *message;
    va_list va_args;

    if (!streamer->stream_function)
        return;

    va_start(va_args, format);
    message = g_strdup_vprintf(format, va_args);
    va_end(va_args);

    g_mutex_lock(streamer->mutex);
    streamer->stream_function(message, &error,
                              streamer->stream_function_user_data);
    g_mutex_unlock(streamer->mutex);

    if (error) {
        g_warning("WriteError: %s:%d: %s",
                  g_quark_to_string(error->domain),
                  error->code,
                  error->message);
        g_error_free(error);
    }

    g_free(message);
}

static void
cb_start_run (CutRunContext *run_context, CutXMLStreamer *streamer)
{
    stream(streamer, "<stream>\n");
}

static void
cb_ready_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests,
                     CutXMLStreamer *streamer)
{
    GString *string;
    gchar *str;

    string = g_string_new(NULL);

    g_string_append(string, "  <ready-test-suite>\n");

    cut_test_to_xml_string(CUT_TEST(test_suite), string, 4);

    str = g_strdup_printf("%d", n_test_cases);
    cut_utils_append_xml_element_with_value(string, 4, "n-test-cases", str);
    g_free(str);

    str = g_strdup_printf("%d", n_tests);
    cut_utils_append_xml_element_with_value(string, 4, "n-tests", str);
    g_free(str);

    g_string_append(string, "  </ready-test-suite>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test-suite>\n");
    cut_test_to_xml_string(CUT_TEST(test_suite), string, 4);
    g_string_append(string, "  </start-test-suite>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_ready_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    guint n_tests, CutXMLStreamer *streamer)
{
    GString *string;
    gchar *str;

    string = g_string_new(NULL);

    g_string_append(string, "  <ready-test-case>\n");

    cut_test_to_xml_string(CUT_TEST(test_case), string, 4);

    str = g_strdup_printf("%d", n_tests);
    cut_utils_append_xml_element_with_value(string, 4, "n-tests", str);
    g_free(str);

    g_string_append(string, "  </ready-test-case>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test-case>\n");
    cut_test_to_xml_string(CUT_TEST(test_case), string, 4);
    g_string_append(string, "  </start-test-case>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_ready_test_iterator (CutRunContext *run_context,
                        CutTestIterator *test_iterator,
                        guint n_tests, CutXMLStreamer *streamer)
{
    GString *string;
    gchar *str;

    string = g_string_new(NULL);

    g_string_append(string, "  <ready-test-iterator>\n");

    cut_test_to_xml_string(CUT_TEST(test_iterator), string, 4);

    str = g_strdup_printf("%d", n_tests);
    cut_utils_append_xml_element_with_value(string, 4, "n-tests", str);
    g_free(str);

    g_string_append(string, "  </ready-test-iterator>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test_iterator (CutRunContext *run_context,
                        CutTestIterator *test_iterator,
                        CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test-iterator>\n");
    cut_test_to_xml_string(CUT_TEST(test_iterator), string, 4);
    g_string_append(string, "  </start-test-iterator>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test (CutRunContext *run_context, CutTest *test,
               CutTestContext *test_context, CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test>\n");
    cut_test_to_xml_string(test, string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    g_string_append(string, "  </start-test>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_pass_assertion (CutRunContext *run_context, CutTest *test,
                   CutTestContext *test_context, CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <pass-assertion>\n");
    cut_test_to_xml_string(test, string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    g_string_append(string, "  </pass-assertion>\n");

    stream(streamer, string->str);

    g_string_free(string, TRUE);
}

static void
cb_test_result (CutRunContext  *run_context,
                CutTest        *test,
                CutTestContext *test_context,
                CutTestResult  *result,
                CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <test-result>\n");
    cut_test_to_xml_string(test, string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    cut_test_result_to_xml_string(result, string, 4);
    g_string_append(string, "  </test-result>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test (CutRunContext *run_context, CutTest *test,
                  CutTestContext *test_context, CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test>\n");
    cut_test_to_xml_string(test, string, 4);
    g_string_append(string, "  </complete-test>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_test_iterator_result (CutRunContext  *run_context,
                         CutTestIterator *test_iterator,
                         CutTestResult  *result,
                         CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <test-iterator-result>\n");
    cut_test_to_xml_string(CUT_TEST(test_iterator), string, 4);
    cut_test_result_to_xml_string(result, string, 4);
    g_string_append(string, "  </test-iterator-result>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test_iterator (CutRunContext *run_context,
                           CutTestIterator *test_iterator,
                           CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test-iterator>\n");
    cut_test_to_xml_string(CUT_TEST(test_iterator), string, 4);
    g_string_append(string, "  </complete-test-iterator>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_test_case_result (CutRunContext  *run_context,
                     CutTestCase    *test_case,
                     CutTestResult  *result,
                     CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <test-case-result>\n");
    cut_test_to_xml_string(CUT_TEST(test_case), string, 4);
    cut_test_result_to_xml_string(result, string, 4);
    g_string_append(string, "  </test-case-result>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test_case (CutRunContext *run_context, CutTestCase *test_case,
                       CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test-case>\n");
    cut_test_to_xml_string(CUT_TEST(test_case), string, 4);
    g_string_append(string, "  </complete-test-case>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                        CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test-suite>\n");
    cut_test_to_xml_string(CUT_TEST(test_suite), string, 4);
    g_string_append(string, "  </complete-test-suite>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_run (CutRunContext *run_context, gboolean success,
                 CutXMLStreamer *streamer)
{
    stream(streamer, "  <success>%s</success>\n", success ? "true" : "false");
    stream(streamer, "</stream>\n");
}

static void
cb_crashed (CutRunContext *run_context, const gchar *backtrace,
            CutXMLStreamer *streamer)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <crashed>\n");
    cut_utils_append_xml_element_with_value(string, 4, "backtrace", backtrace);
    g_string_append(string, "  </crashed>\n");

    stream(streamer, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
connect_to_run_context (CutXMLStreamer *streamer, CutRunContext *run_context)
{
#define CONNECT(name)                                                   \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), streamer)

#define CONNECT_TO_TEST(name)                                           \
    g_signal_connect(run_context, #name "_test",                        \
                     G_CALLBACK(cb_test_result), streamer)

#define CONNECT_TO_TEST_CASE(name)                                      \
    g_signal_connect(run_context, #name "_test_case",                   \
                     G_CALLBACK(cb_test_case_result), streamer)

#define CONNECT_TO_TEST_ITERATOR(name)                                  \
    g_signal_connect(run_context, #name "_test_iterator",               \
                     G_CALLBACK(cb_test_iterator_result), streamer)

    CONNECT(start_run);
    CONNECT(ready_test_suite);
    CONNECT(start_test_suite);
    CONNECT(ready_test_case);
    CONNECT(start_test_case);
    CONNECT(ready_test_iterator);
    CONNECT(start_test_iterator);
    CONNECT(start_test);

    CONNECT(pass_assertion);

    CONNECT_TO_TEST(success);
    CONNECT_TO_TEST(failure);
    CONNECT_TO_TEST(error);
    CONNECT_TO_TEST(pending);
    CONNECT_TO_TEST(notification);
    CONNECT_TO_TEST(omission);

    CONNECT_TO_TEST_ITERATOR(success);
    CONNECT_TO_TEST_ITERATOR(failure);
    CONNECT_TO_TEST_ITERATOR(error);
    CONNECT_TO_TEST_ITERATOR(pending);
    CONNECT_TO_TEST_ITERATOR(notification);
    CONNECT_TO_TEST_ITERATOR(omission);

    CONNECT_TO_TEST_CASE(success);
    CONNECT_TO_TEST_CASE(failure);
    CONNECT_TO_TEST_CASE(error);
    CONNECT_TO_TEST_CASE(pending);
    CONNECT_TO_TEST_CASE(notification);
    CONNECT_TO_TEST_CASE(omission);

    CONNECT(complete_test);
    CONNECT(complete_test_iterator);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);
    CONNECT(complete_run);

    CONNECT(crashed);

#undef CONNECT
}

static void
disconnect_from_run_context (CutXMLStreamer *streamer,
                             CutRunContext *run_context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(run_context,                   \
                                         G_CALLBACK(cb_ ## name),       \
                                         streamer)
    DISCONNECT(start_run);
    DISCONNECT(ready_test_suite);
    DISCONNECT(start_test_suite);
    DISCONNECT(ready_test_case);
    DISCONNECT(start_test_case);
    DISCONNECT(ready_test_iterator);
    DISCONNECT(start_test_iterator);
    DISCONNECT(start_test);

    DISCONNECT(pass_assertion);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_result),
                                         streamer);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_iterator_result),
                                         streamer);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_case_result),
                                         streamer);

    DISCONNECT(complete_test);
    DISCONNECT(complete_test_iterator);
    DISCONNECT(complete_test_case);
    DISCONNECT(complete_test_suite);
    DISCONNECT(complete_run);

    DISCONNECT(crashed);

#undef DISCONNECT
}

static void
attach_to_run_context (CutListener *listener,
                  CutRunContext   *run_context)
{
    CutXMLStreamer *streamer = CUT_XML_STREAMER(listener);
    if (streamer->run_context)
        detach_from_run_context(listener, streamer->run_context);
    
    if (run_context) {
        streamer->run_context = g_object_ref(run_context);
        connect_to_run_context(CUT_XML_STREAMER(listener), run_context);
    }
}

static void
detach_from_run_context (CutListener *listener,
                    CutRunContext   *run_context)
{
    CutXMLStreamer *streamer = CUT_XML_STREAMER(listener);
    if (streamer->run_context != run_context)
        return;

    disconnect_from_run_context(streamer, run_context);
    g_object_unref(streamer->run_context);
    streamer->run_context = NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
