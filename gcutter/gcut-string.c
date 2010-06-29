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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <cutter/cut-public.h>
#include "gcut-string.h"

gboolean
gcut_string_equal (const GString *string1, const GString *string2)
{
    if (string1 == string2)
        return TRUE;

    if (string1 == NULL || string2 == NULL)
        return FALSE;

    if (string1->len != string2->len)
        return FALSE;

    return (memcmp(string1->str, string2->str, string1->len) == 0);
}

gchar *
gcut_string_inspect (const GString *string)
{
    if (!string)
        return g_strdup("No data");

    return cut_utils_inspect_memory(string->str, string->len);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
