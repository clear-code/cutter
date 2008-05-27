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

#ifndef __CUT_CAIRO_CHART_DATA_H__
#define __CUT_CAIRO_CHART_DATA_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_CAIRO_CHART_DATA            (cut_cairo_chart_data_get_type ())
#define CUT_CAIRO_CHART_DATA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_CAIRO_CHART_DATA, CutCairoChartData))
#define CUT_CAIRO_CHART_DATA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_CAIRO_CHART_DATA, CutCairoChartDataClass))
#define CUT_IS_CAIRO_CHART_DATA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_CAIRO_CHART_DATA))
#define CUT_IS_CAIRO_CHART_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_CAIRO_CHART_DATA))
#define CUT_CAIRO_CHART_DATA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_CAIRO_CHART_DATA, CutCairoChartDataClass))

typedef struct _CutCairoChartData         CutCairoChartData;
typedef struct _CutCairoChartDataClass    CutCairoChartDataClass;

struct _CutCairoChartData
{
    GObject object;
};

struct _CutCairoChartDataClass
{
    GObjectClass parent_class;
};

GType             cut_cairo_chart_data_get_type  (void) G_GNUC_CONST;

CutCairoChartData *cut_cairo_chart_data_new       (void);
const gchar       *cut_cairo_chart_data_get_description(CutCairoChartData *data);
gboolean           cut_cairo_chart_data_get_rgb        (CutCairoChartData *data,
                                                        gdouble *red,
                                                        gdouble *green,
                                                        gdouble *blue);

G_END_DECLS

#endif /* __CUT_CAIRO_CHART_DATA_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
