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
#include <glib/gstdio.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-streamer.h>
#include <cutter/cut-runner.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-enum-types.h>

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
    CutRunner    *runner;
};

struct _CutXMLStreamerClass
{
    CutStreamerClass parent_class;
};

enum
{
    PROP_0,
    PROP_RUNNER
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

static void attach_to_runner             (CutListener *listener,
                                          CutRunner   *runner);
static void detach_from_runner           (CutListener *listener,
                                          CutRunner   *runner);

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

    spec = g_param_spec_object("cut-runner",
                               "CutRunner object",
                               "A CutRunner object",
                               CUT_TYPE_RUNNER,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RUNNER, spec);
}

static void
init (CutXMLStreamer *streamer)
{
    streamer->runner = NULL;
}

static void
listener_init (CutListenerClass *listener)
{
    listener->attach_to_runner   = attach_to_runner;
    listener->detach_from_runner = detach_from_runner;
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

    if (streamer->runner) {
        g_object_unref(streamer->runner);
        streamer->runner = NULL;
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
      case PROP_RUNNER:
        attach_to_runner(CUT_LISTENER(streamer), CUT_RUNNER(g_value_get_object(value)));
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
      case PROP_RUNNER:
        g_value_set_object(value, G_OBJECT(streamer->runner));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
cb_ready_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests,
                     CutXMLStreamer *streamer)
{
}

static void
cb_start_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                     CutXMLStreamer *streamer)
{
    g_print("<streamer>\n");
}

static void
cb_start_test_case (CutRunner *runner, CutTestCase *test_case,
                    CutXMLStreamer *streamer)
{
}

static void
cb_start_test (CutRunner *runner, CutTest *test, CutTestContext *test_context,
               CutXMLStreamer *streamer)
{
}

static void
cb_test_signal (CutRunner      *runner,
                CutTest        *test,
                CutTestContext *test_context,
                CutTestResult  *result,
                CutXMLStreamer   *streamer)
{
    gchar *string;
    string = cut_test_result_to_xml(result);
    g_print("%s\n", string);
    g_free(string);
}

static void
cb_complete_test (CutRunner *runner, CutTest *test,
                  CutTestContext *test_context, CutXMLStreamer *streamer)
{
}

static void
cb_complete_test_case (CutRunner *runner, CutTestCase *test_case,
                       CutXMLStreamer *streamer)
{
}

static void
cb_complete_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                        CutXMLStreamer *streamer)
{
    g_print("</streamer>\n");
}

static void
cb_crashed (CutRunner *runner, const gchar *stack_trace,
            CutXMLStreamer *streamer)
{
}

static void
connect_to_runner (CutXMLStreamer *streamer, CutRunner *runner)
{
#define CONNECT(name) \
    g_signal_connect(runner, #name, G_CALLBACK(cb_ ## name), streamer)

#define CONNECT_TO_TEST(name) \
    g_signal_connect(runner, #name, G_CALLBACK(cb_test_signal), streamer)

    CONNECT(ready_test_suite);
    CONNECT(start_test_suite);
    CONNECT(start_test_case);
    CONNECT(start_test);

    CONNECT_TO_TEST(success_test);
    CONNECT_TO_TEST(failure_test);
    CONNECT_TO_TEST(error_test);
    CONNECT_TO_TEST(pending_test);
    CONNECT_TO_TEST(notification_test);

    CONNECT(complete_test);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);

    CONNECT(crashed);

#undef CONNECT
}

static void
disconnect_from_runner (CutXMLStreamer *streamer, CutRunner *runner)
{
#define DISCONNECT(name)                                               \
    g_signal_handlers_disconnect_by_func(runner,                       \
                                         G_CALLBACK(cb_ ## name),      \
                                         streamer)

    DISCONNECT(start_test_suite);
    DISCONNECT(start_test_case);
    DISCONNECT(start_test);

    DISCONNECT(complete_test);
    DISCONNECT(complete_test_case);
    DISCONNECT(complete_test_suite);

    DISCONNECT(crashed);

    g_signal_handlers_disconnect_by_func(runner,
                                         G_CALLBACK(cb_test_signal),
                                         streamer);

#undef DISCONNECT
}

static void
attach_to_runner (CutListener *listener,
                  CutRunner   *runner)
{
    CutXMLStreamer *streamer = CUT_XML_STREAMER(listener);
    if (streamer->runner)
        detach_from_runner(listener, streamer->runner);
    
    if (runner) {
        streamer->runner = g_object_ref(runner);
        connect_to_runner(CUT_XML_STREAMER(listener), runner);
    }
}

static void
detach_from_runner (CutListener *listener,
                    CutRunner   *runner)
{
    CutXMLStreamer *streamer = CUT_XML_STREAMER(listener);
    if (streamer->runner != runner)
        return;

    disconnect_from_runner(streamer, runner);
    g_object_unref(streamer->runner);
    streamer->runner = NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
