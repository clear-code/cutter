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

#include "cut-cairo-pie-chart.h"

#define CUT_CAIRO_PIE_CHART_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CAIRO_PIE_CHART, CutCairoPieChartPrivate))

typedef struct _CutCairoPieChartPrivate	CutCairoPieChartPrivate;
struct _CutCairoPieChartPrivate
{
    gdouble width;
    gdouble height;
};

enum
{
    PROP_0,
    PROP_WIDTH,
    PROP_HEIGHT
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

G_DEFINE_TYPE (CutCairoPieChart, cut_cairo_pie_chart, G_TYPE_OBJECT)

static void
cut_cairo_pie_chart_class_init (CutCairoPieChartClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_double("width",
                               "Width",
                               "Width",
                               0.0,
                               G_MAXDOUBLE,
                               0.0,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_WIDTH, spec);

    spec = g_param_spec_double("height",
                               "Height",
                               "Height",
                               0.0,
                               G_MAXDOUBLE,
                               0.0,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_HEIGHT, spec);

    g_type_class_add_private(gobject_class, sizeof(CutCairoPieChartPrivate));
}

static void
cut_cairo_pie_chart_init (CutCairoPieChart *cairo_pie_chart)
{
}

static void
dispose (GObject *object)
{
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutCairoPieChartPrivate *priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_WIDTH:
        priv->width = g_value_get_double(value);
        break;
      case PROP_HEIGHT:
        priv->height = g_value_get_double(value);
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
    CutCairoPieChartPrivate *priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_WIDTH:
        g_value_set_double(value, priv->width);
        break;
      case PROP_HEIGHT:
        g_value_set_double(value, priv->height);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutCairoPieChart *
cut_cairo_pie_chart_new (void)
{
    return g_object_new(CUT_TYPE_CAIRO_PIE_CHART, NULL);
}

void
cut_cairo_pie_chart_draw (CutCairoPieChart *chart,
                          cairo_t *cr)
{
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
