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

#include "cut-test-runner.h"
#include "cut-runner.h"
#include "cut-repository.h"
#include "cut-test-case.h"
#include "cut-test-result.h"

#include "cut-marshalers.h"
#include "cut-enum-types.h"

static void runner_init (CutRunnerIface *iface);

static CutRunnerIface *parent_runner_iface;

G_DEFINE_TYPE_WITH_CODE(CutTestRunner, cut_test_runner, CUT_TYPE_RUN_CONTEXT,
                        G_IMPLEMENT_INTERFACE(CUT_TYPE_RUNNER, runner_init))

static gboolean runner_run (CutRunner *runner);

static void
cut_test_runner_class_init (CutTestRunnerClass *klass)
{
}

static void
cut_test_runner_init (CutTestRunner *runner)
{
}

static void
runner_init (CutRunnerIface *iface)
{
    parent_runner_iface = g_type_interface_peek_parent(iface);
    iface->run = runner_run;
}

CutRunContext *
cut_test_runner_new (void)
{
    return g_object_new(CUT_TYPE_TEST_RUNNER, NULL);
}

static gboolean
runner_run (CutRunner *runner)
{
    CutRunContext *context;
    CutTestSuite *suite;

    context = CUT_RUN_CONTEXT(runner);
    suite = cut_run_context_get_test_suite(context);
    return cut_test_suite_run(suite, context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
