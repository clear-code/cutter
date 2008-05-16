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

#ifndef __CUT_RUNNER_H__
#define __CUT_RUNNER_H__

#include <glib-object.h>
#include <cutter/cut-private.h>

G_BEGIN_DECLS

#define CUT_TYPE_RUNNER            (cut_runner_get_type ())
#define CUT_RUNNER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUNNER, CutRunner))
#define CUT_RUNNER_CLASS(obj)      (G_TYPE_CHECK_CLASS_CAST ((obj), CUT_TYPE_RUNNER, CutRunnerIface))
#define CUT_IS_RUNNER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUNNER))
#define CUT_RUNNER_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CUT_TYPE_RUNNER, CutRunnerIface))

typedef struct _CutRunner      CutRunner; /* Dummy typedef */
typedef struct _CutRunnerIface CutRunnerIface;

struct _CutRunnerIface
{
    GTypeInterface base_iface;

    gboolean (*run)       (CutRunner   *runner);
    void     (*run_async) (CutRunner   *runner);
};

GType           cut_runner_get_type  (void) G_GNUC_CONST;

gboolean        cut_runner_run       (CutRunner *runner);
void            cut_runner_run_async (CutRunner *runner);

G_END_DECLS

#endif /* __CUT_RUNNER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
