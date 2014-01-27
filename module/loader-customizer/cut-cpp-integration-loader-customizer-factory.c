/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014  Kouhei Sutou <kou@clear-code.com>
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
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-loader-customizer.h>

#define CUT_TYPE_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY    \
    cut_type_cpp_integration_loader_customizer_factory
#define CUT_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY(obj)              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CUT_TYPE_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY, \
                                CutCppIntegrationLoaderCustomizerFactory))
#define CUT_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY_CLASS(klass)      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                                   \
                             CUT_TYPE_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY, \
                             CutCppIntegrationLoaderCustomizerFactoryClass))
#define CUT_IS_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY(obj)           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CUT_TYPE_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY))
#define CUT_IS_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY_CLASS(klass)   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CUT_TYPE_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY))
#define CUT_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY_GET_CLASS(obj)    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                             CUT_TYPE_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY, \
                             CutCppIntegrationLoaderCustomizerFactoryClass))

typedef struct _CutCppIntegrationLoaderCustomizerFactory CutCppIntegrationLoaderCustomizerFactory;
typedef struct _CutCppIntegrationLoaderCustomizerFactoryClass CutCppIntegrationLoaderCustomizerFactoryClass;

struct _CutCppIntegrationLoaderCustomizerFactory
{
    CutModuleFactory     object;
};

struct _CutCppIntegrationLoaderCustomizerFactoryClass
{
    CutModuleFactoryClass parent_class;
};

static GType cut_type_cpp_integration_loader_customizer_factory = 0;
static CutModuleFactoryClass *parent_class;

static GObject   *create           (CutModuleFactory    *factory);

static void
class_init (CutModuleFactoryClass *klass)
{
    CutModuleFactoryClass *factory_class;

    parent_class = g_type_class_peek_parent(klass);

    factory_class = CUT_MODULE_FACTORY_CLASS(klass);

    factory_class->create = create;
}

static void
init (CutCppIntegrationLoaderCustomizerFactory *console)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(CutCppIntegrationLoaderCustomizerFactoryClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(CutCppIntegrationLoaderCustomizerFactory),
        0,
        (GInstanceInitFunc)init,
    };

    cut_type_cpp_integration_loader_customizer_factory =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_MODULE_FACTORY,
                                    "CutCppIntegrationLoaderCustomizerFactory",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_cpp_integration_loader_customizer_factory) {
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_cpp_integration_loader_customizer_factory));
    }

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_CPP_INTEGRATION_LOADER_CUSTOMIZER_FACTORY, first_property, var_args);
}

GObject *
create (CutModuleFactory *factory)
{
    return G_OBJECT(cut_loader_customizer_new("cpp-integration",
                                              NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
