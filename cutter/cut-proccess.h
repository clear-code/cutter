/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifndef __CUT_PROCCESS_H__
#define __CUT_PROCCESS_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_PROCCESS            (cut_proccess_get_type ())
#define CUT_PROCCESS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_PROCCESS, CutProccess))
#define CUT_PROCCESS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_PROCCESS, CutProccessClass))
#define CUT_IS_PROCCESS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_PROCCESS))
#define CUT_IS_PROCCESS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_PROCCESS))
#define CUT_PROCCESS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_PROCCESS, CutProccessClass))

typedef struct _CutProccess      CutProccess;
typedef struct _CutProccessClass CutProccessClass;

struct _CutProccess
{
    GObject object;
};

struct _CutProccessClass
{
    GObjectClass parent_class;
};

GType cut_proccess_get_type  (void) G_GNUC_CONST;

CutProccess *cut_proccess_new (void);
int          cut_proccess_fork    (CutProccess *proccess);
int          cut_proccess_get_pid (CutProccess *proccess);
const gchar *cut_proccess_get_stdout_message
                                  (CutProccess *proccess);
const gchar *cut_proccess_get_stderr_message
                                  (CutProccess *proccess);

G_END_DECLS

#endif /* __CUT_PROCCESS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
