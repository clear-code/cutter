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

#ifndef __CUT_CAIRO_PIE_CHART_H__
#define __CUT_CAIRO_PIE_CHART_H__

#include <glib-object.h>
#include <cairo.h>

G_BEGIN_DECLS

#define CUT_TYPE_CAIRO_PIE_CHART            (cut_cairo_pie_chart_get_type ())
#define CUT_CAIRO_PIE_CHART(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_CAIRO_PIE_CHART, CutCairoPieChart))
#define CUT_CAIRO_PIE_CHART_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_CAIRO_PIE_CHART, CutCairoPieChartClass))
#define CUT_IS_CAIRO_PIE_CHART(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_CAIRO_PIE_CHART))
#define CUT_IS_CAIRO_PIE_CHART_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_CAIRO_PIE_CHART))
#define CUT_CAIRO_PIE_CHART_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_CAIRO_PIE_CHART, CutCairoPieChartClass))

typedef struct _CutCairoPieChart         CutCairoPieChart;
typedef struct _CutCairoPieChartClass    CutCairoPieChartClass;

struct _CutCairoPieChart
{
    GObject object;
};

struct _CutCairoPieChartClass
{
    GObjectClass parent_class;
};

GType             cut_cairo_pie_chart_get_type  (void) G_GNUC_CONST;

CutCairoPieChart *cut_cairo_pie_chart_new       (void);
void              cut_cairo_pie_chart_draw      (CutCairoPieChart *chart,
                                                 cairo_t *cr);

G_END_DECLS

#endif /* __CUT_CAIRO_PIE_CHART_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
