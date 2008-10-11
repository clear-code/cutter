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

#ifndef __CUTTEST_UTILS_H__
#define __CUTTEST_UTILS_H__

#include <glib.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>

#define ISO8601_PATTERN_WITHOUT_YEAR                    \
    "\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}(?:\\.\\d+)?Z"
#define ISO8601_PATTERN "\\d{4}-" ISO8601_PATTERN_WITHOUT_YEAR

#define CUTTEST_TEST_DIR_KEY "CUTTEST_TEST_DIR"

const gchar *cuttest_get_base_dir (void);
void         cuttest_add_test     (CutTestCase *test_case,
                                   const gchar *test_name,
                                   CutTestFunction test_function);

#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
