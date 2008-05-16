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

#ifndef __CUT_PRIVATE_H__
#define __CUT_PRIVATE_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _CutRunContext      CutRunContext;
typedef struct _CutTestRunner      CutTestRunner;
typedef struct _CutListener        CutListener;
typedef struct _CutTestSuite       CutTestSuite;
typedef struct _CutTestCase        CutTestCase;
typedef struct _CutTest            CutTest;
typedef struct _CutTestResult      CutTestResult;

G_END_DECLS

#endif /* __CUT_PRIVATE_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
