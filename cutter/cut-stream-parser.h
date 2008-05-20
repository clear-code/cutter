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

#ifndef __CUT_STREAM_PARSER_H__
#define __CUT_STREAM_PARSER_H__

#include <glib.h>

#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>

G_BEGIN_DECLS

#define CUT_TYPE_STREAM_PARSER            (cut_stream_parser_get_type ())
#define CUT_STREAM_PARSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_STREAM_PARSER, CutStreamParser))
#define CUT_STREAM_PARSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_STREAM_PARSER, CutStreamParserClass))
#define CUT_IS_STREAM_PARSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_STREAM_PARSER))
#define CUT_IS_STREAM_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_STREAM_PARSER))
#define CUT_STREAM_PARSER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_STREAM_PARSER, CutStreamParserClass))

typedef struct _CutStreamParser         CutStreamParser;
typedef struct _CutStreamParserClass    CutStreamParserClass;

struct _CutStreamParser
{
    GObject object;
};

struct _CutStreamParserClass
{
    GObjectClass parent_class;

    void (*result)         (CutStreamParser *stream_parser,
                            CutTestResult   *result);
};

GType            cut_stream_parser_get_type  (void) G_GNUC_CONST;

CutStreamParser *cut_stream_parser_new       (CutRunContext   *run_context);
CutStreamParser *cut_test_result_parser_new  (void);

gboolean         cut_stream_parser_parse     (CutStreamParser *parser,
                                              const gchar     *text,
                                              gsize            text_len,
                                              GError         **error);
gboolean         cut_stream_parser_end_parse (CutStreamParser *stream_parser,
                                              GError         **error);

G_END_DECLS

#endif /* __CUT_STREAM_PARSER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
