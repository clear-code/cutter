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

#ifndef __CUT_HIDDEN_DEFINITIONS_H__
#define __CUT_HIDDEN_DEFINITIONS_H__

#include <stdlib.h>
#include <cutter/cut-public.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_current_test_context(CutTestContext *context);
CutTestContext *get_current_test_context(void);

static CutTestContextKey _current_test_context_key;

void
set_current_test_context (CutTestContext *context)
{
    cut_test_context_set_current(&_current_test_context_key, context);
}

CutTestContext *
get_current_test_context (void)
{
    return cut_test_context_get_current(&_current_test_context_key);
}

#ifdef __cplusplus
}
#endif

#endif /* __CUTTER_HIDDEN_DEFINITIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
