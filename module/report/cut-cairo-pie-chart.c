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

#include <math.h>
#include <glib.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

#include <cutter/cut-test-result.h>
#include <cutter/cut-run-context.h>

#include "cut-cairo-pie-chart.h"
#include "cut-cairo.h"

#define CUT_CAIRO_PIE_CHART_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CAIRO_PIE_CHART, CutCairoPieChartPrivate))

typedef struct _CutCairoPieChartPrivate	CutCairoPieChartPrivate;
struct _CutCairoPieChartPrivate
{
    gdouble width;
    gdouble height;
    guint n_legends;
    gboolean show_data_label;
    GList *series;
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
    CutCairoPieChartPrivate *priv;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    priv->n_legends = 0;
    priv->series = NULL;
    priv->show_data_label = TRUE;
}

static void
dispose (GObject *object)
{
    CutCairoPieChartPrivate *priv;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(object);

    if (priv->series) {
        g_list_free(priv->series);
        priv->series = NULL;
    }

    G_OBJECT_CLASS(cut_cairo_pie_chart_parent_class)->dispose(object);
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
                     gdouble radian, gint font_size)
{
    PangoLayout *layout;
    int width, height;

    if (!utf8)
        return;

    layout = cut_cairo_create_pango_layout(cr, utf8, font_size);
    if (!layout)
        return;

    pango_layout_get_pixel_size(layout, &width, &height);

    if (cos(radian) < 0.0)
        center_x -= width;
    center_y -= height / 2;
    cairo_move_to(cr, center_x, center_y);
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
draw_pie_piece (CutCairoPieChart *chart,
                cairo_t *cr, gdouble start, gdouble percent)
{
    gdouble end;
    gdouble center_x, center_y;
    gdouble radius;

    if (percent == 0.0)
        return start;

    get_pie_center_position(chart, &center_x, &center_y);
    radius = get_pie_radius(chart);

    cairo_move_to(cr, center_x, center_y);
    end = start + 2 * M_PI * percent;
    cairo_arc(cr, center_x, center_y, radius, start, end);
    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.8);
    cairo_stroke(cr);

    return end;
}

static gdouble
get_status_result_number (CutRunContext *run_context,
                          CutTestResultStatus status)
{
    guint n_results = 0;

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        n_results = cut_run_context_get_n_successes(run_context);
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        n_results = cut_run_context_get_n_notifications(run_context);
        break;
      case CUT_TEST_RESULT_OMISSION:
        n_results = cut_run_context_get_n_omissions(run_context);
        break;
      case CUT_TEST_RESULT_PENDING:
        n_results = cut_run_context_get_n_pendings(run_context);
        break;
      case CUT_TEST_RESULT_FAILURE:
        n_results = cut_run_context_get_n_failures(run_context);
        break;
      case CUT_TEST_RESULT_ERROR:
        n_results = cut_run_context_get_n_errors(run_context);
        break;
      default:
        break;
    }

    return (gdouble)n_results;
}

static gdouble
calculate_sum (CutCairoPieChart *chart, CutRunContext *run_context)
{
    CutCairoPieChartPrivate *priv;
    GList *node;
    gdouble sum = 0.0;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    for (node = priv->series; node; node = g_list_next(node)) {
        CutTestResultStatus status;

        status = GPOINTER_TO_INT(node->data);
        sum += get_status_result_number(run_context, status);
    }

    return sum;
}

static void
draw_data_label (CutCairoPieChart *chart, cairo_t *cr, CutRunContext *run_context)
{
    CutCairoPieChartPrivate *priv;
    GList *node;
    gdouble sum, start;
    gdouble center_x, center_y, radius;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    sum = calculate_sum(chart, run_context);
    start = 2 * M_PI * 0.75;
    get_pie_center_position(chart, &center_x, &center_y);
    radius = get_pie_radius(chart);

    for (node = priv->series; node; node = g_list_next(node)) {
        CutTestResultStatus status;
        gint n_results;
        gdouble x, y;
        gdouble ratio, radian;
        const gchar *label;

        status = GPOINTER_TO_INT(node->data);
        n_results = get_status_result_number(run_context, status);
        if (n_results == 0)
            continue;

        ratio = n_results / sum;
        radian = start + M_PI * ratio;
        start += 2 * M_PI * ratio;
        if (cos(radian) > 0.0)
            x = center_x + radius + 20;
        else
            x = center_x - radius - 20;

        y = center_y + sin(radian) * radius;
        label = cut_test_result_status_to_signal_name(status);
        show_text_at_center(cr, label, x, y, radian, 6);

        cairo_set_line_width(cr, 0.50);
        y = center_y + sin(radian) * radius;
        cairo_move_to(cr, x, y);
        x = center_x + cos(radian) * radius;
        cairo_line_to(cr, x, y);
        cairo_stroke(cr);
    }
}

static void
draw_chart (CutCairoPieChart *chart, cairo_t *cr, CutRunContext *run_context)
{
    CutCairoPieChartPrivate *priv;
    GList *node;
    gdouble sum;
    gdouble start;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    cairo_set_line_width(cr, 0.75);

    sum = calculate_sum(chart, run_context);
    start = 2 * M_PI * 0.75;

    for (node = priv->series; node; node = g_list_next(node)) {
        CutTestResultStatus status;
        gint n_results;
        gdouble ratio;

        status = GPOINTER_TO_INT(node->data);
        cut_cairo_set_source_result_color(cr, status);
        n_results = get_status_result_number(run_context, status);
        if (n_results == 0)
            continue;

        ratio = (gdouble)n_results / (gdouble)sum;
        start = draw_pie_piece(chart, cr, start, ratio);
    }
}

static void
draw_legend (CutCairoPieChart *chart, cairo_t *cr)
{
    CutCairoPieChartPrivate *priv;
    GList *node;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    for (node = priv->series; node; node = g_list_next(node)) {
        CutTestResultStatus status;

        status = GPOINTER_TO_INT(node->data);

        show_legend(chart, cr, status);
    }
}

static void
create_series (CutCairoPieChart *chart)
{
    CutCairoPieChartPrivate *priv;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);

    priv->series = g_list_append(priv->series, GINT_TO_POINTER(CUT_TEST_RESULT_SUCCESS));
    priv->series = g_list_append(priv->series, GINT_TO_POINTER(CUT_TEST_RESULT_FAILURE));
    priv->series = g_list_append(priv->series, GINT_TO_POINTER(CUT_TEST_RESULT_ERROR));
    priv->series = g_list_append(priv->series, GINT_TO_POINTER(CUT_TEST_RESULT_PENDING));
    priv->series = g_list_append(priv->series, GINT_TO_POINTER(CUT_TEST_RESULT_OMISSION));
}

void
cut_cairo_pie_chart_draw (CutCairoPieChart *chart,
                          cairo_t *cr, CutRunContext *run_context)
{
    CutCairoPieChartPrivate *priv;
    double current_x, current_y;

    priv = CUT_CAIRO_PIE_CHART_GET_PRIVATE(chart);
    cairo_get_current_point(cr, &current_x, &current_y);

    cairo_save(cr);
    cairo_translate(cr, current_x, current_y);

    /* tentative */ 
    create_series(chart);
    draw_chart(chart, cr, run_context);
    draw_legend(chart, cr);
    if (priv->show_data_label)
        draw_data_label(chart, cr, run_context);

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
