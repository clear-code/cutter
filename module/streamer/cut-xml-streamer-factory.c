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
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-streamer.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_XML_STREAMER_FACTORY            cut_type_xml_streamer_factory
#define CUT_XML_STREAMER_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_XML_STREAMER_FACTORY, CutXMLStreamerFactory))
#define CUT_XML_STREAMER_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_XML_STREAMER_FACTORY, CutXMLStreamerFactoryClass))
#define CUT_IS_XML_STREAMER_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_XML_STREAMER_FACTORY))
#define CUT_IS_XML_STREAMER_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_XML_STREAMER_FACTORY))
#define CUT_XML_STREAMER_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_XML_STREAMER_FACTORY, CutXMLStreamerFactoryClass))

typedef struct _CutXMLStreamerFactory CutXMLStreamerFactory;
typedef struct _CutXMLStreamerFactoryClass CutXMLStreamerFactoryClass;

struct _CutXMLStreamerFactory
{
    CutModuleFactory     object;

    gint                 fd;
};

struct _CutXMLStreamerFactoryClass
{
    CutModuleFactoryClass parent_class;
};

static GType cut_type_xml_streamer_factory = 0;
static CutModuleFactoryClass *parent_class;

static void     dispose          (GObject         *object);
static void     set_option_group (CutModuleFactory *factory,
                                  GOptionContext   *context);
static GObject *create           (CutModuleFactory *factory);

static void
class_init (CutModuleFactoryClass *klass)
{
    CutModuleFactoryClass *factory_class;
    GObjectClass *gobject_class;

    parent_class = g_type_class_peek_parent(klass);
    gobject_class = G_OBJECT_CLASS(klass);
    factory_class  = CUT_MODULE_FACTORY_CLASS(klass);

    gobject_class->dispose      = dispose;

    factory_class->set_option_group = set_option_group;
    factory_class->create           = create;
}

static void
init (CutXMLStreamerFactory *xml)
{
    xml->fd = -1;
}

static void
dispose (GObject *object)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutXMLStreamerFactoryClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutXMLStreamerFactory),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_xml_streamer_factory =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_MODULE_FACTORY,
                                    "CutXMLStreamerFactory",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_xml_streamer_factory)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_xml_streamer_factory));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_XML_STREAMER_FACTORY, first_property, var_args);
}

static void
set_option_group (CutModuleFactory *factory, GOptionContext *context)
{
    CutXMLStreamerFactory *xml = CUT_XML_STREAMER_FACTORY(factory);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {"stream-fd", 0, 0, G_OPTION_ARG_INT, &(xml->fd),
         N_("Stream to FILE_DESCRIPTOR (default: stdout)"), "FILE_DESCRIPTOR"},
        {NULL}
    };

    if (CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group)
        CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group(factory, context);

    group = g_option_group_new(("xml-streamer"),
                               _("XML Streamer Options"),
                               _("Show XML streamer options"),
                               xml, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

GObject *
create (CutModuleFactory *factory)
{
    return G_OBJECT(cut_streamer_new("xml",
                                     "fd", CUT_XML_STREAMER_FACTORY(factory)->fd,
                                     NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
