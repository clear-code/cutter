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

#ifndef __CUT_FILE_STREAM_READER_H__
#define __CUT_FILE_STREAM_READER_H__

#include <glib-object.h>

#include <cutter/cut-stream-reader.h>

G_BEGIN_DECLS

#define CUT_TYPE_FILE_STREAM_READER            (cut_file_stream_reader_get_type ())
#define CUT_FILE_STREAM_READER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_FILE_STREAM_READER, CutFileStreamReader))
#define CUT_FILE_STREAM_READER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_FILE_STREAM_READER, CutFileStreamReaderClass))
#define CUT_IS_FILE_STREAM_READER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_FILE_STREAM_READER))
#define CUT_IS_FILE_STREAM_READER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_FILE_STREAM_READER))
#define CUT_FILE_STREAM_READER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_FILE_STREAM_READER, CutFileStreamReaderClass))

typedef struct _CutFileStreamReader      CutFileStreamReader;
typedef struct _CutFileStreamReaderClass CutFileStreamReaderClass;

struct _CutFileStreamReader
{
    CutStreamReader object;
};

struct _CutFileStreamReaderClass
{
    CutStreamReaderClass parent_class;
};

GType          cut_file_stream_reader_get_type  (void) G_GNUC_CONST;

CutRunContext *cut_file_stream_reader_new       (const gchar *file_name);

G_END_DECLS

#endif /* __CUT_FILE_STREAM_READER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
