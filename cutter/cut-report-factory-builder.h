/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_REPORT_FACTORY_BUILDER_H__
#define __CUT_REPORT_FACTORY_BUILDER_H__

#include <glib-object.h>

#include <cutter/cut-factory-builder.h>

G_BEGIN_DECLS

#define CUT_TYPE_REPORT_FACTORY_BUILDER            (cut_report_factory_builder_get_type ())
#define CUT_REPORT_FACTORY_BUILDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_REPORT_FACTORY_BUILDER, CutReportFactoryBuilder))
#define CUT_REPORT_FACTORY_BUILDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_REPORT_FACTORY_BUILDER, CutReportFactoryBuilderClass))
#define CUT_IS_REPORT_FACTORY_BUILDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_REPORT_FACTORY_BUILDER))
#define CUT_IS_REPORT_FACTORY_BUILDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_REPORT_FACTORY_BUILDER))
#define CUT_REPORT_FACTORY_BUILDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_REPORT_FACTORY_BUILDER, CutReportFactoryBuilderClass))

typedef struct _CutReportFactoryBuilder         CutReportFactoryBuilder;
typedef struct _CutReportFactoryBuilderClass    CutReportFactoryBuilderClass;

struct _CutReportFactoryBuilder
{
    CutFactoryBuilder object;
};

struct _CutReportFactoryBuilderClass
{
    CutFactoryBuilderClass parent_class;
};

GType cut_report_factory_builder_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __CUT_REPORT_FACTORY_BUILDER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
