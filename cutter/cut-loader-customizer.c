/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014  Kouhei Sutou  <kou@clear-code.com>
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

#include <glib.h>

#include "cut-loader-customizer.h"
#include "cut-module.h"

#define CUT_LOADER_CUSTOMIZER_GET_PRIVATE(obj)                  \
  (G_TYPE_INSTANCE_GET_PRIVATE((obj),                           \
                               CUT_TYPE_LOADER_CUSTOMIZER,      \
                               CutLoaderCustomizerPrivate))

G_DEFINE_ABSTRACT_TYPE(CutLoaderCustomizer, cut_loader_customizer, G_TYPE_OBJECT)
CUT_MODULE_DEFINE_INTERNAL_DEFINITIONS(loader_customizer, LOADER_CUSTOMIZER)

static void
cut_loader_customizer_class_init (CutLoaderCustomizerClass *klass)
{
}

static void
cut_loader_customizer_init (CutLoaderCustomizer *report)
{
}

CutLoaderCustomizer *
cut_loader_customizer_new (const gchar *name,
                           const gchar *first_property, ...)
{
    CutModule *module;
    GObject *customizer;
    va_list var_args;

    module = cut_loader_customizer_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    customizer = cut_module_instantiate(module,
                                        first_property, var_args);
    va_end(var_args);

    return CUT_LOADER_CUSTOMIZER(customizer);
}

void
cut_loader_customizer_customize (CutLoaderCustomizer *customizer,
                                 CutLoader           *loader)
{
    CutLoaderCustomizerClass *klass;

    g_return_if_fail(CUT_IS_LOADER_CUSTOMIZER(customizer));

    klass = CUT_LOADER_CUSTOMIZER_GET_CLASS(customizer);
    if (klass->customize) {
        klass->customize(customizer, loader);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
