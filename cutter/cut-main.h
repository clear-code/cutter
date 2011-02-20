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

#ifndef __CUT_MAIN_H__
#define __CUT_MAIN_H__

#include <cutter/cut-version.h>

#include <cutter/cut-test-runner.h>
#include <cutter/cut-test-suite.h>

G_BEGIN_DECLS

void          cut_init_environment   (int *argc, char ***argv);
void          cut_quit_environment   (void);
void          cut_init               (int *argc, char ***argv);
void          cut_quit               (void);

gboolean      cut_run                (void);
gboolean      cut_run_iteration      (void);

void          cut_set_cutter_command_path (const char *argv0);
const char   *cut_get_cutter_command_path (void);

CutRunContext *cut_create_run_context(void);
void           cut_setup_run_context (void);
gboolean       cut_start_run_context (CutRunContext *run_context);

G_END_DECLS

#endif /* __CUT_MAIN_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
