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

#ifndef __CUT_STRING_DIFF_WRITER_H__
#define __CUT_STRING_DIFF_WRITER_H__

#include <glib-object.h>
#include <cutter/cut-diff-writer.h>

G_BEGIN_DECLS

#define CUT_TYPE_STRING_DIFF_WRITER            (cut_string_diff_writer_get_type ())
#define CUT_STRING_DIFF_WRITER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_STRING_DIFF_WRITER, CutDiffWriter))
#define CUT_STRING_DIFF_WRITER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_STRING_DIFF_WRITER, CutDiffWriterClass))
#define CUT_IS_STRING_DIFF_WRITER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_STRING_DIFF_WRITER))
#define CUT_IS_STRING_DIFF_WRITER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_STRING_DIFF_WRITER))
#define CUT_STRING_DIFF_WRITER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_STRING_DIFF_WRITER, CutDiffWriterClass))

typedef struct _CutStringDiffWriter         CutStringDiffWriter;
typedef struct _CutStringDiffWriterClass    CutStringDiffWriterClass;

struct _CutStringDiffWriter
{
    CutDiffWriter object;
};

struct _CutStringDiffWriterClass
{
    CutDiffWriterClass parent_class;
};

GType          cut_string_diff_writer_get_type         (void) G_GNUC_CONST;

CutDiffWriter *cut_string_diff_writer_new              (void);
const gchar   *cut_string_diff_writer_get_result       (CutDiffWriter *writer);

G_END_DECLS

#endif /* __CUT_STRING_DIFF_WRITER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
