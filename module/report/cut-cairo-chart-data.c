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

#include <math.h>
#include <glib.h>

#include "cut-cairo-chart-data.h"

#define CUT_CAIRO_CHART_DATA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CAIRO_CHART_DATA, CutCairoChartDataPrivate))

typedef struct _CutCairoChartDataPrivate	CutCairoChartDataPrivate;
struct _CutCairoChartDataPrivate
{
    gchar *description;
    gdouble red;
    gdouble green;
    gdouble blue;
};

enum
{
    PROP_0,
    PROP_DESCRIPTION,
    PROP_RED,
    PROP_GREEN,
    PROP_BLUE
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

G_DEFINE_TYPE (CutCairoChartData, cut_cairo_chart_data, G_TYPE_OBJECT)

static void
cut_cairo_chart_data_class_init (CutCairoChartDataClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("description",
                               "The description",
                               "The description",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_DESCRIPTION, spec);

    spec = g_param_spec_double("red",
                               "Red",
                               "Red value",
                               0.0,
                               G_MAXDOUBLE,
                               0.0,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RED, spec);

    spec = g_param_spec_double("green",
                               "Green",
                               "Green value",
                               0.0,
                               G_MAXDOUBLE,
                               0.0,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_GREEN, spec);

    spec = g_param_spec_double("blue",
                               "Blue",
                               "Blue value",
                               0.0,
                               G_MAXDOUBLE,
                               0.0,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BLUE, spec);

    g_type_class_add_private(gobject_class, sizeof(CutCairoChartDataPrivate));
}

static void
cut_cairo_chart_data_init (CutCairoChartData *chart)
{
    CutCairoChartDataPrivate *priv;

    priv = CUT_CAIRO_CHART_DATA_GET_PRIVATE(chart);

    priv->description = NULL;
}

static void
dispose (GObject *object)
{
    CutCairoChartDataPrivate *priv;

    priv = CUT_CAIRO_CHART_DATA_GET_PRIVATE(object);

    if (priv->description) {
        g_free(priv->description);
        priv->description = NULL;
    }

    G_OBJECT_CLASS(cut_cairo_chart_data_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutCairoChartDataPrivate *priv = CUT_CAIRO_CHART_DATA_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_DESCRIPTION:
        if (priv->description)
            g_free(priv->description);
        priv->description = g_value_dup_string(value);
        break;
      case PROP_RED:
        priv->red = g_value_get_double(value);
        break;
      case PROP_GREEN:
        priv->green = g_value_get_double(value);
        break;
      case PROP_BLUE:
        priv->blue = g_value_get_double(value);
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
    CutCairoChartDataPrivate *priv = CUT_CAIRO_CHART_DATA_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_DESCRIPTION:
        g_value_set_string(value, priv->description);
        break;
      case PROP_RED:
        g_value_set_double(value, priv->red);
        break;
      case PROP_GREEN:
        g_value_set_double(value, priv->green);
        break;
      case PROP_BLUE:
        g_value_set_double(value, priv->blue);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutCairoChartData *
cut_cairo_chart_data_new (void)
{
    return g_object_new(CUT_TYPE_CAIRO_CHART_DATA,
                        NULL);
}

gboolean
cut_cairo_chart_data_get_rgb (CutCairoChartData *chart,
                              gdouble *red, gdouble *green, gdouble *blue)
{
    CutCairoChartDataPrivate *priv;
    
    priv = CUT_CAIRO_CHART_DATA_GET_PRIVATE(chart);

    *red = priv->red;
    *green = priv->green;
    *blue = priv->blue;

    return TRUE;
}

const gchar *
cut_cairo_chart_data_get_description (CutCairoChartData *chart)
{
    return CUT_CAIRO_CHART_DATA_GET_PRIVATE(chart)->description;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
