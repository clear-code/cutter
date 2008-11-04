/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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

#include "cut-file-stream-reader.h"
#include "cut-runner.h"

#define CUT_FILE_STREAM_READER_GET_PRIVATE(obj)                         \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_FILE_STREAM_READER,    \
                                 CutFileStreamReaderPrivate))

typedef struct _CutFileStreamReaderPrivate	CutFileStreamReaderPrivate;
struct _CutFileStreamReaderPrivate
{
    gchar *file_name;
    GIOChannel *channel;
    guint watch_source_id;
};

enum
{
    PROP_0,
    PROP_FILE_NAME
};

static CutRunnerIface *parent_runner_iface;

static void runner_init (CutRunnerIface *iface);

G_DEFINE_TYPE_WITH_CODE(CutFileStreamReader, cut_file_stream_reader,
                        CUT_TYPE_STREAM_READER,
                        G_IMPLEMENT_INTERFACE(CUT_TYPE_RUNNER, runner_init))

static void     dispose          (GObject         *object);
static void     set_property     (GObject         *object,
                                  guint            prop_id,
                                  const GValue    *value,
                                  GParamSpec      *pspec);
static void     get_property     (GObject         *object,
                                  guint            prop_id,
                                  GValue          *value,
                                  GParamSpec      *pspec);

static void     runner_run_async (CutRunner       *runner);

static void
cut_file_stream_reader_class_init (CutFileStreamReaderClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("file-name",
                               "File name",
                               "The name of the streamed XML file",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FILE_NAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutFileStreamReaderPrivate));
}

static void
cut_file_stream_reader_init (CutFileStreamReader *file_stream_reader)
{
    CutFileStreamReaderPrivate *priv;

    priv = CUT_FILE_STREAM_READER_GET_PRIVATE(file_stream_reader);

    priv->file_name = NULL;
    priv->channel = NULL;
    priv->watch_source_id = 0;
}

static void
runner_init (CutRunnerIface *iface)
{
    parent_runner_iface = g_type_interface_peek_parent(iface);
    iface->run_async = runner_run_async;
    iface->run       = NULL;
}

static void
free_file_name (CutFileStreamReaderPrivate *priv)
{
    if (priv->file_name) {
        g_free(priv->file_name);
        priv->file_name = NULL;
    }
}

static void
remove_watch_func (CutFileStreamReaderPrivate *priv)
{
    g_source_remove(priv->watch_source_id);
    priv->watch_source_id = 0;
}

static void
dispose (GObject *object)
{
    CutFileStreamReaderPrivate *priv;

    priv = CUT_FILE_STREAM_READER_GET_PRIVATE(object);
    free_file_name(priv);

    if (priv->watch_source_id)
        remove_watch_func(priv);

    if (priv->channel) {
        g_io_channel_unref(priv->channel);
        priv->channel = NULL;
    }

    G_OBJECT_CLASS(cut_file_stream_reader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutFileStreamReaderPrivate *priv;

    priv = CUT_FILE_STREAM_READER_GET_PRIVATE(object);
    switch (prop_id) {
      case PROP_FILE_NAME:
        free_file_name(priv);
        priv->file_name = g_value_dup_string(value);
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
    CutFileStreamReaderPrivate *priv;

    priv = CUT_FILE_STREAM_READER_GET_PRIVATE(object);
    switch (prop_id) {
      case PROP_FILE_NAME:
        g_value_set_string(value, priv->file_name);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutRunContext *
cut_file_stream_reader_new (const gchar *file_name)
{
    return g_object_new(CUT_TYPE_FILE_STREAM_READER,
                        "file-name", file_name,
                        NULL);
}

GQuark
cut_file_stream_reader_error_quark (void)
{
    return g_quark_from_static_string("cut-file-stream-reader-error-quark");
}

#define emit_error(file_stream_reader, code, sub_error, format, ...) do \
{                                                                       \
    CutRunContext *_run_context;                                        \
                                                                        \
    _run_context = CUT_RUN_CONTEXT(file_stream_reader);                 \
    cut_run_context_emit_error(_run_context,                            \
                               CUT_FILE_STREAM_READER_ERROR,            \
                               code, sub_error,                         \
                               format, ## __VA_ARGS__);                 \
    emit_complete_signal(file_stream_reader, FALSE);                    \
} while (0)

static void
emit_complete_signal (CutFileStreamReader *file_stream_reader, gboolean success)
{
    g_signal_emit_by_name(file_stream_reader, "complete-run", success);
}

static void
runner_run_async (CutRunner *runner)
{
    CutStreamReader *stream_reader;
    CutFileStreamReader *file_stream_reader;
    CutFileStreamReaderPrivate *priv;
    GError *error = NULL;

    file_stream_reader = CUT_FILE_STREAM_READER(runner);
    stream_reader = CUT_STREAM_READER(file_stream_reader);
    priv = CUT_FILE_STREAM_READER_GET_PRIVATE(file_stream_reader);
    priv->channel = g_io_channel_new_file(priv->file_name, "r", &error);
    if (error) {
        emit_error(file_stream_reader, CUT_FILE_STREAM_READER_ERROR_FILE, error,
                   "can't open file: %s", priv->file_name);
        return;
    }

    g_io_channel_set_close_on_unref(priv->channel, TRUE);
    priv->watch_source_id = cut_stream_reader_watch_io_channel(stream_reader,
                                                               priv->channel);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
