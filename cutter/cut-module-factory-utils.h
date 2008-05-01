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

#ifndef __CUT_MODULE_FACTORY_UTILS_H__
#define __CUT_MODULE_FACTORY_UTILS_H__

#include <glib-object.h>

#include <cutter/cut-module.h>

G_BEGIN_DECLS

void         cut_module_factory_init        (void);
void         cut_module_factory_quit        (void);

const gchar *cut_module_factory_get_default_module_dir   (void);
void         cut_module_factory_set_default_module_dir   (const gchar *dir);

void         cut_module_factory_load        (const gchar *dir,
                                             const gchar *type);
void         cut_module_factory_load_all    (const gchar *base_dir);
void         cut_module_factory_unload      (void);
GList       *cut_module_factory_get_names   (const gchar *type);
gboolean     cut_module_factory_exist_module(const gchar *type,
                                             const gchar *name);
CutModule   *cut_module_factory_load_module (const gchar *type,
                                             const gchar *name);

G_END_DECLS

#endif /* __CUT_MODULE_FACTORY_UTILS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
