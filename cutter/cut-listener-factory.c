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
#include "cut-listener-factory.h"
#include "cut-listener.h"
#include "cut-enum-types.h"

#define CUT_LISTENER_FACTORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_LISTENER_FACTORY, CutListenerFactoryPrivate))

typedef struct _CutListenerFactoryPrivate    CutListenerFactoryPrivate;
struct _CutListenerFactoryPrivate
{
    GOptionContext *option_context;
};

enum
{
    PROP_0,
    PROP_OPTION_CONTEXT
};

G_DEFINE_ABSTRACT_TYPE(CutListenerFactory, cut_listener_factory, G_TYPE_OBJECT)

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
cut_listener_factory_class_init (CutListenerFactoryClass *klass)
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

    g_type_class_add_private(gobject_class, sizeof(CutListenerFactoryPrivate));
}

static void
cut_listener_factory_init (CutListenerFactory *factory)
{
    CutListenerFactoryPrivate *priv = CUT_LISTENER_FACTORY_GET_PRIVATE(factory);

    priv->option_context = NULL;
}

static void
dispose (GObject *object)
{
    CutListenerFactoryPrivate *priv = CUT_LISTENER_FACTORY_GET_PRIVATE(object);

    priv->option_context = NULL;

    G_OBJECT_CLASS(cut_listener_factory_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutListenerFactoryPrivate *priv = CUT_LISTENER_FACTORY_GET_PRIVATE(object);

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
    CutListenerFactoryPrivate *priv = CUT_LISTENER_FACTORY_GET_PRIVATE(object);

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
cut_listener_factory_set_option_context (CutListenerFactory *factory,
                                         GOptionContext *context)
{
    CutListenerFactoryClass *klass;
    CutListenerFactoryPrivate *priv = CUT_LISTENER_FACTORY_GET_PRIVATE(factory);

    g_return_if_fail(CUT_IS_LISTENER_FACTORY(factory));

    klass = CUT_LISTENER_FACTORY_GET_CLASS(factory);
    g_return_if_fail(klass->set_option_context);

    priv->option_context = context;
    klass->set_option_context(factory, context);
}

void
cut_listener_factory_activate (CutListenerFactory *factory)
{
    CutListenerFactoryClass *klass;

    g_return_if_fail(CUT_IS_LISTENER_FACTORY(factory));

    klass = CUT_LISTENER_FACTORY_GET_CLASS(factory);
    g_return_if_fail(klass->activate);

    klass->activate(factory);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
