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
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>
#include <gtk/gtk.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-runner.h>
#include <cutter/cut-ui-factory.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_RUNNER_FACTORY_GTK            cut_type_runner_factory_gtk
#define CUT_RUNNER_FACTORY_GTK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUNNER_FACTORY_GTK, CutRunnerFactoryGtk))
#define CUT_RUNNER_FACTORY_GTK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUNNER_FACTORY_GTK, CutRunnerFactoryGtkClass))
#define CUT_IS_RUNNER_FACTORY_GTK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUNNER_FACTORY_GTK))
#define CUT_IS_RUNNER_FACTORY_GTK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUNNER_FACTORY_GTK))
#define CUT_RUNNER_FACTORY_GTK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUNNER_FACTORY_GTK, CutRunnerFactoryGtkClass))

typedef struct _CutRunnerFactoryGtk CutRunnerFactoryGtk;
typedef struct _CutRunnerFactoryGtkClass CutRunnerFactoryGtkClass;

struct _CutRunnerFactoryGtk
{
    CutRunnerFactory     object;
};

struct _CutRunnerFactoryGtkClass
{
    CutRunnerFactoryClass parent_class;
};

enum
{
    PROP_0
};

static GType cut_type_runner_factory_gtk = 0;
static CutRunnerFactoryClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void       set_option_group (CutRunnerFactory    *factory,
                                    GOptionContext      *context);
static CutRunner *create           (CutRunnerFactory    *factory);

static void
class_init (CutRunnerFactoryClass *klass)
{
    GObjectClass *gobject_class;
    CutRunnerFactoryClass *factory_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class  = CUT_RUNNER_FACTORY_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    factory_class->set_option_group = set_option_group;
    factory_class->create           = create;
}

static void
init (CutRunnerFactoryGtk *gtk)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutRunnerFactoryGtkClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutRunnerFactoryGtk),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_runner_factory_gtk =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_RUNNER_FACTORY,
                                    "CutRunnerFactoryGtk",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_runner_factory_gtk)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_runner_factory_gtk));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_RUNNER_FACTORY_GTK, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(parent_class)->dispose(object);
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

static void
set_option_group (CutRunnerFactory *factory, GOptionContext *context)
{
    GOptionGroup *group;

    CUT_RUNNER_FACTORY_CLASS(parent_class)->set_option_group(factory, context);

    group = gtk_get_option_group(TRUE);
    g_option_context_add_group(context, group);
}

CutRunner *
create (CutRunnerFactory *factory)
{
    return cut_runner_new("gtk", NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
