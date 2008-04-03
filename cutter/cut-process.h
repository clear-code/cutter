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

#ifndef __CUT_PROCESS_H__
#define __CUT_PROCESS_H__

#include <glib-object.h>
#include "cut-test-result.h"

G_BEGIN_DECLS

#define CUT_TYPE_PROCESS            (cut_process_get_type ())
#define CUT_PROCESS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_PROCESS, CutProcess))
#define CUT_PROCESS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_PROCESS, CutProcessClass))
#define CUT_IS_PROCESS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_PROCESS))
#define CUT_IS_PROCESS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_PROCESS))
#define CUT_PROCESS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_PROCESS, CutProcessClass))

typedef struct _CutProcess      CutProcess;
typedef struct _CutProcessClass CutProcessClass;

struct _CutProcess
{
    GObject object;
};

struct _CutProcessClass
{
    GObjectClass parent_class;
};

GType cut_process_get_type  (void) G_GNUC_CONST;

CutProcess *cut_process_new (void);
int          cut_process_fork    (CutProcess *process);
int          cut_process_get_pid (CutProcess *process);
const gchar *cut_process_get_stdout_message
                                  (CutProcess *process);
const gchar *cut_process_get_stderr_message
                                  (CutProcess *process);
gboolean     cut_process_send_test_result_to_parent
                                  (CutProcess *process,
                                   CutTestResult *result);
const gchar *cut_process_get_result_from_child
                                  (CutProcess *process);
void         cut_process_exit     (CutProcess *process);
G_END_DECLS

#endif /* __CUT_PROCESS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
