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
#include <unistd.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-stream.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_XML_STREAM_FACTORY            cut_type_xml_stream_factory
#define CUT_XML_STREAM_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_XML_STREAM_FACTORY, CutXMLStreamFactory))
#define CUT_XML_STREAM_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_XML_STREAM_FACTORY, CutXMLStreamFactoryClass))
#define CUT_IS_XML_STREAM_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_XML_STREAM_FACTORY))
#define CUT_IS_XML_STREAM_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_XML_STREAM_FACTORY))
#define CUT_XML_STREAM_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_XML_STREAM_FACTORY, CutXMLStreamFactoryClass))

typedef struct _CutXMLStreamFactory CutXMLStreamFactory;
typedef struct _CutXMLStreamFactoryClass CutXMLStreamFactoryClass;

struct _CutXMLStreamFactory
{
    CutModuleFactory     object;

    gint                 fd;
    gchar               *log_directory;
};

struct _CutXMLStreamFactoryClass
{
    CutModuleFactoryClass parent_class;
};

enum
{
    PROP_0,
    PROP_FD,
    PROP_LOG_DIRECTORY
};

static GType cut_type_xml_stream_factory = 0;
static CutModuleFactoryClass *parent_class;

static void     dispose          (GObject         *object);
static void     set_property     (GObject         *object,
                                  guint            prop_id,
                                  const GValue    *value,
                                  GParamSpec      *pspec);
static void     get_property     (GObject         *object,
                                  guint            prop_id,
                                  GValue          *value,
                                  GParamSpec      *pspec);
static void     set_option_group (CutModuleFactory *factory,
                                  GOptionContext   *context);
static GObject *create           (CutModuleFactory *factory);

static void
class_init (CutModuleFactoryClass *klass)
{
    CutModuleFactoryClass *factory_class;
    GObjectClass *gobject_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);
    gobject_class = G_OBJECT_CLASS(klass);
    factory_class  = CUT_MODULE_FACTORY_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    factory_class->set_option_group = set_option_group;
    factory_class->create           = create;

    spec = g_param_spec_int("fd",
                            "FD",
                            "The FD of output stream",
                            G_MININT32, G_MAXINT32, -1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_FD, spec);

    spec = g_param_spec_string("log-directory",
                               "Log Directory",
                               "The directory of log files",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_LOG_DIRECTORY, spec);
}

static void
init (CutXMLStreamFactory *factory)
{
    factory->fd = -1;
    factory->log_directory = NULL;
}

static void
dispose (GObject *object)
{
    CutXMLStreamFactory *factory;

    factory = CUT_XML_STREAM_FACTORY(object);
    if (factory->log_directory) {
        g_free(factory->log_directory);
        factory->log_directory = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutXMLStreamFactory *factory;

    factory = CUT_XML_STREAM_FACTORY(object);
    switch (prop_id) {
      case PROP_FD:
        factory->fd = g_value_get_int(value);
        break;
      case PROP_LOG_DIRECTORY:
        if (factory->log_directory)
            g_free(factory->log_directory);
        factory->log_directory = g_value_dup_string(value);
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
    CutXMLStreamFactory *factory;

    factory = CUT_XML_STREAM_FACTORY(object);
    switch (prop_id) {
      case PROP_FD:
        g_value_set_int(value, factory->fd);
        break;
      case PROP_LOG_DIRECTORY:
        g_value_set_string(value, factory->log_directory);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutXMLStreamFactoryClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutXMLStreamFactory),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_xml_stream_factory =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_MODULE_FACTORY,
                                    "CutXMLStreamFactory",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_xml_stream_factory)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_xml_stream_factory));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_XML_STREAM_FACTORY, first_property, var_args);
}

static void
set_option_group (CutModuleFactory *factory, GOptionContext *context)
{
    CutXMLStreamFactory *xml = CUT_XML_STREAM_FACTORY(factory);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {NULL}
    };

    if (CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group)
        CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group(factory, context);

    return;

    group = g_option_group_new(("xml-stream"),
                               _("XML Stream Options"),
                               _("Show XML stream options"),
                               xml, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

typedef struct _StreamData StreamData;
struct _StreamData
{
    gboolean initialized;
    gint fd;
    gchar *log_directory;
    GList *channels;
};

static StreamData *
stream_data_new (gint fd, gchar *log_directory)
{
    StreamData *data;

    data = g_slice_new(StreamData);
    data->initialized = FALSE;
    data->fd = fd;
    data->log_directory = g_strdup(log_directory);
    data->channels = NULL;

    return data;
}

static void
stream_data_free (StreamData *data)
{
    if (data->log_directory)
        g_free(data->log_directory);

    if (data->channels) {
        g_list_foreach(data->channels, (GFunc)g_io_channel_unref, NULL);
        g_list_free(data->channels);
    }

    g_slice_free(StreamData, data);
}

static GList *
create_channels (StreamData *data, GError **error)
{
    gint fd = -1;
    GList *channels = NULL;
    GIOChannel *channel;

    if (data->fd == -1 && !data->log_directory)
        fd = STDOUT_FILENO;
    else
        fd = data->fd;

    if (fd != -1) {
#ifdef G_OS_WIN32
        channel = g_io_channel_win32_new_fd(fd);
#else
        channel = g_io_channel_unix_new(fd);
#endif

        if (channel) {
            g_io_channel_set_close_on_unref(channel, TRUE);
            channels = g_list_prepend(channels, channel);
        }
    }

    if (data->log_directory) {
        gchar *file_name, *base_name;
        time_t now;
        struct tm *tm;

        time(&now);
        tm = gmtime(&now);
        base_name = g_strdup_printf("%04d-%02d-%02d-%02d-%02d-%02d.xml",
                                    1900 + tm->tm_year,
                                    tm->tm_mon + 1,
                                    tm->tm_mday,
                                    tm->tm_hour,
                                    tm->tm_min,
                                    tm->tm_sec);
        file_name = g_build_filename(data->log_directory, base_name, NULL);

        g_mkdir_with_parents(data->log_directory, 0755);
        channel = g_io_channel_new_file(file_name, "w", error);
        if (channel) {
            g_io_channel_set_close_on_unref(channel, TRUE);
            channels = g_list_prepend(channels, channel);
        }
        g_free(base_name);
        g_free(file_name);
    }

    return channels;
}

static gboolean
stream (const gchar *message, GError **error, gpointer user_data)
{
    StreamData *data = user_data;
    GList *node;
    gsize length;

    if (!data->initialized) {
        data->channels = create_channels(data, error);
        data->initialized = TRUE;
        if (*error)
            return FALSE;
    }

    if (!data->channels)
        return FALSE;

    length = strlen(message);
    for (node = data->channels; node; node = g_list_next(node)) {
        GIOChannel *channel = node->data;
        gsize len, written;
        const gchar *snippet;

        len = length;
        written = 0;
        snippet = message;
        while (len > 0) {
            g_io_channel_write_chars(channel, snippet, len, &written, error);
            if (*error)
                break;

            snippet += written;
            len -= written;
        }
        g_io_channel_flush(channel, NULL);

        if (*error)
            break;
    }

    return *error == NULL;
}

GObject *
create (CutModuleFactory *factory)
{
    CutXMLStreamFactory *xml_factory;
    StreamData *data;

    xml_factory = CUT_XML_STREAM_FACTORY(factory);
    data = stream_data_new(xml_factory->fd, xml_factory->log_directory);
    return G_OBJECT(cut_stream_new("xml",
                                   "stream-function", stream,
                                   "stream-function-user-data", data,
                                   "stream-function-user-data-destroy-function",
                                   stream_data_free,
                                   NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
