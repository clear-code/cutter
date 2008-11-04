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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-module.h"
#include "cut-module-factory.h"
#include "cut-module-factory-utils.h"
#include "cut-factory-builder.h"

#define cut_module_factory_init init
G_DEFINE_ABSTRACT_TYPE(CutModuleFactory, cut_module_factory, G_TYPE_OBJECT)
#undef cut_module_factory_init

static void
cut_module_factory_class_init (CutModuleFactoryClass *klass)
{
    klass->set_option_group = NULL;
}

static void
init (CutModuleFactory *factory)
{
}

CutModuleFactory *
cut_module_factory_new_valist (const gchar *type, const gchar *name,
                               const gchar *first_property, va_list var_args)
{
    CutModule *module;
    GObject *factory;

    module = cut_module_factory_load_module(type, name);
    g_return_val_if_fail(module != NULL, NULL);

    factory = cut_module_instantiate(module, first_property, var_args);

    return CUT_MODULE_FACTORY(factory);
}

CutModuleFactory *
cut_module_factory_new (const gchar *type, const gchar *name,
                        const gchar *first_property, ...)
{
    CutModule *module;
    GObject *factory;
    va_list var_args;

    module = cut_module_factory_load_module(type, name);
    if (!module)
        return NULL;

    va_start(var_args, first_property);
    factory = cut_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return CUT_MODULE_FACTORY(factory);
}

void
cut_module_factory_set_option_group (CutModuleFactory *factory,
                                     GOptionContext   *context)
{
    CutModuleFactoryClass *klass;

    g_return_if_fail(CUT_IS_MODULE_FACTORY(factory));

    klass = CUT_MODULE_FACTORY_GET_CLASS(factory);
    g_return_if_fail(klass->set_option_group);

    klass->set_option_group(factory, context);
}

GObject *
cut_module_factory_create (CutModuleFactory *factory)
{
    CutModuleFactoryClass *klass;

    g_return_val_if_fail(CUT_IS_MODULE_FACTORY(factory), NULL);

    klass = CUT_MODULE_FACTORY_GET_CLASS(factory);
    g_return_val_if_fail(klass->create, NULL);

    return klass->create(factory);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
