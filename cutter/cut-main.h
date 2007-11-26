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

#ifndef __CUT_MAIN_H__
#define __CUT_MAIN_H__

#include <cutter/cut-context.h>
#include <cutter/cut-test-suite.h>

G_BEGIN_DECLS

void          cut_init              (int *argc, char ***argv);
void          cut_quit              (void);

gboolean      cut_run               (const gchar *directory);

CutContext   *cut_create_context    (void);
CutTestSuite *cut_create_test_suite (const gchar *directory);
gboolean      cut_run_test_suite    (CutTestSuite *suite,
                                     CutContext   *context);

G_END_DECLS

#endif /* __CUT_MAIN_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
