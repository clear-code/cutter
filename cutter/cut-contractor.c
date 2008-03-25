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

#include "cut-contractor.h"
#include "cut-module-factory.h"
#include "cut-factory-builder.h"
#include "cut-report-factory-builder.h"
#include "cut-ui-factory-builder.h"

#define CUT_CONTRACTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CONTRACTOR, CutContractorPrivate))

typedef struct _CutContractorPrivate    CutContractorPrivate;
struct _CutContractorPrivate
{
    GList *builders;
};

G_DEFINE_TYPE(CutContractor, cut_contractor, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

static void
cut_contractor_class_init (CutContractorClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutContractorPrivate));
}

static GList *
create_default_builders (void)
{
    GList *list = NULL;

    list = g_list_prepend(list, g_object_new(CUT_TYPE_UI_FACTORY_BUILDER, NULL));
    list = g_list_prepend(list, g_object_new(CUT_TYPE_REPORT_FACTORY_BUILDER, NULL));

    return list;
}

static void
load_ui_factory (CutContractor *contractor)
{
    GList *node;
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);

    for (node = priv->builders; node; node = g_list_next(node)) {
        const gchar *module_dir, *type_name;
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);

        type_name = cut_factory_builder_get_type_name(builder);
        if (strcmp(type_name, "ui"))
            continue;

        module_dir = cut_factory_builder_get_module_dir(builder);

        cut_module_factory_load(module_dir, "ui");
    }
}

static void
cut_contractor_init (CutContractor *contractor)
{
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);

    cut_module_factory_init();
    priv->builders = create_default_builders();

    load_ui_factory(contractor);
}

static void
dispose (GObject *object)
{
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(object);

    if (priv->builders) {
        g_list_foreach(priv->builders, (GFunc)g_object_unref, NULL);
        g_list_free(priv->builders);
        priv->builders = NULL;
    }

    cut_module_factory_quit();

    G_OBJECT_CLASS(cut_contractor_parent_class)->dispose(object);
}

CutContractor *
cut_contractor_new (void)
{
    return CUT_CONTRACTOR(g_object_new(CUT_TYPE_CONTRACTOR, NULL));
}

gboolean
cut_contractor_has_builder (CutContractor *contractor, const gchar *type_name)
{
    GList *node;
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);

    for (node = priv->builders; node; node = g_list_next(node)) {
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        const gchar *name;
        name = cut_factory_builder_get_type_name(builder);
        if (!strcmp(type_name, name))
            return TRUE;
    }

    return FALSE;
}

#if 0
static gchar *
create_builder_type_name (const gchar *builder_name)
{
    return  g_strconcat("Cut", builder_name, "FactoryBuilder", NULL);
}

static GType
get_builder_type_from_name (const gchar *type_name)
{
    GType *children;
    GType type = 0;
    guint n_children, i;

    children = g_type_children(CUT_TYPE_FACTORY_BUILDER, &n_children);

    for (i = 0; i < n_children; i++) {
        GType child = children[i];
        const gchar *name = g_type_name(child);
        if (g_ascii_strcasecmp(name, type_name))
            type = child;
    }

    g_free(children);

    return type;
}

GObject *
cut_contractor_create_builder (CutContractor *contractor, const gchar *type_name)
{
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);
    gchar *gtype_name;
    GType type;
    GObject *builder;

    gtype_name = create_builder_type_name(type_name);
    type = get_builder_type_from_name(gtype_name);

    g_free(gtype_name);

    if (!type)
        return NULL;
    builder = g_object_new(type, NULL);
    if (builder)
        priv->builders = g_list_prepend(priv->builders, builder);

    return builder;
}
#endif

GList *
cut_contractor_build_factories (CutContractor *contractor)
{
    GList *factories = NULL, *node;
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);

    for (node = priv->builders; node; node = g_list_next(node)) {
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        factories = g_list_concat(cut_factory_builder_build(builder), factories);
    }
    return factories;
}

GList *
cut_contractor_build_all_factories (CutContractor *contractor)
{
    GList *factories = NULL, *node;
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);

    for (node = priv->builders; node; node = g_list_next(node)) {
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        factories = g_list_concat(cut_factory_builder_build_all(builder), factories);
    }
    return factories;
}

void
cut_contractor_set_option_context (CutContractor *contractor, GOptionContext *context)
{
    GList *node;
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);

    for (node = priv->builders; node; node = g_list_next(node)) {
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        cut_factory_builder_set_option_context(builder, context);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
