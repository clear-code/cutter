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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gst-cutter-server.h"

#include <cutter/cut-test-runner.h>
#include <cutter/cut-streamer.h>
#include <cutter/cut-listener.h>

GST_DEBUG_CATEGORY_STATIC(cutter_server_debug);
#define GST_CAT_DEFAULT cutter_server_debug

static const GstElementDetails cutter_server_details =
    GST_ELEMENT_DETAILS("Cutter test server",
                        "Cutter test server",
                        "Cutter test server",
                        "g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>");
static GstStaticPadTemplate cutter_server_src_template_factory =
    GST_STATIC_PAD_TEMPLATE("src",
                            GST_PAD_SRC,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS_ANY);

#define GST_CUTTER_SERVER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GST_TYPE_CUTTER_SERVER, GstCutterServerPrivate))

typedef struct _GstCutterServerPrivate    GstCutterServerPrivate;
struct _GstCutterServerPrivate
{
    CutRunContext *run_context;
    CutStreamer *cut_streamer;

    gchar *test_directory;
    gchar *host;
    gint port;

    GString *xml_string;
    GstElement *tcp_server_sink;
};

GST_BOILERPLATE(GstCutterServer, gst_cutter_server, GstBaseSrc, GST_TYPE_BASE_SRC);

enum
{
    ARG_0,
    ARG_TEST_DIRECTORY,
    ARG_HOST,
    ARG_PORT
};

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static gboolean      start           (GstBaseSrc *base_src);
static gboolean      stop            (GstBaseSrc *base_src);
static gboolean      is_seekable     (GstBaseSrc *base_src);
static GstFlowReturn create          (GstBaseSrc *basr_src,
                                      guint64     offset,
                                      guint       length,
                                      GstBuffer **buffer);
static gboolean      check_get_range (GstBaseSrc *base_src);

static GstStateChangeReturn change_state (GstElement *element,
                                          GstStateChange transition);

static void
gst_cutter_server_base_init (gpointer klass)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

    gst_element_class_add_pad_template(element_class,
        gst_static_pad_template_get(&cutter_server_src_template_factory));

    gst_element_class_set_details(element_class, &cutter_server_details);
}

static void
gst_cutter_server_class_init (GstCutterServerClass * klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    GstBaseSrcClass *base_src_class = GST_BASE_SRC_CLASS(klass);
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    element_class->change_state = change_state;

    base_src_class->start           = start;
    base_src_class->stop            = stop;
    base_src_class->is_seekable     = is_seekable;
    base_src_class->create          = create;
    base_src_class->check_get_range = check_get_range;

    spec = g_param_spec_string("test-directory",
                               "Test directory",
                               "The directory name in which test cases are stored",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, ARG_TEST_DIRECTORY, spec);

    spec = g_param_spec_string("host",
                               "host",
                               "The host/IP to send the packets to",
                               "localhost", 
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, ARG_HOST, spec);

    spec = g_param_spec_int("port",
                            "port",
                            "The port to send the packets to",
                            0,
                            65535,
                            4953, 
                            G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, ARG_PORT, spec);

    g_type_class_add_private(gobject_class, sizeof(GstCutterServerPrivate));

    GST_DEBUG_CATEGORY_INIT(cutter_server_debug, "cutter-test", 0, "Cutter test elements");
}

static void
gst_cutter_server_init (GstCutterServer *cutter_server, GstCutterServerClass * klass)
{
    GstCutterServerPrivate *priv = GST_CUTTER_SERVER_GET_PRIVATE(cutter_server);

    priv->run_context = NULL;
    priv->cut_streamer = NULL;
    priv->test_directory = NULL;
    priv->xml_string = NULL;
    priv->port = 4953;
    priv->host = NULL;
    priv->tcp_server_sink = gst_element_factory_make("tcpserversink", NULL);
    gst_element_link(GST_ELEMENT(cutter_server), priv->tcp_server_sink);
}

static void
dispose (GObject *object)
{
    GstCutterServerPrivate *priv = GST_CUTTER_SERVER_GET_PRIVATE(object);

    if (priv->run_context) {
        g_object_unref(priv->run_context);
        priv->run_context = NULL;
    }

    if (priv->cut_streamer) {
        g_object_unref(priv->cut_streamer);
        priv->cut_streamer = NULL;
    }

    if (priv->test_directory) {
        g_free(priv->test_directory);
        priv->test_directory = NULL;
    }

    if (priv->xml_string) {
        g_string_free(priv->xml_string, TRUE);
        priv->xml_string = NULL;
    }

    if (priv->host) {
        g_free(priv->host);
        priv->host = NULL;
    }

    if (priv->tcp_server_sink) {
        gst_object_unref(priv->tcp_server_sink);
        priv->tcp_server_sink = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GstCutterServerPrivate *priv = GST_CUTTER_SERVER_GET_PRIVATE(object);

    switch (prop_id) {
      case ARG_TEST_DIRECTORY:
        priv->test_directory = g_value_dup_string(value);
        break;
      case ARG_HOST:
        priv->host = g_value_dup_string(value);
        break;
      case ARG_PORT:
        priv->port = g_value_get_int(value);
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
    GstCutterServerPrivate *priv = GST_CUTTER_SERVER_GET_PRIVATE(object);

    switch (prop_id) {
      case ARG_TEST_DIRECTORY:
        g_value_set_string(value, priv->test_directory);
      case ARG_HOST:
        g_value_set_string(value, priv->host);
      case ARG_PORT:
        g_value_set_int(value, priv->port);
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gboolean
stream_to_string (const gchar *message, GError **error, gpointer user_data)
{
    GString *string = user_data;

    g_string_append(string, message);

    return TRUE;
}

static gboolean
start (GstBaseSrc *base_src)
{
    GstCutterServerPrivate *priv = GST_CUTTER_SERVER_GET_PRIVATE(base_src);

    priv->run_context = g_object_new(CUT_TYPE_TEST_RUNNER,
                                     "test-directory", priv->test_directory,
                                     NULL);
    priv->xml_string = g_string_new(NULL);
    priv->cut_streamer =
        cut_streamer_new("xml",
                         "stream-function", stream_to_string,
                         "stream-function-user-data", priv->xml_string,
                         NULL);
    cut_run_context_add_listener(priv->run_context,
                                 CUT_LISTENER(priv->cut_streamer));

    gst_base_src_set_format(base_src, GST_FORMAT_BYTES);

    cut_run_context_start(priv->run_context);

    return TRUE;
}

static gboolean
stop (GstBaseSrc *base_src)
{
    GstCutterServerPrivate *priv = GST_CUTTER_SERVER_GET_PRIVATE(base_src);

    cut_run_context_cancel(priv->run_context);
    cut_run_context_remove_listener(priv->run_context, CUT_LISTENER(priv->cut_streamer));

    return TRUE;
}

static gboolean
is_seekable (GstBaseSrc *base_src)
{
    return FALSE;
}

static gboolean
check_get_range (GstBaseSrc *base_src)
{
    return FALSE;
}

static GstFlowReturn
create (GstBaseSrc *base_src, guint64 offset,
        guint length, GstBuffer **buffer)
{
    GstBuffer *buf;
    guint send_size;
    gboolean is_end_of_buffer = FALSE;
    GstCutterServerPrivate *priv = GST_CUTTER_SERVER_GET_PRIVATE(base_src);

    GST_DEBUG("create buffer");

    if (priv->xml_string->len < offset + length) {
        is_end_of_buffer = TRUE;
    }

    send_size = priv->xml_string->len - offset;

    buf = gst_buffer_new_and_alloc(send_size);
    memcpy(GST_BUFFER_DATA(buf), priv->xml_string->str + offset, send_size);
    GST_BUFFER_SIZE(buf) = send_size;
    GST_BUFFER_OFFSET(buf) = offset;
    GST_BUFFER_OFFSET_END(buf) = offset + send_size;
    *buffer = buf;

    return !is_end_of_buffer ? GST_FLOW_OK : GST_FLOW_UNEXPECTED;
}

static GstStateChangeReturn
change_state (GstElement *element, GstStateChange transition)
{
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

    switch (transition) {
      case GST_STATE_CHANGE_NULL_TO_READY:
        break;
      case GST_STATE_CHANGE_READY_TO_PAUSED:
        break;
      case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
        break;
      default:
        break;
    }

    ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
    if (ret == GST_STATE_CHANGE_FAILURE)
        return ret;

    switch (transition) {
      case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
        break;
      case GST_STATE_CHANGE_PAUSED_TO_READY:
        break;
      case GST_STATE_CHANGE_READY_TO_NULL:
        break;
      default:
        break;
    }

    return ret;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
