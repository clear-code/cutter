/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifndef __CUT_REPORT_FACTORY_H__
#define __CUT_REPORT_FACTORY_H__

#include <glib-object.h>

#include <cutter/cut-module-factory.h>

G_BEGIN_DECLS

#define CUT_TYPE_REPORT_FACTORY            (cut_report_factory_get_type ())
#define CUT_REPORT_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_REPORT_FACTORY, CutReportFactory))
#define CUT_REPORT_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_REPORT_FACTORY, CutReportFactoryClass))
#define CUT_IS_REPORT_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_REPORT_FACTORY))
#define CUT_IS_REPORT_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_REPORT_FACTORY))
#define CUT_REPORT_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_REPORT_FACTORY, CutReportFactoryClass))

typedef struct _CutReportFactory         CutReportFactory;
typedef struct _CutReportFactoryClass    CutReportFactoryClass;

struct _CutReportFactory
{
    CutModuleFactory object;
};

struct _CutReportFactoryClass
{
    CutModuleFactoryClass parent_class;
};

GType             cut_report_factory_get_type (void) G_GNUC_CONST;

CutModuleFactory *cut_report_factory_new      (const gchar *name,
                                               const gchar *first_property,
                                               ...);

G_END_DECLS

#endif /* __CUT_REPORT_FACTORY_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
