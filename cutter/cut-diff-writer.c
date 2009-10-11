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

#include "cut-sequence-matcher.h"
#include "cut-diff-writer.h"
#include "cut-utils.h"

G_DEFINE_TYPE(CutDiffWriter, cut_diff_writer, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

static void
cut_diff_writer_class_init (CutDiffWriterClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
}

static void
cut_diff_writer_init (CutDiffWriter *writer)
{
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_diff_writer_parent_class)->dispose(object);
}

void
cut_diff_writer_write (CutDiffWriter       *writer,
                       const gchar         *string,
                       CutDiffWriterTag     tag)
{
    CUT_DIFF_WRITER_GET_CLASS(writer)->write(writer, string, tag);
}

void
cut_diff_writer_write_segment (CutDiffWriter       *writer,
                               const gchar         *string,
                               guint                begin,
                               guint                end,
                               CutDiffWriterTag     tag)
{
    GString *segment;
    const gchar *segment_begin, *segment_end;

    segment_begin = g_utf8_offset_to_pointer(string, begin);
    segment_end = g_utf8_offset_to_pointer(string, end);
    segment = g_string_new(NULL);
    g_string_append_len(segment, segment_begin, segment_end - segment_begin);
    cut_diff_writer_write(writer, segment->str, tag);
    g_string_free(segment, TRUE);
}

void
cut_diff_writer_write_mark (CutDiffWriter       *writer,
                            const gchar         *mark,
                            CutDiffWriterTag     tag)
{
    CUT_DIFF_WRITER_GET_CLASS(writer)->write_mark(writer, mark, tag);
}

void
cut_diff_writer_write_line (CutDiffWriter       *writer,
                            const gchar         *line,
                            CutDiffWriterTag     tag)
{
    CUT_DIFF_WRITER_GET_CLASS(writer)->write_line(writer, line, tag);
}

void
cut_diff_writer_write_lines (CutDiffWriter       *writer,
                             gchar              **lines,
                             guint                begin,
                             guint                end,
                             CutDiffWriterTag     tag)
{
    guint i;

    for (i = begin; i < end; i++) {
        cut_diff_writer_write_line(writer, lines[i], tag);
    }
}

void
cut_diff_writer_mark_line (CutDiffWriter       *writer,
                           const gchar         *mark,
                           const gchar         *line,
                           CutDiffWriterTag     tag)
{
    cut_diff_writer_write_mark(writer, mark, tag);
    cut_diff_writer_write_line(writer, line, tag);
}

void
cut_diff_writer_mark_lines (CutDiffWriter       *writer,
                            const gchar         *mark,
                            gchar              **lines,
                            guint                begin,
                            guint                end,
                            CutDiffWriterTag     tag)
{
    guint i;

    for (i = begin; i < end; i++) {
        cut_diff_writer_mark_line(writer, mark, lines[i], tag);
    }
}

void
cut_diff_writer_write_character_n_times (CutDiffWriter    *writer,
                                         gchar             character,
                                         guint             n,
                                         CutDiffWriterTag  tag)
{
    GString *string;
    guint i;

    string = g_string_new(NULL);

    for (i = 0; i < n; i++) {
        g_string_append_c(string, character);
    }

    cut_diff_writer_write(writer, string->str, tag);
    g_string_free(string, TRUE);
}

void
cut_diff_writer_finish (CutDiffWriter *writer)
{
    CUT_DIFF_WRITER_GET_CLASS(writer)->finish(writer);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
