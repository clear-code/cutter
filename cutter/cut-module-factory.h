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

#ifndef __CUT_MODULE_FACTORY_H__
#define __CUT_MODULE_FACTORY_H__

#include <glib-object.h>

#include <cutter/cut-module.h>

G_BEGIN_DECLS

#define CUT_TYPE_MODULE_FACTORY            (cut_module_factory_get_type ())
#define CUT_MODULE_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_MODULE_FACTORY, CutModuleFactory))
#define CUT_MODULE_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_MODULE_FACTORY, CutModuleFactoryClass))
#define CUT_IS_MODULE_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_MODULE_FACTORY))
#define CUT_IS_MODULE_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_MODULE_FACTORY))
#define CUT_MODULE_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_MODULE_FACTORY, CutModuleFactoryClass))

typedef struct _CutModuleFactory         CutModuleFactory;
typedef struct _CutModuleFactoryClass    CutModuleFactoryClass;

struct _CutModuleFactory
{
    GObject object;
};

struct _CutModuleFactoryClass
{
    GObjectClass parent_class;

    void         (*set_option_group)    (CutModuleFactory *factory,
                                         GOptionContext *context);
    GObject     *(*create)              (CutModuleFactory *factory);
};

GType        cut_module_factory_get_type    (void) G_GNUC_CONST;

void         cut_module_factory_init        (void);
void         cut_module_factory_quit        (void);

const gchar *cut_module_factory_get_default_module_dir   (void);
void         cut_module_factory_set_default_module_dir   (const gchar *dir);

void         cut_module_factory_load        (const gchar *base_dir);
void         cut_module_factory_unload      (void);
const GList *cut_module_factory_get_names   (const gchar *type);

CutModuleFactory *cut_module_factory_new    (const gchar *type,
                                             const gchar *name,
                                             const gchar *first_property,
                                             ...);

void         cut_module_factory_set_option_group (CutModuleFactory *factory,
                                                  GOptionContext *context);
GObject     *cut_module_factory_create           (CutModuleFactory *factory);
const gchar *cut_module_factory_get_name         (CutModuleFactory *factory);

G_END_DECLS

#endif /* __CUT_MODULE_FACTORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
