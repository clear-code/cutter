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

#ifndef __CUT_PIPELINE_H__
#define __CUT_PIPELINE_H__

#include <glib-object.h>

#include <cutter/cut-run-context.h>

G_BEGIN_DECLS

#define CUT_TYPE_PIPELINE            (cut_pipeline_get_type ())
#define CUT_PIPELINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_PIPELINE, CutPipeline))
#define CUT_PIPELINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_PIPELINE, CutPipelineClass))
#define CUT_IS_PIPELINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_PIPELINE))
#define CUT_IS_PIPELINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_PIPELINE))
#define CUT_PIPELINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_PIPELINE, CutPipelineClass))

typedef struct _CutPipeline      CutPipeline;
typedef struct _CutPipelineClass CutPipelineClass;

struct _CutPipeline
{
    CutRunContext object;
};

struct _CutPipelineClass
{
    CutRunContextClass parent_class;
};

GType          cut_pipeline_get_type  (void) G_GNUC_CONST;

CutRunContext *cut_pipeline_new       (void);

G_END_DECLS

#endif /* __CUT_PIPELINE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
