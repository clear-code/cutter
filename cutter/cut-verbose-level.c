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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-verbose-level.h"

GQuark
cut_verbose_level_error_quark (void)
{
  return g_quark_from_static_string ("cut-verbose-level-error-quark");
}

CutVerboseLevel
cut_verbose_level_parse (const gchar *name, GError **error)
{
    CutVerboseLevel level = CUT_VERBOSE_LEVEL_NORMAL;

    if (name == NULL) {
        level = CUT_VERBOSE_LEVEL_NORMAL;
    } else if (g_utf8_collate(name, "s") == 0 ||
               g_utf8_collate(name, "silent") == 0) {
        level = CUT_VERBOSE_LEVEL_SILENT;
    } else if (g_utf8_collate(name, "n") == 0 ||
               g_utf8_collate(name, "normal") == 0) {
        level = CUT_VERBOSE_LEVEL_NORMAL;
    } else if (g_utf8_collate(name, "v") == 0 ||
               g_utf8_collate(name, "verbose") == 0) {
        level = CUT_VERBOSE_LEVEL_VERBOSE;
    } else {
        if (error)
            g_set_error(error,
                        CUT_VERBOSE_LEVEL_ERROR,
                        CUT_VERBOSE_LEVEL_ERROR_BAD_VALUE,
                        "Invalid verbose level name: %s", name);
    }

    return level;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
