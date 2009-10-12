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

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-string-diff-writer.h"
#include "cut-utils.h"

#define CUT_STRING_DIFF_WRITER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_STRING_DIFF_WRITER, CutStringDiffWriterPrivate))

typedef struct _CutStringDiffWriterPrivate         CutStringDiffWriterPrivate;
struct _CutStringDiffWriterPrivate
{
    GString *result;
};

G_DEFINE_TYPE(CutStringDiffWriter, cut_string_diff_writer, CUT_TYPE_DIFF_WRITER)

static void dispose        (GObject         *object);

static void write          (CutDiffWriter       *writer,
                            const gchar         *string,
                            CutDiffWriterTag     tag);
static void write_line     (CutDiffWriter       *writer,
                            const gchar         *line,
                            CutDiffWriterTag     tag);
static void finish         (CutDiffWriter       *writer);

static void
cut_string_diff_writer_class_init (CutStringDiffWriterClass *klass)
{
    GObjectClass *gobject_class;
    CutDiffWriterClass *diff_writer_class;

    gobject_class = G_OBJECT_CLASS(klass);
    diff_writer_class = CUT_DIFF_WRITER_CLASS(klass);

    gobject_class->dispose = dispose;

    diff_writer_class->write = write;
    diff_writer_class->write_line = write_line;
    diff_writer_class->finish = finish;

    g_type_class_add_private(gobject_class, sizeof(CutStringDiffWriterPrivate));
}

static void
cut_string_diff_writer_init (CutStringDiffWriter *writer)
{
    CutStringDiffWriterPrivate *priv;

    priv = CUT_STRING_DIFF_WRITER_GET_PRIVATE(writer);
    priv->result = g_string_new(NULL);
}

static void
dispose (GObject *object)
{
    CutStringDiffWriterPrivate *priv;

    priv = CUT_STRING_DIFF_WRITER_GET_PRIVATE(object);

    if (priv->result) {
        g_string_free(priv->result, TRUE);
        priv->result = NULL;
    }

    G_OBJECT_CLASS(cut_string_diff_writer_parent_class)->dispose(object);
}

CutDiffWriter *
cut_string_diff_writer_new (void)
{
    return g_object_new(CUT_TYPE_STRING_DIFF_WRITER, NULL);
}

const gchar *
cut_string_diff_writer_get_result (CutDiffWriter *writer)
{
    return CUT_STRING_DIFF_WRITER_GET_PRIVATE(writer)->result->str;
}

static void
write (CutDiffWriter *writer, const gchar *string, CutDiffWriterTag tag)
{
    CutStringDiffWriterPrivate *priv;

    priv = CUT_STRING_DIFF_WRITER_GET_PRIVATE(writer);
    g_string_append(priv->result, string);
}

static void
write_line (CutDiffWriter *writer, const gchar *line, CutDiffWriterTag tag)
{
    CutStringDiffWriterPrivate *priv;

    priv = CUT_STRING_DIFF_WRITER_GET_PRIVATE(writer);
    g_string_append(priv->result, line);
    g_string_append_c(priv->result, '\n');
}

static void
finish (CutDiffWriter *writer)
{
    CutStringDiffWriterPrivate *priv;

    priv = CUT_STRING_DIFF_WRITER_GET_PRIVATE(writer);
    if (priv->result->str[priv->result->len - 1] == '\n')
        g_string_truncate(priv->result, priv->result->len - 1);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
