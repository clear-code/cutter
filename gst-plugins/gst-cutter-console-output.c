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

#include "gst-cutter-console-output.h"

#include <cutter/cut-run-context.h>
#include <cutter/cut-stream-parser.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-enum-types.h>

#include "cut-gst-run-context.h"

static const GstElementDetails cutter_console_output_details =
    GST_ELEMENT_DETAILS("Cutter console output",
                        "Cutter console output",
                        "Cutter console output",
                        "g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>");
static GstStaticPadTemplate cutter_console_output_sink_template_factory =
    GST_STATIC_PAD_TEMPLATE("sink",
                            GST_PAD_SINK,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS_ANY);

#define GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GST_TYPE_CUTTER_CONSOLE_OUTPUT, GstCutterConsoleOutputPrivate))

typedef struct _GstCutterConsoleOutputPrivate    GstCutterConsoleOutputPrivate;
struct _GstCutterConsoleOutputPrivate
{
    CutRunContext *run_context;
    CutStreamParser *parser;
    GObject *ui;
    gboolean use_color;
    gchar *verbose_level_string;
};

GST_BOILERPLATE(GstCutterConsoleOutput, gst_cutter_console_output, GstBaseSink, GST_TYPE_BASE_SINK);

enum
{
    ARG_0,
    ARG_USE_COLOR,
    ARG_VERBOSE_LEVEL
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
gst_cutter_console_output_base_init (gpointer klass)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

    gst_element_class_add_pad_template(element_class,
        gst_static_pad_template_get(&cutter_console_output_sink_template_factory));

    gst_element_class_set_details(element_class, &cutter_console_output_details);
}

static void
gst_cutter_console_output_class_init (GstCutterConsoleOutputClass * klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstBaseSinkClass *base_sink_class = GST_BASE_SINK_CLASS(klass);
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    base_sink_class->start      = start;
    base_sink_class->stop       = stop;
    base_sink_class->render     = render;

    spec = g_param_spec_boolean("use-color",
                                "Use color",
                                "Whether use color",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, ARG_USE_COLOR, spec);

    spec = g_param_spec_string("verbose-level",
                               "Verbose Level",
                               "The string of representing verbosity level",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, ARG_VERBOSE_LEVEL, spec);

    g_type_class_add_private(gobject_class, sizeof(GstCutterConsoleOutputPrivate));
}

static void
gst_cutter_console_output_init (GstCutterConsoleOutput *cutter_console_output, GstCutterConsoleOutputClass * klass)
{
    GstCutterConsoleOutputPrivate *priv = GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(cutter_console_output);

    priv->run_context = NULL;
    priv->parser = NULL;
    priv->ui = NULL;
    priv->use_color = FALSE;
    priv->verbose_level_string = NULL;
}

static void
dispose (GObject *object)
{
    GstCutterConsoleOutputPrivate *priv = GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(object);

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

    if (priv->verbose_level_string) {
        g_free(priv->verbose_level_string);
        priv->verbose_level_string = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GstCutterConsoleOutputPrivate *priv = GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(object);

    switch (prop_id) {
      case ARG_USE_COLOR:
        priv->use_color = g_value_get_boolean(value);
        break;
      case ARG_VERBOSE_LEVEL:
        priv->verbose_level_string = g_value_dup_string(value);
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
    GstCutterConsoleOutputPrivate *priv = GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(object);

    switch (prop_id) {
      case ARG_USE_COLOR:
        g_value_set_boolean(value, priv->use_color);
        break;
      case ARG_VERBOSE_LEVEL:
        g_value_set_string(value, priv->verbose_level_string);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gboolean
start (GstBaseSink *base_sink)
{
    GstCutterConsoleOutputPrivate *priv = GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(base_sink);

    priv->run_context = g_object_new(CUT_TYPE_GST_RUN_CONTEXT, NULL);
    priv->ui = cut_ui_new("console", 
                          "use-color", priv->use_color,
                          "verbose-level", cut_verbose_level_parse(priv->verbose_level_string, NULL),
                          NULL);
    cut_listener_attach_to_run_context(CUT_LISTENER(priv->ui), priv->run_context);
    priv->parser = cut_stream_parser_new(priv->run_context);

    return TRUE;
}

static gboolean
stop (GstBaseSink *base_sink)
{
    GstCutterConsoleOutputPrivate *priv = GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(base_sink);

    cut_listener_detach_from_run_context(CUT_LISTENER(priv->ui), priv->run_context);

    return TRUE;
}

static GstFlowReturn
render (GstBaseSink *base_sink, GstBuffer *buffer)
{
    guint size;
    guint8 *data;
    GstCutterConsoleOutputPrivate *priv = GST_CUTTER_CONSOLE_OUTPUT_GET_PRIVATE(base_sink);

    size = GST_BUFFER_SIZE(buffer);
    data = GST_BUFFER_DATA(buffer);

    if (size > 0)
        cut_stream_parser_parse(priv->parser, (gchar *)data, size, NULL);

    return GST_FLOW_OK;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
