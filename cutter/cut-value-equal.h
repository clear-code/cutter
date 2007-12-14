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

#ifndef __CUT_VALUE_EQUAL_H__
#define __CUT_VALUE_EQUAL_H__

#include <glib-object.h>

G_BEGIN_DECLS

void             cut_value_equal_init          (void);

gboolean         cut_value_equal               (GValue *value1,
                                                GValue *value2);

void             cut_value_register_equal_func (GType type1,
                                                GType type2,
                                                GEqualFunc equal_func);


G_END_DECLS

#endif /* __CUT_VALUE_EQUAL_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
