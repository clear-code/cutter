/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2009  Kouhei Sutou <kou@clear-code.com>
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

CutModuleFactory *cut_module_factory_new    (const gchar *type,
                                             const gchar *name,
                                             const gchar *first_property,
                                             ...);
CutModuleFactory *cut_module_factory_new_valist
                                            (const gchar *type,
                                             const gchar *name,
                                             const gchar *first_property,
                                             va_list      var_args);

void         cut_module_factory_set_option_group (CutModuleFactory *factory,
                                                  GOptionContext *context);
GObject     *cut_module_factory_create           (CutModuleFactory *factory);

G_END_DECLS

#endif /* __CUT_MODULE_FACTORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
