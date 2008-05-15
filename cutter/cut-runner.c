/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#include "cut-repository.h"
#include "cut-test-case.h"
#include "cut-test-result.h"

#include "cut-marshalers.h"
#include "cut-enum-types.h"

G_DEFINE_TYPE (CutRunner, cut_runner, CUT_TYPE_RUN_CONTEXT)

static void
cut_runner_class_init (CutRunnerClass *klass)
{
}

static void
cut_runner_init (CutRunner *runner)
{
}

CutRunner *
cut_runner_new (void)
{
    return g_object_new(CUT_TYPE_RUNNER, NULL);
}

gboolean
cut_runner_run (CutRunner *runner)
{
    CutRunContext *context;
    CutTestSuite *suite;
    gboolean success;

    context = CUT_RUN_CONTEXT(runner);
    suite = cut_run_context_get_test_suite(context);
    cut_run_context_attach_listeners(context);
    success = cut_test_suite_run(suite, context);
    cut_run_context_detach_listeners(context);

    return success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
