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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include "cut-runner.h"
#include "cut-run-context.h"

GType
cut_runner_get_type (void)
{
    static GType runner_type = 0;

    if (!runner_type)
        runner_type = g_type_register_static_simple(G_TYPE_INTERFACE,
                                                    "CutRunner",
                                                    sizeof(CutRunnerIface),
                                                    NULL, 0, NULL, 0);

    return runner_type;
}

gboolean
cut_runner_run (CutRunner *runner)
{
    gboolean success = TRUE;
    CutRunnerIface *iface;

    iface = CUT_RUNNER_GET_IFACE(runner);
    if (iface->run) {
        success = iface->run(runner);
    } /*else if (iface->run_async) {
        iface->run_async(runner);
        
        return success;
    }*/

    g_signal_emit_by_name(CUT_RUN_CONTEXT(runner), "complete-run", success);
    return success;
}

void
cut_runner_run_async (CutRunner *runner)
{
    CutRunnerIface *iface;

    iface = CUT_RUNNER_GET_IFACE(runner);
    if (iface->run_async)
        iface->run_async(runner);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
