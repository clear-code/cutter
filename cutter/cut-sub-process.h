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

#ifndef __CUT_SUB_PROCESS_H__
#define __CUT_SUB_PROCESS_H__

#include <glib-object.h>
#include <cutter/cut-public.h>
#include <cutter/cut-run-context.h>

G_BEGIN_DECLS

#define CUT_TYPE_SUB_PROCESS            (cut_sub_process_get_type ())
#define CUT_SUB_PROCESS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_SUB_PROCESS, CutSubProcess))
#define CUT_SUB_PROCESS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_SUB_PROCESS, CutSubProcessClass))
#define CUT_IS_SUB_PROCESS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_SUB_PROCESS))
#define CUT_IS_SUB_PROCESS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_SUB_PROCESS))
#define CUT_SUB_PROCESS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_SUB_PROCESS, CutSubProcessClass))

typedef struct _CutSubProcessClass    CutSubProcessClass;

struct _CutSubProcess
{
    GObject object;
};

struct _CutSubProcessClass
{
    GObjectClass parent_class;
};

GType           cut_sub_process_get_type         (void) G_GNUC_CONST;

CutSubProcess  *cut_sub_process_new              (const char     *test_directory,
                                                  CutTestContext *test_context);

CutRunContext  *cut_sub_process_get_pipeline     (CutSubProcess  *sub_process);
void            cut_sub_process_set_pipeline     (CutSubProcess  *sub_process,
                                                  CutRunContext  *run_context);

CutTestContext *cut_sub_process_get_test_context (CutSubProcess  *sub_process);
void            cut_sub_process_set_test_context (CutSubProcess  *sub_process,
                                                  CutTestContext *test_context);


G_END_DECLS

#endif /* __CUT_SUB_PROCESS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
