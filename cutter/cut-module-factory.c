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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-module.h"
#include "cut-module-factory.h"
#include "cut-factory-builder.h"

static GHashTable *factories = NULL;
static GList *builders = NULL;
static gchar *module_dir = NULL;

static void
unload_modules (gpointer data)
{
    GList *modules = (GList *)data;
    g_list_foreach(modules, (GFunc)cut_module_unload, NULL);
    g_list_free(modules);
}

void
cut_module_factory_init (void)
{
    builders = cut_factory_builder_create_default();
    factories = g_hash_table_new_full(g_str_hash,
                                      g_str_equal,
                                      g_free,
                                      unload_modules);
}

void
cut_module_factory_quit (void)
{
    /* g_list_foreach(builders, (GFunc)g_object_unref, NULL);*/
    g_list_free(builders);

    cut_module_factory_unload();
    cut_module_factory_set_default_module_dir(NULL);
}

const gchar *
cut_module_factory_get_default_module_dir (void)
{
    return module_dir;
}

void
cut_module_factory_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

static const gchar *
_cut_module_factory_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CUT_FACTORY_MODULE_DIR");
    if (dir)
        return dir;

    return FACTORY_MODULEDIR;
}

static void
load_default_factory (void)
{
    GList *node;

    for (node = builders; node; node = g_list_next(node)) {
        const gchar *module_dir, *type_name;
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        GList *modules;
      
        module_dir = cut_factory_builder_get_module_dir(builder);
        if (!module_dir)
            continue;
        type_name = cut_factory_builder_get_type_name(builder);

        modules = cut_module_load_modules(module_dir);
        if (modules)
            g_hash_table_replace(factories, g_strdup(type_name), modules);
    }
}

void
cut_module_factory_load (const gchar *base_dir)
{
    GDir *gdir;
    const gchar *entry;

    load_default_factory();

    if (!base_dir)
        base_dir = _cut_module_factory_module_dir();

    gdir = g_dir_open(base_dir, 0, NULL);
    if (!gdir) return;

    while ((entry = g_dir_read_name(gdir))) {
        gchar *dir_name;

        if (cut_factory_builder_has_builder(entry))
            continue; 

        dir_name = g_build_filename(base_dir, entry, NULL);

        if (g_file_test(dir_name, G_FILE_TEST_IS_DIR)) {
            GList *modules = cut_module_load_modules(dir_name);
            if (modules)
                g_hash_table_replace(factories, g_strdup(entry), modules);
        }
        g_free(dir_name);
    }
    g_dir_close(gdir);
}

static CutModule *
cut_module_factory_load_module (const gchar *type, const gchar *name)
{
    CutModule *module;
    GList *modules = NULL;
    gchar *real_name;

    modules = g_hash_table_lookup(factories, type);
    if (!modules)
        return NULL;

    real_name = g_strconcat(name, "_factory", NULL);

    module = cut_module_find(modules, real_name);
    if (module)
        goto END;

    module = cut_module_load_module(_cut_module_factory_module_dir(), real_name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            modules = g_list_prepend(modules, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
            g_hash_table_replace(factories, g_strdup(type), modules);
        }
    }

END:
    g_free(real_name);

    return module;
}

void
cut_module_factory_unload (void)
{
    if (!factories)
        return;

    g_hash_table_unref(factories);
    factories = NULL;
}

GList *
cut_module_factory_get_names (const gchar *type)
{
    GList *modules, *orig_names, *node, *names = NULL;

    modules = g_hash_table_lookup(factories, type);
    if (!modules)
        return NULL;

    orig_names = cut_module_collect_names(modules);
    for (node = orig_names; node; node = g_list_next(node)) {
        const gchar *name = node->data;
        gchar *p;
        if (!g_str_has_suffix(name, "_factory"))
            continue;
        p = g_strrstr(name, "_factory");
        names = g_list_prepend(names, g_strndup(name, p - name));
    }
    g_list_free(orig_names);
    return names;
}

gboolean
cut_module_factory_exist_module (const gchar *type, const gchar *name)
{
    GList *names, *list;

    names = cut_module_factory_get_names(type);
    if (!names)
        return FALSE;
    list= g_list_find_custom(names, name, (GCompareFunc)strcmp);

    g_list_free(names);

    return (list != NULL);
}

void
cut_module_factory_set_option_context (GOptionContext *context)
{
    GList *node;

    for (node = builders; node; node = g_list_next(node)) {
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        cut_factory_builder_set_option_context(builder, context);
    }
}

GList *
cut_module_factory_build_factories (void)
{
    GList *factories = NULL, *node;

    for (node = builders; node; node = g_list_next(node)) {
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        factories = g_list_concat(cut_factory_builder_build(builder), factories);
    }
    return factories;
}

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
