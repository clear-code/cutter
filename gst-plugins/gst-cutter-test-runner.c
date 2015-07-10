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
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>

#include "gst-cutter-test-runner.h"

#include <cutter/cut-test-runner.h>
#include <cutter/cut-stream.h>
#include <cutter/cut-listener.h>

GST_DEBUG_CATEGORY_STATIC(cutter_test_runner_debug);
#define GST_CAT_DEFAULT cutter_test_runner_debug

static GstStaticPadTemplate cutter_test_runner_src_template_factory =
    GST_STATIC_PAD_TEMPLATE("src",
                            GST_PAD_SRC,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS_ANY);

#define GST_CUTTER_TEST_RUNNER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GST_TYPE_CUTTER_TEST_RUNNER, GstCutterTestRunnerPrivate))

typedef struct _GstCutterTestRunnerPrivate    GstCutterTestRunnerPrivate;
struct _GstCutterTestRunnerPrivate
{
    CutRunContext *run_context;
    CutStream *cut_stream;
    gchar *test_directory;
    GString *xml_string;
};

#define gst_cutter_test_runner_parent_class parent_class
G_DEFINE_TYPE(GstCutterTestRunner, gst_cutter_test_runner, GST_TYPE_BASE_SRC);

enum
{
    ARG_0,
    ARG_TEST_DIRECTORY
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

static GstStateChangeReturn change_state (GstElement *element,
                                          GstStateChange transition);

static void
gst_cutter_test_runner_class_init (GstCutterTestRunnerClass * klass)
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

    spec = g_param_spec_string("test-directory",
                               "Test directory",
                               "The directory name in which test cases are stored",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, ARG_TEST_DIRECTORY, spec);

    g_type_class_add_private(gobject_class, sizeof(GstCutterTestRunnerPrivate));

    GST_DEBUG_CATEGORY_INIT(cutter_test_runner_debug, "cutter-test", 0, "Cutter test elements");

    gst_element_class_add_pad_template(element_class,
        gst_static_pad_template_get(&cutter_test_runner_src_template_factory));

    gst_element_class_set_metadata(element_class,
                                   "Cutter test runner",
                                   "Cutter test runner",
                                   "Cutter test runner",
                                   "g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>");
}

static void
gst_cutter_test_runner_init (GstCutterTestRunner *cutter_test_runner)
{
    GstCutterTestRunnerPrivate *priv = GST_CUTTER_TEST_RUNNER_GET_PRIVATE(cutter_test_runner);

    priv->run_context = NULL;
    priv->cut_stream = NULL;
    priv->test_directory = NULL;
    priv->xml_string = NULL;
}

static void
dispose (GObject *object)
{
    GstCutterTestRunnerPrivate *priv = GST_CUTTER_TEST_RUNNER_GET_PRIVATE(object);

    if (priv->run_context) {
        g_object_unref(priv->run_context);
        priv->run_context = NULL;
    }

    if (priv->cut_stream) {
        g_object_unref(priv->cut_stream);
        priv->cut_stream = NULL;
    }

    if (priv->test_directory) {
        g_free(priv->test_directory);
        priv->test_directory = NULL;
    }

    if (priv->xml_string) {
        g_string_free(priv->xml_string, TRUE);
        priv->xml_string = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GstCutterTestRunnerPrivate *priv = GST_CUTTER_TEST_RUNNER_GET_PRIVATE(object);

    switch (prop_id) {
      case ARG_TEST_DIRECTORY:
        priv->test_directory = g_value_dup_string(value);
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
    GstCutterTestRunnerPrivate *priv = GST_CUTTER_TEST_RUNNER_GET_PRIVATE(object);

    switch (prop_id) {
      case ARG_TEST_DIRECTORY:
        g_value_set_string(value, priv->test_directory);
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
    GstCutterTestRunnerPrivate *priv = GST_CUTTER_TEST_RUNNER_GET_PRIVATE(base_src);

    priv->run_context = g_object_new(CUT_TYPE_TEST_RUNNER,
                                     "test-directory", priv->test_directory,
                                     NULL);
    priv->xml_string = g_string_new(NULL);
    priv->cut_stream =
        cut_stream_new("xml",
                       "stream-function", stream_to_string,
                       "stream-function-user-data", priv->xml_string,
                       NULL);
    cut_run_context_add_listener(priv->run_context,
                                 CUT_LISTENER(priv->cut_stream));

    gst_base_src_set_format(base_src, GST_FORMAT_BYTES);

    cut_run_context_start(priv->run_context);

    return TRUE;
}

static gboolean
stop (GstBaseSrc *base_src)
{
    GstCutterTestRunnerPrivate *priv = GST_CUTTER_TEST_RUNNER_GET_PRIVATE(base_src);

    cut_run_context_cancel(priv->run_context);
    cut_run_context_remove_listener(priv->run_context, CUT_LISTENER(priv->cut_stream));

    return TRUE;
}

static gboolean
is_seekable (GstBaseSrc *base_src)
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
    GstCutterTestRunnerPrivate *priv = GST_CUTTER_TEST_RUNNER_GET_PRIVATE(base_src);

    GST_DEBUG("create buffer");

    if (priv->xml_string->len < offset + length) {
        is_end_of_buffer = TRUE;
    }

    send_size = priv->xml_string->len - offset;

    buf = gst_buffer_new_and_alloc(send_size);
    gst_buffer_fill(buf, 0, priv->xml_string->str + offset, send_size);
    GST_BUFFER_OFFSET(buf) = offset;
    GST_BUFFER_OFFSET_END(buf) = offset + send_size;
    *buffer = buf;

    return !is_end_of_buffer ? GST_FLOW_OK : GST_FLOW_EOS;
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
