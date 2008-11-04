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

#include <glib.h>

#include "gcut-io.h"

gchar *
gcut_io_inspect_condition (GIOCondition condition)
{
    GArray *messages;
    gchar *message;

    messages = g_array_new(TRUE, TRUE, sizeof(gchar *));
    if (condition & G_IO_IN) {
        message = "In";
        g_array_append_val(messages, message);
    }
    if (condition & G_IO_OUT) {
        message = "Out";
        g_array_append_val(messages, message);
    }
    if (condition & G_IO_PRI) {
        message = "Priority high";
        g_array_append_val(messages, message);
    }
    if (condition & G_IO_ERR) {
        message = "Error";
        g_array_append_val(messages, message);
    }
    if (condition & G_IO_HUP) {
        message = "Hung up";
        g_array_append_val(messages, message);
    }
    if (condition & G_IO_NVAL) {
        message = "Invalid request";
        g_array_append_val(messages, message);
    }
    message = g_strjoinv(" | ", (gchar **)(messages->data));
    g_array_free(messages, TRUE);

    return message;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
