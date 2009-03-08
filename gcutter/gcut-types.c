/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifdef HAVE_CONFIG_H
#  include "../config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include "gcut-types.h"

GType
gcut_error_get_type (void)
{
    static GType type_id = 0;
    if (!type_id)
        type_id = g_boxed_type_register_static(g_intern_static_string("GError"),
                                               (GBoxedCopyFunc)g_error_copy,
                                               (GBoxedFreeFunc)g_error_free);
    return type_id;
}

/*
vi:nowrap:ai:expandtab:sw=4
*/

