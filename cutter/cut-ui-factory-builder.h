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

#ifndef __CUT_UI_FACTORY_BUILDER_H__
#define __CUT_UI_FACTORY_BUILDER_H__

#include <glib-object.h>

#include <cutter/cut-factory-builder.h>

G_BEGIN_DECLS

#define CUT_TYPE_UI_FACTORY_BUILDER            (cut_ui_factory_builder_get_type ())
#define CUT_UI_FACTORY_BUILDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UI_FACTORY_BUILDER, CutUIFactoryBuilder))
#define CUT_UI_FACTORY_BUILDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_UI_FACTORY_BUILDER, CutUIFactoryBuilderClass))
#define CUT_IS_UI_FACTORY_BUILDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UI_FACTORY_BUILDER))
#define CUT_IS_UI_FACTORY_BUILDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_UI_FACTORY_BUILDER))
#define CUT_UI_FACTORY_BUILDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_UI_FACTORY_BUILDER, CutUIFactoryBuilderClass))

typedef struct _CutUIFactoryBuilder         CutUIFactoryBuilder;
typedef struct _CutUIFactoryBuilderClass    CutUIFactoryBuilderClass;

struct _CutUIFactoryBuilder
{
    CutFactoryBuilder object;
};

struct _CutUIFactoryBuilderClass
{
    CutFactoryBuilderClass parent_class;
};

GType cut_ui_factory_builder_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __CUT_UI_FACTORY_BUILDER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
