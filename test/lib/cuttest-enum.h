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

#ifndef __CUTTEST_ENUM_H__
#define __CUTTEST_ENUM_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum
{
    CUTTEST_FLAG_FIRST	= (1 << 0),
    CUTTEST_FLAG_SECOND	= (1 << 1),
    CUTTEST_FLAG_THIRD	= (1 << 2)
} CuttestFlags;

GType cuttest_flags_get_type (void);
#define CUTTEST_FLAGS (cuttest_flags_get_type())

G_END_DECLS

#endif

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
