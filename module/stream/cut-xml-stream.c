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
#include <cutter/cut-stream.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-enum-types.h>
#include <cutter/cut-utils.h>

#define CUT_TYPE_XML_STREAM            cut_type_xml_stream
#define CUT_XML_STREAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_XML_STREAM, CutXMLStream))
#define CUT_XML_STREAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_XML_STREAM, CutXMLStreamClass))
#define CUT_IS_XML_STREAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_XML_STREAM))
#define CUT_IS_XML_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_XML_STREAM))
#define CUT_XML_STREAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_XML_STREAM, CutXMLStreamClass))

typedef struct _CutXMLStream CutXMLStream;
typedef struct _CutXMLStreamClass CutXMLStreamClass;

struct _CutXMLStream
{
    CutStream   object;
    CutRunContext    *run_context;
    GMutex *mutex;
    CutStreamFunction stream_function;
    gpointer stream_function_user_data;
    GDestroyNotify stream_function_user_data_destroy_function;
};

struct _CutXMLStreamClass
{
    CutStreamClass parent_class;
};

enum
{
    PROP_0,
    PROP_RUN_CONTEXT,
    PROP_STREAM_FUNCTION,
    PROP_STREAM_FUNCTION_USER_DATA,
    PROP_STREAM_FUNCTION_USER_DATA_DESTROY_FUNCTION
};

static GType cut_type_xml_stream = 0;
static CutStreamClass *parent_class;

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
class_init (CutXMLStreamClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;
    CutStreamClass *stream_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    stream_class = CUT_STREAM_CLASS(klass);

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
init (CutXMLStream *stream)
{
    stream->run_context = NULL;
    stream->mutex = g_mutex_new();
    stream->stream_function = NULL;
    stream->stream_function_user_data = NULL;
    stream->stream_function_user_data_destroy_function = NULL;
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
            sizeof (CutXMLStreamClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutXMLStream),
            0,
            (GInstanceInitFunc) init,
        };

    static const GInterfaceInfo listener_info =
        {
            (GInterfaceInitFunc) listener_init,
            NULL,
            NULL
        };

    cut_type_xml_stream =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_STREAM,
                                    "CutXMLStream",
                                    &info, 0);

    g_type_module_add_interface(type_module,
                                cut_type_xml_stream,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_xml_stream)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_xml_stream));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_XML_STREAM, first_property, var_args);
}

static void
dispose (GObject *object)
{
    CutXMLStream *stream = CUT_XML_STREAM(object);

    if (stream->run_context) {
        g_object_unref(stream->run_context);
        stream->run_context = NULL;
    }

    if (stream->mutex) {
        g_mutex_free(stream->mutex);
        stream->mutex = NULL;
    }

    if (stream->stream_function_user_data) {
        if (stream->stream_function_user_data_destroy_function)
            stream->stream_function_user_data_destroy_function(stream->stream_function_user_data);
        stream->stream_function_user_data = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutXMLStream *stream = CUT_XML_STREAM(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        attach_to_run_context(CUT_LISTENER(stream),
                              CUT_RUN_CONTEXT(g_value_get_object(value)));
        break;
      case PROP_STREAM_FUNCTION:
        stream->stream_function = g_value_get_pointer(value);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA:
        stream->stream_function_user_data = g_value_get_pointer(value);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA_DESTROY_FUNCTION:
        stream->stream_function_user_data_destroy_function =
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
    CutXMLStream *stream = CUT_XML_STREAM(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        g_value_set_object(value, G_OBJECT(stream->run_context));
        break;
      case PROP_STREAM_FUNCTION:
        g_value_set_pointer(value, stream->stream_function);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA:
        g_value_set_pointer(value, stream->stream_function_user_data);
        break;
      case PROP_STREAM_FUNCTION_USER_DATA_DESTROY_FUNCTION:
        g_value_set_pointer(value, stream->stream_function_user_data_destroy_function);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
flow (CutXMLStream *stream, const gchar *format, ...)
{
    GError *error = NULL;
    gchar *message;
    va_list va_args;

    if (!stream->stream_function)
        return;

    va_start(va_args, format);
    message = g_strdup_vprintf(format, va_args);
    va_end(va_args);

    g_mutex_lock(stream->mutex);
    stream->stream_function(message, &error,
                              stream->stream_function_user_data);
    g_mutex_unlock(stream->mutex);

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
cb_start_run (CutRunContext *run_context, CutXMLStream *stream)
{
    flow(stream, "<stream>\n");
}

static void
cb_ready_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests,
                     CutXMLStream *stream)
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

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test-suite>\n");
    cut_test_to_xml_string(CUT_TEST(test_suite), string, 4);
    g_string_append(string, "  </start-test-suite>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_ready_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    guint n_tests, CutXMLStream *stream)
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

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test-case>\n");
    cut_test_to_xml_string(CUT_TEST(test_case), string, 4);
    g_string_append(string, "  </start-test-case>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_ready_test_iterator (CutRunContext *run_context,
                        CutTestIterator *test_iterator,
                        guint n_tests, CutXMLStream *stream)
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

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test_iterator (CutRunContext *run_context,
                        CutTestIterator *test_iterator,
                        CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test-iterator>\n");
    cut_test_to_xml_string(CUT_TEST(test_iterator), string, 4);
    g_string_append(string, "  </start-test-iterator>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_iterated_test (CutRunContext *run_context,
                        CutIteratedTest *iterated_test,
                        CutTestContext *test_context,
                        CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-iterated-test>\n");
    cut_test_to_xml_string(CUT_TEST(iterated_test), string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    g_string_append(string, "  </start-iterated-test>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_start_test (CutRunContext *run_context, CutTest *test,
               CutTestContext *test_context, CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <start-test>\n");
    cut_test_to_xml_string(test, string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    g_string_append(string, "  </start-test>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_pass_assertion (CutRunContext *run_context, CutTest *test,
                   CutTestContext *test_context, CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <pass-assertion>\n");
    cut_test_to_xml_string(test, string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    g_string_append(string, "  </pass-assertion>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_test_result (CutRunContext  *run_context,
                CutTest        *test,
                CutTestContext *test_context,
                CutTestResult  *result,
                CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <test-result>\n");
    cut_test_to_xml_string(test, string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    cut_test_result_to_xml_string(result, string, 4);
    g_string_append(string, "  </test-result>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test (CutRunContext *run_context, CutTest *test,
                  CutTestContext *test_context, CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test>\n");
    cut_test_to_xml_string(test, string, 4);
    if (test_context)
        cut_test_context_to_xml_string(test_context, string, 4);
    g_string_append(string, "  </complete-test>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_iterated_test (CutRunContext *run_context,
                           CutIteratedTest *iterated_test,
                           CutTestContext *test_context,
                           CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-iterated-test>\n");
    cut_test_to_xml_string(CUT_TEST(iterated_test), string, 4);
    cut_test_context_to_xml_string(test_context, string, 4);
    g_string_append(string, "  </complete-iterated-test>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_test_iterator_result (CutRunContext  *run_context,
                         CutTestIterator *test_iterator,
                         CutTestResult *result,
                         CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <test-iterator-result>\n");
    cut_test_to_xml_string(CUT_TEST(test_iterator), string, 4);
    cut_test_result_to_xml_string(result, string, 4);
    g_string_append(string, "  </test-iterator-result>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test_iterator (CutRunContext *run_context,
                           CutTestIterator *test_iterator,
                           CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test-iterator>\n");
    cut_test_to_xml_string(CUT_TEST(test_iterator), string, 4);
    g_string_append(string, "  </complete-test-iterator>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_test_case_result (CutRunContext  *run_context,
                     CutTestCase    *test_case,
                     CutTestResult  *result,
                     CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <test-case-result>\n");
    cut_test_to_xml_string(CUT_TEST(test_case), string, 4);
    cut_test_result_to_xml_string(result, string, 4);
    g_string_append(string, "  </test-case-result>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test_case (CutRunContext *run_context, CutTestCase *test_case,
                       CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test-case>\n");
    cut_test_to_xml_string(CUT_TEST(test_case), string, 4);
    g_string_append(string, "  </complete-test-case>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                        CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <complete-test-suite>\n");
    cut_test_to_xml_string(CUT_TEST(test_suite), string, 4);
    g_string_append(string, "  </complete-test-suite>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
cb_complete_run (CutRunContext *run_context, gboolean success,
                 CutXMLStream *stream)
{
    flow(stream, "  <success>%s</success>\n", success ? "true" : "false");
    flow(stream, "</stream>\n");
}

static void
cb_crashed (CutRunContext *run_context, const gchar *backtrace,
            CutXMLStream *stream)
{
    GString *string;

    string = g_string_new(NULL);

    g_string_append(string, "  <crashed>\n");
    cut_utils_append_xml_element_with_value(string, 4, "backtrace", backtrace);
    g_string_append(string, "  </crashed>\n");

    flow(stream, "%s", string->str);

    g_string_free(string, TRUE);
}

static void
connect_to_run_context (CutXMLStream *stream, CutRunContext *run_context)
{
#define CONNECT(name)                                                   \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), stream)

#define CONNECT_TO_TEST(name)                                           \
    g_signal_connect(run_context, #name "_test",                        \
                     G_CALLBACK(cb_test_result), stream)

#define CONNECT_TO_TEST_CASE(name)                                      \
    g_signal_connect(run_context, #name "_test_case",                   \
                     G_CALLBACK(cb_test_case_result), stream)

#define CONNECT_TO_TEST_ITERATOR(name)                                  \
    g_signal_connect(run_context, #name "_test_iterator",               \
                     G_CALLBACK(cb_test_iterator_result), stream)

    CONNECT(start_run);
    CONNECT(ready_test_suite);
    CONNECT(start_test_suite);
    CONNECT(ready_test_case);
    CONNECT(start_test_case);
    CONNECT(ready_test_iterator);
    CONNECT(start_test_iterator);
    CONNECT(start_iterated_test);
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
    CONNECT(complete_iterated_test);
    CONNECT(complete_test_iterator);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);
    CONNECT(complete_run);

    CONNECT(crashed);

#undef CONNECT
}

static void
disconnect_from_run_context (CutXMLStream *stream,
                             CutRunContext *run_context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(run_context,                   \
                                         G_CALLBACK(cb_ ## name),       \
                                         stream)
    DISCONNECT(start_run);
    DISCONNECT(ready_test_suite);
    DISCONNECT(start_test_suite);
    DISCONNECT(ready_test_case);
    DISCONNECT(start_test_case);
    DISCONNECT(ready_test_iterator);
    DISCONNECT(start_test_iterator);
    DISCONNECT(start_iterated_test);
    DISCONNECT(start_test);

    DISCONNECT(pass_assertion);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_result),
                                         stream);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_iterator_result),
                                         stream);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_case_result),
                                         stream);

    DISCONNECT(complete_test);
    DISCONNECT(complete_iterated_test);
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
    CutXMLStream *stream = CUT_XML_STREAM(listener);
    if (stream->run_context)
        detach_from_run_context(listener, stream->run_context);
    
    if (run_context) {
        stream->run_context = g_object_ref(run_context);
        connect_to_run_context(CUT_XML_STREAM(listener), run_context);
    }
}

static void
detach_from_run_context (CutListener *listener,
                    CutRunContext   *run_context)
{
    CutXMLStream *stream = CUT_XML_STREAM(listener);
    if (stream->run_context != run_context)
        return;

    disconnect_from_run_context(stream, run_context);
    g_object_unref(stream->run_context);
    stream->run_context = NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
