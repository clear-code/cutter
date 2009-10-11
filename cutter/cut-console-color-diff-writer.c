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

#include "cut-console-color-diff-writer.h"
#include "cut-console-colors.h"

G_DEFINE_TYPE(CutConsoleColorDiffWriter, cut_console_color_diff_writer, CUT_TYPE_DIFF_WRITER)

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
cut_console_color_diff_writer_class_init (CutConsoleColorDiffWriterClass *klass)
{
    CutDiffWriterClass *diff_writer_class;

    diff_writer_class = CUT_DIFF_WRITER_CLASS(klass);

    diff_writer_class->write = write;
    diff_writer_class->write_mark = write_mark;
    diff_writer_class->write_line = write_line;
    diff_writer_class->finish = finish;
}

static void
cut_console_color_diff_writer_init (CutConsoleColorDiffWriter *writer)
{
}

CutDiffWriter *
cut_console_color_diff_writer_new (void)
{
    return g_object_new(CUT_TYPE_CONSOLE_COLOR_DIFF_WRITER, NULL);
}

static const gchar *
tag_to_color(CutDiffWriterTag tag)
{
    const gchar *color;

    switch (tag) {
    case CUT_DIFF_WRITER_TAG_DELETED_MARK:
        color = CUT_CONSOLE_COLOR_GREEN;
        break;
    case CUT_DIFF_WRITER_TAG_INSERTED_MARK:
        color = CUT_CONSOLE_COLOR_RED;
        break;
    case CUT_DIFF_WRITER_TAG_DIFFERENCE_MARK:
        color = CUT_CONSOLE_COLOR_CYAN;
        break;
    case CUT_DIFF_WRITER_TAG_DELETED:
        color = CUT_CONSOLE_COLOR_WHITE CUT_CONSOLE_COLOR_RED_BACK;
        break;
    case CUT_DIFF_WRITER_TAG_INSERTED:
        color = CUT_CONSOLE_COLOR_WHITE CUT_CONSOLE_COLOR_GREEN_BACK;
        break;
    default:
        color = "";
        break;
    }

    return color;
}

static void
write (CutDiffWriter *writer, const gchar *string, CutDiffWriterTag tag)
{
    const gchar *color;

    color = tag_to_color(tag);
    if (color[0])
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
