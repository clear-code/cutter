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

#include "cut-utils.h"
#include "cut-test-utils.h"

#include <gcutter/gcut-assertions.h>

const gchar *
cut_take_replace_helper (const gchar *target, const gchar *pattern,
                         const gchar *replacement, CutCallbackFunction callback)
{
    gchar *replaced;
    GError *error = NULL;

    replaced = cut_utils_regex_replace(pattern, target, replacement, &error);
    gcut_assert_error_helper(error, "error");
    if (callback)
        callback();
    return cut_take_string(replaced);
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
