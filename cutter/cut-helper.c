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
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "cut-helper.h"
#include "cut-run-context.h"

void
cut_pop_backtrace (void)
{
    cut_test_context_pop_backtrace(cut_get_current_test_context());
}

const char *
cut_get_test_directory (void)
{
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestCase *test_case;

    test_context = cut_get_current_test_context();
    if (!test_context)
        return NULL;

    test_case = cut_test_context_get_test_case(test_context);
    if (!test_case)
        return NULL;

    run_context = cut_test_context_get_run_context(test_context);

    return cut_build_path(cut_run_context_get_test_directory(run_context),
                          cut_test_get_base_directory(CUT_TEST(test_case)),
                          NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
