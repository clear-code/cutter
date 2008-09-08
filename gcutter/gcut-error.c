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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gcut-error.h"

gboolean
gcut_error_equal (const GError *error1, const GError *error2)
{
    if (error1 == error2)
        return TRUE;

    if (error1 == NULL || error2 == NULL)
        return FALSE;

    if (error1->domain != error2->domain)
        return FALSE;

    if (error1->code != error2->code)
        return FALSE;

    if (error1->message == NULL && error2->message == NULL)
        return TRUE;

    if (error1->message == NULL || error2->message == NULL)
        return FALSE;

    return g_str_equal(error1->message, error2->message);
}

gchar *
gcut_error_inspect (const GError *error)
{
    GString *inspected;

    if (!error)
        return g_strdup("No error");

    inspected = g_string_new(g_quark_to_string(error->domain));
    g_string_append_printf(inspected, ":%d", error->code);
    if (error->message) {
        g_string_append(inspected, ": ");
        g_string_append(inspected, error->message);
    }

    return g_string_free(inspected, FALSE);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
