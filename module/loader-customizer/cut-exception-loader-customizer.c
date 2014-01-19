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
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-loader-customizer.h>

#include <cppcutter/cppcut-test.h>

#define CUT_TYPE_EXCEPTION_LOADER_CUSTOMIZER    \
    cut_type_exception_loader_customizer
#define CUT_EXCEPTION_LOADER_CUSTOMIZER(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CUT_TYPE_EXCEPTION_LOADER_CUSTOMIZER,   \
                                CutExceptionLoaderCustomizer))
#define CUT_EXCEPTION_LOADER_CUSTOMIZER_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_CAST((klass),                                   \
                             CUT_TYPE_EXCEPTION_LOADER_CUSTOMIZER,      \
                             CutExceptionLoaderCustomizerClass))
#define CUT_IS_EXCEPTION_LOADER_CUSTOMIZER(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CUT_TYPE_EXCEPTION_LOADER_CUSTOMIZER))
#define CUT_IS_EXCEPTION_LOADER_CUSTOMIZER_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CUT_TYPE_EXCEPTION_LOADER_CUSTOMIZER))
#define CUT_EXCEPTION_LOADER_CUSTOMIZER_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CUT_TYPE_EXCEPTION_LOADER_CUSTOMIZER,    \
                               CutExceptionLoaderCustomizerClass))

typedef struct _CutExceptionLoaderCustomizer CutExceptionLoaderCustomizer;
typedef struct _CutExceptionLoaderCustomizerClass CutExceptionLoaderCustomizerClass;

struct _CutExceptionLoaderCustomizer
{
    CutLoaderCustomizer parent;
};

struct _CutExceptionLoaderCustomizerClass
{
    CutLoaderCustomizerClass parent_class;
};

static GType cut_type_exception_loader_customizer = 0;
static GObjectClass *parent_class;

static void     customize   (CutLoaderCustomizer *customizer,
                             CutLoader           *loader);

static void
class_init (CutExceptionLoaderCustomizerClass *klass)
{
    CutLoaderCustomizerClass *loader_customizer_class;

    parent_class = g_type_class_peek_parent(klass);

    loader_customizer_class = CUT_LOADER_CUSTOMIZER_CLASS(klass);
    loader_customizer_class->customize = customize;
}

static void
init (CutExceptionLoaderCustomizer *customizer)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(CutExceptionLoaderCustomizerClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(CutExceptionLoaderCustomizer),
        0,
        (GInstanceInitFunc)init,
    };

    cut_type_exception_loader_customizer =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_LOADER_CUSTOMIZER,
                                    "CutExceptionLoaderCustomizer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_exception_loader_customizer) {
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_exception_loader_customizer));
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
    return g_object_new_valist(CUT_TYPE_EXCEPTION_LOADER_CUSTOMIZER,
                               first_property, var_args);
}

static CutTest *
create_test (const gchar     *name,
             CutTestFunction  function,
             gpointer         user_data)
{
    return CUT_TEST(cppcut_test_new(name, function));
}

static void
customize (CutLoaderCustomizer *customizer, CutLoader *loader)
{
    cut_loader_set_create_test_function(loader, create_test, NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
