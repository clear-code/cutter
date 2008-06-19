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

#ifndef __CUT_GPUBLIC_H__
#define __CUT_GPUBLIC_H__

#include <glib-object.h>

G_BEGIN_DECLS

GObject *cut_test_context_take_g_object (CutTestContext *context,
                                         GObject        *object);
const GError *cut_test_context_take_g_error (CutTestContext *context,
                                             GError         *error);

char    *cut_utils_inspect_g_error      (GError *error);

G_END_DECLS

#endif /* __CUT_GPUBLIC_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
