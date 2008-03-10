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

#ifndef __CUT_FACTORY_BUILDER_H__
#define __CUT_FACTORY_BUILDER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_FACTORY_BUILDER            (cut_factory_builder_get_type ())
#define CUT_FACTORY_BUILDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_FACTORY_BUILDER, CutFactoryBuilder))
#define CUT_FACTORY_BUILDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_FACTORY_BUILDER, CutFactoryBuilderClass))
#define CUT_IS_FACTORY_BUILDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_FACTORY_BUILDER))
#define CUT_IS_FACTORY_BUILDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_FACTORY_BUILDER))
#define CUT_FACTORY_BUILDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_FACTORY_BUILDER, CutFactoryBuilderClass))

typedef struct _CutFactoryBuilder         CutFactoryBuilder;
typedef struct _CutFactoryBuilderClass    CutFactoryBuilderClass;

struct _CutFactoryBuilder
{
    GObject object;
};

struct _CutFactoryBuilderClass
{
    GObjectClass parent_class;
    void         (*set_option_context) (CutFactoryBuilder *builder,
                                        GOptionContext *context);
    GList       *(*build)              (CutFactoryBuilder *builder);
    const gchar *(*get_type_name)      (CutFactoryBuilder *builder);
};

void         cut_factory_builder_register_builder (void);
gboolean     cut_factory_builder_has_builder      (const gchar *biulder_name);
GObject     *cut_factory_builder_create           (const gchar *builder_name);
const gchar **cut_factory_builder_get_builders    (void);

GType        cut_factory_builder_get_type       (void) G_GNUC_CONST;
const gchar *cut_factory_builder_get_type_name  (CutFactoryBuilder *builder);
const gchar *cut_factory_builder_get_module_dir (CutFactoryBuilder *builder);
void         cut_factory_builder_set_option_context
                                                (CutFactoryBuilder *builder,
                                                 GOptionContext *context);
GList       *cut_factory_builder_build          (CutFactoryBuilder *builder);

G_END_DECLS

#endif /* __CUT_FACTORY_BUILDER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
