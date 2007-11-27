/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>

#include "cut-module.h"
#include "cut-ui-factory.h"
#include "cut-verbose-level.h"
#include "cut-enum-types.h"

static GList *factories = NULL;
static gchar *module_dir = NULL;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void real_set_option_group (CutUIFactory    *factory,
                                   GOptionContext      *context);

void cut_ui_factory_init (void)
{
}

void cut_ui_factory_quit (void)
{
    cut_ui_factory_unload();
    cut_ui_factory_set_default_module_dir(NULL);
}

const gchar *
cut_ui_factory_get_default_module_dir (void)
{
    return module_dir;
}

void
cut_ui_factory_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

static const gchar *
_cut_ui_factory_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CUT_UI_FACTORY_MODULE_DIR");
    if (dir)
        return dir;

    return UI_FACTORY_MODULEDIR;
}

void
cut_ui_factory_load (const gchar *base_dir)
{
    if (!base_dir)
        base_dir = _cut_ui_factory_module_dir();

    factories = g_list_concat(cut_module_load_modules(base_dir), factories);
}

static CutModule *
cut_ui_factory_load_module (const gchar *name)
{
    CutModule *module;

    module = cut_module_find(factories, name);
    if (module)
        return module;

    module = cut_module_load_module(_cut_ui_factory_module_dir(), name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            factories = g_list_prepend(factories, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
    }

    return module;
}

void
cut_ui_factory_unload (void)
{
    g_list_foreach(factories, (GFunc)cut_module_unload, NULL);
    g_list_free(factories);
    factories = NULL;
}

GList *
cut_ui_factory_get_registered_types (void)
{
    return cut_module_collect_registered_types(factories);
}

GList *
cut_ui_factory_get_log_domains (void)
{
    return cut_module_collect_log_domains(factories);
}

GList *
cut_ui_factory_get_names (void)
{
    return cut_module_collect_names(factories);
}

#define cut_ui_factory_init init
G_DEFINE_ABSTRACT_TYPE(CutUIFactory, cut_ui_factory, G_TYPE_OBJECT)
#undef cut_ui_factory_init

static void
cut_ui_factory_class_init (CutUIFactoryClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->set_option_group = real_set_option_group;
}

static void
init (CutUIFactory *factory)
{
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_ui_factory_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutUIFactory *
cut_ui_factory_new (const gchar *name, const gchar *first_property, ...)
{
    CutModule *module;
    GObject *factory;
    va_list var_args;

    module = cut_ui_factory_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    factory = cut_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return CUT_UI_FACTORY(factory);
}

static void
real_set_option_group (CutUIFactory *factory, GOptionContext *context)
{
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {NULL}
    };

    group = g_option_group_new(("runner-common"),
                               _("Common Runner Options"),
                               _("Show Common Runner Options"),
                               factory, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

void
cut_ui_factory_set_option_group (CutUIFactory *factory,
                                     GOptionContext   *context)
{
    CutUIFactoryClass *klass;

    g_return_if_fail(CUT_IS_UI_FACTORY(factory));

    klass = CUT_UI_FACTORY_GET_CLASS(factory);
    g_return_if_fail(klass->set_option_group);

    klass->set_option_group(factory, context);
}

CutRunner *
cut_ui_factory_create (CutUIFactory *factory)
{
    CutUIFactoryClass *klass;

    g_return_val_if_fail(CUT_IS_UI_FACTORY(factory), NULL);

    klass = CUT_UI_FACTORY_GET_CLASS(factory);
    g_return_val_if_fail(klass->create, NULL);

    return klass->create(factory);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
