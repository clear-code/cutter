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

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-module.h"
#include "cut-factory-builder.h"
#include "cut-report-factory-builder.h"
#include "cut-listener.h"
#include "cut-enum-types.h"

#define CUT_FACTORY_BUILDER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_FACTORY_BUILDER, CutFactoryBuilderPrivate))

typedef struct _CutFactoryBuilderPrivate    CutFactoryBuilderPrivate;
struct _CutFactoryBuilderPrivate
{
    GOptionContext *option_context;
    GList *factories;
};

enum
{
    PROP_0,
    PROP_OPTION_CONTEXT
};

G_DEFINE_ABSTRACT_TYPE(CutFactoryBuilder, cut_factory_builder, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);
static void
cut_factory_builder_class_init (CutFactoryBuilderClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_pointer("option-context",
                                "GOptionContext",
                                "GOptionContext",
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_OPTION_CONTEXT, spec);

    g_type_class_add_private(gobject_class, sizeof(CutFactoryBuilderPrivate));
}

static void
cut_factory_builder_init (CutFactoryBuilder *builder)
{
    CutFactoryBuilderPrivate *priv = CUT_FACTORY_BUILDER_GET_PRIVATE(builder);

    priv->option_context = NULL;
    priv->factories = NULL;
}

static void
dispose (GObject *object)
{
    CutFactoryBuilderPrivate *priv = CUT_FACTORY_BUILDER_GET_PRIVATE(object);

    if (priv->factories) {
        g_list_free(priv->factories);
        priv->factories = NULL;
    }
    priv->option_context = NULL;

    G_OBJECT_CLASS(cut_factory_builder_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutFactoryBuilderPrivate *priv = CUT_FACTORY_BUILDER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_OPTION_CONTEXT:
        priv->option_context = g_value_get_pointer(value);
        break;
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
    CutFactoryBuilderPrivate *priv = CUT_FACTORY_BUILDER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_OPTION_CONTEXT:
        g_value_set_pointer(value, priv->option_context);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

void
cut_factory_builder_set_option_context (CutFactoryBuilder *builder,
                                         GOptionContext *context)
{
    CutFactoryBuilderClass *klass;
    CutFactoryBuilderPrivate *priv = CUT_FACTORY_BUILDER_GET_PRIVATE(builder);

    g_return_if_fail(CUT_IS_FACTORY_BUILDER(builder));

    klass = CUT_FACTORY_BUILDER_GET_CLASS(builder);
    g_return_if_fail(klass->set_option_context);

    priv->option_context = context;
    klass->set_option_context(builder, context);
}

void
cut_factory_builder_build (CutFactoryBuilder *builder)
{
    CutFactoryBuilderClass *klass;
    CutFactoryBuilderPrivate *priv = CUT_FACTORY_BUILDER_GET_PRIVATE(builder);

    g_return_if_fail(CUT_IS_FACTORY_BUILDER(builder));

    klass = CUT_FACTORY_BUILDER_GET_CLASS(builder);
    g_return_if_fail(klass->build);

    priv->factories = klass->build(builder);
}

GType
cut_factory_builder_get_child_type_from_name (const gchar *type_name)
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

void
cut_factory_builder_register_builder (void)
{
    g_type_class_unref(g_type_class_ref(CUT_TYPE_REPORT_FACTORY_BUILDER));
    /* g_type_class_unref(g_type_class_ref(CUT_TYPE_UI_FACTORY_BUILDER)); */
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
