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

#ifndef __CUT_DIFF_WRITER_H__
#define __CUT_DIFF_WRITER_H__

#include <glib-object.h>
#include <cutter/cut-public.h>

G_BEGIN_DECLS

#define CUT_TYPE_DIFF_WRITER            (cut_diff_writer_get_type ())
#define CUT_DIFF_WRITER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_DIFF_WRITER, CutDiffWriter))
#define CUT_DIFF_WRITER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_DIFF_WRITER, CutDiffWriterClass))
#define CUT_IS_DIFF_WRITER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_DIFF_WRITER))
#define CUT_IS_DIFF_WRITER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_DIFF_WRITER))
#define CUT_DIFF_WRITER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_DIFF_WRITER, CutDiffWriterClass))

typedef struct _CutDiffWriter         CutDiffWriter;
typedef struct _CutDiffWriterClass    CutDiffWriterClass;

typedef enum
{
    CUT_DIFF_WRITER_TAG_NONE,
    CUT_DIFF_WRITER_TAG_EQUAL_MARK,
    CUT_DIFF_WRITER_TAG_DELETED_MARK,
    CUT_DIFF_WRITER_TAG_INSERTED_MARK,
    CUT_DIFF_WRITER_TAG_DIFFERENCE_MARK,
    CUT_DIFF_WRITER_TAG_EQUAL,
    CUT_DIFF_WRITER_TAG_DELETED,
    CUT_DIFF_WRITER_TAG_INSERTED,
    CUT_DIFF_WRITER_TAG_DIFFERENCE
} CutDiffWriterTag;

struct _CutDiffWriter
{
    GObject object;
};

struct _CutDiffWriterClass
{
    GObjectClass parent_class;

    void (*write)      (CutDiffWriter     *writer,
                        const gchar       *string,
                        CutDiffWriterTag   tag);
    void (*write_line) (CutDiffWriter     *writer,
                        const gchar       *line,
                        CutDiffWriterTag   tag);
    void (*finish)     (CutDiffWriter     *writer);
};

GType         cut_diff_writer_get_type         (void) G_GNUC_CONST;

void          cut_diff_writer_write            (CutDiffWriter      *writer,
                                                const gchar        *string,
                                                CutDiffWriterTag    tag);
void          cut_diff_writer_write_segment    (CutDiffWriter      *writer,
                                                const gchar        *string,
                                                guint               begin,
                                                guint               end,
                                                CutDiffWriterTag    tag);
void          cut_diff_writer_write_mark       (CutDiffWriter      *writer,
                                                const gchar        *mark,
                                                const gchar        *separator,
                                                CutDiffWriterTag    tag);
void          cut_diff_writer_write_line       (CutDiffWriter      *writer,
                                                const gchar        *line,
                                                CutDiffWriterTag    tag);
void          cut_diff_writer_write_lines      (CutDiffWriter      *writer,
                                                gchar             **lines,
                                                guint               begin,
                                                guint               end,
                                                CutDiffWriterTag    tag);
void          cut_diff_writer_mark_line        (CutDiffWriter      *writer,
                                                const gchar        *mark,
                                                const gchar        *separator,
                                                const gchar        *line,
                                                CutDiffWriterTag    tag);
void          cut_diff_writer_mark_lines       (CutDiffWriter      *writer,
                                                const gchar        *mark,
                                                const gchar        *separator,
                                                gchar             **lines,
                                                guint               begin,
                                                guint               end,
                                                CutDiffWriterTag    tag);

void          cut_diff_writer_write_character_n_times
                                               (CutDiffWriter      *writer,
                                                gchar               character,
                                                guint               n,
                                                CutDiffWriterTag    tag);

void          cut_diff_writer_finish           (CutDiffWriter      *writer);


G_END_DECLS

#endif /* __CUT_DIFF_WRITER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
