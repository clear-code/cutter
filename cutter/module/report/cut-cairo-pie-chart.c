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
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

#include <cutter/cut-test-result.h>
#include <cutter/cut-runner.h>

#include "cut-cairo-pie-chart.h"
#include "cut-cairo.h"

#define CUT_CAIRO_PIE_CHART_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CAIRO_PIE_CHART, CutCairoPieChartPrivate))

typedef struct _CutCairoPieChartPrivate	CutCairoPieChartPrivate;
struct _CutCairoPieChartPrivate
{
    gdouble width;
    gdouble height;
    guint n_legends;
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
cut_cairo_pie_chart_init (CutCairoPieChart *chart)
{
    CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart)->n_legends = 0;
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
cut_cairo_pie_chart_new (gdouble width, gdouble height)
{
    return g_object_new(CUT_TYPE_CAIRO_PIE_CHART,
                        "width", width,
                        "height", height,
                        NULL);
}

#define SQUARE_LENGTH(x, y) ((x) * (x) + (y) * (y))

static void
show_text_at_center (cairo_t *cr, const gchar *utf8,
                     gdouble center_x, gdouble center_y,
                     gint font_size)
{
    PangoLayout *layout;
    int width, height;

    if (!utf8)
        return;

    layout = cut_cairo_create_pango_layout(cr, utf8, font_size);
    if (!layout)
        return;

    pango_layout_get_pixel_size(layout, &width, &height);

    cairo_move_to(cr,
                  center_x - (width / 2.0),
                  center_y - (height / 2.0));
    pango_cairo_show_layout(cr, layout);
    g_object_unref(layout);
}

static void
get_pie_center_position (CutCairoPieChart *chart,
                         gdouble *x, gdouble *y)
{
    CutCairoPieChartPrivate *priv;
    
    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    *x = priv->width / 2.0;
    *y = priv->height / 2.0;
}

static gdouble
get_pie_radius (CutCairoPieChart *chart)
{
    return CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart)->height / 2.0;
}

static gdouble
show_pie_piece (CutCairoPieChart *chart,
                cairo_t *cr, gdouble start, gdouble percent,
                CutTestResultStatus status)
{
    gdouble end;
    gdouble center_x, center_y;
    gdouble text_x, text_y;
    gdouble radian, radius;
    gchar *string;
    gint font_size = 6;

    if (percent == 0.0)
        return start;

    get_pie_center_position(chart, &center_x, &center_y);
    radius = get_pie_radius(chart);

    cairo_move_to(cr, center_x, center_y);
    end = start + 2 * M_PI * percent;
    cairo_arc(cr, center_x, center_y, radius, start, end);
    cut_cairo_set_source_result_color(cr, status);
    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.8);
    cairo_stroke(cr);

    radian = start + ((end - start) / 2.0);
    text_x = center_x + cos(radian) * (radius + font_size);
    text_y = center_y + sin(radian) * (radius + font_size);
    string = g_strdup_printf("%.1f%%", percent * 100);
    show_text_at_center(cr, string, text_x, text_y, font_size);
    g_free(string);

    return end;
}

#define LEGEND_X 200
#define LEGEND_Y 50

static void
draw_legend_mark (cairo_t *cr, gdouble x, gdouble y,
                  CutTestResultStatus status)
{
    cairo_rectangle(cr, x, y, 10, 10);
    cut_cairo_set_source_result_color(cr, status);
    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.8);
    cairo_stroke(cr);
}

static void
show_legend (CutCairoPieChart *chart, cairo_t *cr, CutTestResultStatus status)
{
    CutCairoPieChartPrivate *priv;
    PangoLayout *layout;
    const gchar *text;
    gdouble x, y;
    gdouble center_x, center_y;
    gdouble radius;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    radius = get_pie_radius(chart);
    get_pie_center_position(chart, &center_x, &center_y);

    x = center_x + radius + 10;
    y = center_y - radius + priv->n_legends * 10;
    draw_legend_mark(cr, x, y, status);

    text = cut_test_result_status_to_signal_name(status);
    layout = cut_cairo_create_pango_layout(cr, text, 6);
    if (!layout)
        return;

    cairo_move_to(cr, x + 12, y);
    pango_cairo_show_layout(cr, layout);
    g_object_unref(layout);

    priv->n_legends++;
}

static gdouble
show_status_pie_piece (CutCairoPieChart *chart,
                       cairo_t *cr, CutRunner *runner,
                       gdouble start, CutTestResultStatus status)
{
    guint n_tests = 0, n_results = 0;
    gdouble end;

    n_tests = cut_runner_get_n_tests(runner);

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        n_results = cut_runner_get_n_successes(runner);
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        n_results = cut_runner_get_n_notifications(runner);
        break;
      case CUT_TEST_RESULT_OMISSION:
        n_results = cut_runner_get_n_omissions(runner);
        break;
      case CUT_TEST_RESULT_PENDING:
        n_results = cut_runner_get_n_pendings(runner);
        break;
      case CUT_TEST_RESULT_FAILURE:
        n_results = cut_runner_get_n_failures(runner);
        break;
      case CUT_TEST_RESULT_ERROR:
        n_results = cut_runner_get_n_errors(runner);
        break;
      default:
        break;
    }

    if (n_results == 0)
        return start;

    end = show_pie_piece(chart, cr, start,
                         ((gdouble)n_results / (gdouble)n_tests),
                         status);
    show_legend(chart, cr, status);

    return end;
}

void
cut_cairo_pie_chart_draw (CutCairoPieChart *chart,
                          cairo_t *cr, CutRunner *runner)
{
    double start;
    double current_x, current_y;

    cairo_get_current_point(cr, &current_x, &current_y);

    cairo_save(cr);
    cairo_set_line_width(cr, 0.75);
    cairo_translate(cr, current_x, current_y);

    start = 2 * M_PI * 0.75;
    start = show_status_pie_piece(chart, cr, runner, start,
                                  CUT_TEST_RESULT_SUCCESS);
    start = show_status_pie_piece(chart, cr, runner, start,
                                  CUT_TEST_RESULT_FAILURE);
    start = show_status_pie_piece(chart, cr, runner, start,
                                  CUT_TEST_RESULT_ERROR);
    start = show_status_pie_piece(chart, cr, runner, start,
                                  CUT_TEST_RESULT_PENDING);
    start = show_status_pie_piece(chart, cr, runner, start,
                                  CUT_TEST_RESULT_OMISSION);
    cairo_restore(cr);
}

void
cut_cairo_pie_chart_get_size (CutCairoPieChart *chart,
                              gdouble *width, gdouble *height)
{
    CutCairoPieChartPrivate *priv;
    
    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    *width = priv->width;
    *height = priv->height;
}

gdouble
cut_cairo_pie_chart_get_width (CutCairoPieChart *chart)
{
    return CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart)->width;
}

gdouble
cut_cairo_pie_chart_get_height (CutCairoPieChart *chart)
{
    return CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart)->height;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
