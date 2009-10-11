/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#include "cut-console-diff-writer.h"
#include "cut-console.h"

#define DEFAULT_DELETED_MARK_COLOR CUT_CONSOLE_COLOR_RED
#define DEFAULT_INSERTED_MARK_COLOR CUT_CONSOLE_COLOR_GREEN

#define DEFAULT_DELETED_COLOR                   \
    CUT_CONSOLE_COLOR_WHITE                     \
    CUT_CONSOLE_COLOR_RED_BACK
#define DEFAULT_INSERTED_COLOR                  \
    CUT_CONSOLE_COLOR_WHITE                     \
    CUT_CONSOLE_COLOR_GREEN_BACK

#define CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CONSOLE_DIFF_WRITER, CutConsoleDiffWriterPrivate))

typedef struct _CutConsoleDiffWriterPrivate         CutConsoleDiffWriterPrivate;
struct _CutConsoleDiffWriterPrivate
{
    gboolean use_color;
    gchar *deleted_mark_color;
    gchar *inserted_mark_color;
    gchar *deleted_color;
    gchar *inserted_color;
};

enum {
    PROP_0,
    PROP_USE_COLOR
};

G_DEFINE_TYPE(CutConsoleDiffWriter, cut_console_diff_writer, CUT_TYPE_DIFF_WRITER)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void write          (CutDiffWriter       *writer,
                            const gchar         *console,
                            CutDiffWriterTag     tag);
static void write_mark     (CutDiffWriter       *writer,
                            const gchar         *mark,
                            CutDiffWriterTag     tag);
static void write_line     (CutDiffWriter       *writer,
                            const gchar         *line,
                            CutDiffWriterTag     tag);
static void finish         (CutDiffWriter       *writer);

static void
cut_console_diff_writer_class_init (CutConsoleDiffWriterClass *klass)
{
    GObjectClass *gobject_class;
    CutDiffWriterClass *diff_writer_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);
    diff_writer_class = CUT_DIFF_WRITER_CLASS(klass);

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    diff_writer_class->write = write;
    diff_writer_class->write_mark = write_mark;
    diff_writer_class->write_line = write_line;
    diff_writer_class->finish = finish;

    spec = g_param_spec_boolean("use-color",
                                "Use color",
                                "Whether use color",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_USE_COLOR, spec);

    g_type_class_add_private(gobject_class, sizeof(CutConsoleDiffWriterPrivate));
}

static void
cut_console_diff_writer_init (CutConsoleDiffWriter *writer)
{
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer);
    priv->deleted_mark_color = g_strdup(DEFAULT_DELETED_MARK_COLOR);
    priv->inserted_mark_color = g_strdup(DEFAULT_INSERTED_MARK_COLOR);
    priv->deleted_color = g_strdup(DEFAULT_DELETED_COLOR);
    priv->inserted_color = g_strdup(DEFAULT_INSERTED_COLOR);
}

static void
dispose (GObject *object)
{
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(object);

    if (priv->deleted_mark_color) {
        g_free(priv->deleted_mark_color);
        priv->deleted_mark_color = NULL;
    }

    if (priv->inserted_mark_color) {
        g_free(priv->inserted_mark_color);
        priv->inserted_mark_color = NULL;
    }

    if (priv->deleted_color) {
        g_free(priv->deleted_color);
        priv->deleted_color = NULL;
    }

    if (priv->inserted_color) {
        g_free(priv->inserted_color);
        priv->inserted_color = NULL;
    }

    G_OBJECT_CLASS(cut_console_diff_writer_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_USE_COLOR:
        priv->use_color = g_value_get_boolean(value);
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
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_USE_COLOR:
        g_value_set_boolean(value, priv->use_color);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutDiffWriter *
cut_console_diff_writer_new (gboolean use_color)
{
    return g_object_new(CUT_TYPE_CONSOLE_DIFF_WRITER,
                        "use-color", use_color,
                        NULL);
}

void
cut_console_diff_writer_set_deleted_mark_color (CutDiffWriter *writer,
                                                const gchar *color)
{
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer);
    g_free(priv->deleted_mark_color);
    if (!color)
        color = DEFAULT_DELETED_MARK_COLOR;
    priv->deleted_mark_color = g_strdup(color);
}

const gchar *
cut_console_diff_writer_get_deleted_mark_color (CutDiffWriter *writer)
{
    return CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer)->deleted_mark_color;
}

void
cut_console_diff_writer_set_inserted_mark_color (CutDiffWriter *writer,
                                                 const gchar *color)
{
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer);
    g_free(priv->inserted_mark_color);
    if (!color)
        color = DEFAULT_INSERTED_MARK_COLOR;
    priv->inserted_mark_color = g_strdup(color);
}

const gchar *
cut_console_diff_writer_get_inserted_mark_color (CutDiffWriter *writer)
{
    return CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer)->inserted_mark_color;
}

void
cut_console_diff_writer_set_deleted_color (CutDiffWriter *writer,
                                           const gchar *color)
{
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer);
    g_free(priv->deleted_color);
    if (!color)
        color = DEFAULT_DELETED_COLOR;
    priv->deleted_color = g_strdup(color);
}

const gchar *
cut_console_diff_writer_get_deleted_color (CutDiffWriter *writer)
{
    return CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer)->deleted_color;
}

void
cut_console_diff_writer_set_inserted_color (CutDiffWriter *writer,
                                            const gchar *color)
{
    CutConsoleDiffWriterPrivate *priv;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer);
    g_free(priv->inserted_color);
    if (!color)
        color = DEFAULT_INSERTED_COLOR;
    priv->inserted_color = g_strdup(color);
}

const gchar *
cut_console_diff_writer_get_inserted_color (CutDiffWriter *writer)
{
    return CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer)->inserted_color;
}

static const gchar *
tag_to_color (CutConsoleDiffWriterPrivate *priv, CutDiffWriterTag tag)
{
    const gchar *color;

    switch (tag) {
    case CUT_DIFF_WRITER_TAG_DELETED_MARK:
        color = priv->deleted_mark_color;
        break;
    case CUT_DIFF_WRITER_TAG_INSERTED_MARK:
        color = priv->inserted_mark_color;
        break;
    case CUT_DIFF_WRITER_TAG_DIFFERENCE_MARK:
        color = CUT_CONSOLE_COLOR_CYAN;
        break;
    case CUT_DIFF_WRITER_TAG_DELETED:
        color = priv->deleted_color;
        break;
    case CUT_DIFF_WRITER_TAG_INSERTED:
        color = priv->inserted_color;
        break;
    default:
        color = NULL;
        break;
    }

    return color;
}

static void
write (CutDiffWriter *writer, const gchar *string, CutDiffWriterTag tag)
{
    CutConsoleDiffWriterPrivate *priv;
    const gchar *color = NULL;

    priv = CUT_CONSOLE_DIFF_WRITER_GET_PRIVATE(writer);
    if (priv->use_color)
        color = tag_to_color(priv, tag);

    if (color)
        g_print("%s%s%s", color, string, CUT_CONSOLE_COLOR_NORMAL);
    else
        g_print("%s", string);
}

static void
write_mark (CutDiffWriter *writer, const gchar *mark, CutDiffWriterTag tag)
{
    write(writer, mark, tag);
    g_print(" ");
}

static void
write_line (CutDiffWriter *writer, const gchar *line, CutDiffWriterTag tag)
{
    write(writer, line, tag);
    g_print("\n");
}

static void
finish (CutDiffWriter *writer)
{
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
