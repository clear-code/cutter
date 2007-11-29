/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef __CUT_HIDDEN_DEFINITIONS_H__
#define __CUT_HIDDEN_DEFINITIONS_H__

#include <cutter/cut-public.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_current_test_context(CutTestContext *context);
CutTestContext *get_current_test_context(void);

static CutTestContext *_current_test_context = NULL;

void
set_current_test_context (CutTestContext *context)
{
    _current_test_context = context;
}

CutTestContext *
get_current_test_context (void)
{
    return _current_test_context;
}

#ifdef __cplusplus
}
#endif

#endif /* __CUTTER_HIDDEN_DEFINITIONS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
