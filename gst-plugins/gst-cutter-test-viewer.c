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

#include "gst-cutter-test-viewer.h"

#include <cutter/cut-run-context.h>
#include <cutter/cut-stream-parser.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-verbose-level.h>

#include "cut-gst-run-context.h"

static const GstElementDetails cutter_test_viewer_details =
    GST_ELEMENT_DETAILS("Cutter test viewer",
                        "Cutter test viewer",
                        "Cutter test viewer",
                        "g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>");
static GstStaticPadTemplate cutter_test_viewer_sink_template_factory =
    GST_STATIC_PAD_TEMPLATE("sink",
                            GST_PAD_SINK,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS_ANY);

#define GST_CUTTER_TEST_VIEWER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GST_TYPE_CUTTER_TEST_VIEWER, GstCutterTestViewerPrivate))

typedef struct _GstCutterTestViewerPrivate    GstCutterTestViewerPrivate;
struct _GstCutterTestViewerPrivate
{
    CutRunContext *run_context;
    CutStreamParser *parser;
    GObject *ui;
};

GST_BOILERPLATE(GstCutterTestViewer, gst_cutter_test_viewer, GstBaseSink, GST_TYPE_BASE_SINK);

enum
{
    ARG_0
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

static gboolean      start           (GstBaseSink *base_sink);
static gboolean      stop            (GstBaseSink *base_sink);
static GstFlowReturn render          (GstBaseSink *base_sink,
                                      GstBuffer   *buffer);

static void
gst_cutter_test_viewer_base_init (gpointer klass)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

    gst_element_class_add_pad_template(element_class,
        gst_static_pad_template_get(&cutter_test_viewer_sink_template_factory));

    gst_element_class_set_details(element_class, &cutter_test_viewer_details);
}

static void
gst_cutter_test_viewer_class_init (GstCutterTestViewerClass * klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstBaseSinkClass *base_sink_class = GST_BASE_SINK_CLASS(klass);

    parent_class = g_type_class_peek_parent(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    base_sink_class->start           = start;
    base_sink_class->stop            = stop;
    base_sink_class->render          = render;

    g_type_class_add_private(gobject_class, sizeof(GstCutterTestViewerPrivate));
}

static void
gst_cutter_test_viewer_init (GstCutterTestViewer *cutter_test_viewer, GstCutterTestViewerClass * klass)
{
    GstCutterTestViewerPrivate *priv = GST_CUTTER_TEST_VIEWER_GET_PRIVATE(cutter_test_viewer);

    priv->run_context = NULL;
    priv->parser = NULL;
    priv->ui = NULL;
}

static void
dispose (GObject *object)
{
    GstCutterTestViewerPrivate *priv = GST_CUTTER_TEST_VIEWER_GET_PRIVATE(object);

    if (priv->run_context) {
        g_object_unref(priv->run_context);
        priv->run_context = NULL;
    }

    if (priv->parser) {
        g_object_unref(priv->parser);
        priv->parser = NULL;
    }

    if (priv->ui) {
        g_object_unref(priv->ui);
        priv->ui = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    switch (prop_id) {
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
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gboolean
start (GstBaseSink *base_sink)
{
    GstCutterTestViewerPrivate *priv = GST_CUTTER_TEST_VIEWER_GET_PRIVATE(base_sink);

    priv->run_context = g_object_new(CUT_TYPE_GST_RUN_CONTEXT, NULL);
    priv->ui = cut_ui_new("console", 
                          "use-color", TRUE,
                          "verbose-level", CUT_VERBOSE_LEVEL_VERBOSE,
                          NULL);
    cut_listener_attach_to_run_context(CUT_LISTENER(priv->ui), priv->run_context);
    priv->parser = cut_stream_parser_new(priv->run_context);

    return TRUE;
}

static gboolean
stop (GstBaseSink *base_sink)
{
    GstCutterTestViewerPrivate *priv = GST_CUTTER_TEST_VIEWER_GET_PRIVATE(base_sink);

    cut_run_context_remove_listener(priv->run_context, CUT_LISTENER(priv->ui));

    return TRUE;
}

static GstFlowReturn
render (GstBaseSink *base_sink, GstBuffer *buffer)
{
    guint size;
    guint8 *data;
    GstCutterTestViewerPrivate *priv = GST_CUTTER_TEST_VIEWER_GET_PRIVATE(base_sink);

    size = GST_BUFFER_SIZE(buffer);
    data = GST_BUFFER_DATA(buffer);

    if (size > 0)
        cut_stream_parser_parse(priv->parser, (gchar *)data, size, NULL);

    return GST_FLOW_OK;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
