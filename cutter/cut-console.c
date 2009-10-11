/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-console.h"

gboolean
cut_console_guess_color_usability (void)
{
    const gchar *term, *emacs;

    term = g_getenv("TERM");
    if (term && (g_str_has_suffix(term, "term") ||
                 g_str_has_suffix(term, "term-color") ||
                 g_str_equal(term, "screen") ||
                 g_str_equal(term, "linux")))
        return TRUE;

    emacs = g_getenv("EMACS");
    if (emacs && (g_str_equal(emacs, "t")))
        return TRUE;

    return FALSE;
}

gboolean
cut_console_parse_color_arg (const gchar *option_name, const gchar *value,
                             gboolean *use_color, GError **error)
{
    if (value == NULL ||
        g_utf8_collate(value, "yes") == 0 ||
        g_utf8_collate(value, "true") == 0) {
        *use_color = TRUE;
    } else if (g_utf8_collate(value, "no") == 0 ||
               g_utf8_collate(value, "false") == 0) {
        *use_color = FALSE;
    } else if (g_utf8_collate(value, "auto") == 0) {
        *use_color = cut_console_guess_color_usability();
    } else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_BAD_VALUE,
                    _("Invalid color value: %s"), value);
        return FALSE;
    }

    return TRUE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
