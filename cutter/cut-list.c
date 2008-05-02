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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include "cut-list.h"

gboolean
cut_list_equal_int (const GList *list1, const GList *list2)
{
    const GList *node1, *node2;

    for (node1 = list1, node2 = list2;
         node1 && node2;
         node1 = g_list_next(node1), node2 = g_list_next(node2)) {
        if (GPOINTER_TO_INT(node1->data) != GPOINTER_TO_INT(node2->data))
            return FALSE;
    }

    return node1 == NULL && node2 == NULL;
}

gchar *
cut_list_inspect_int (const GList *list)
{
    const GList *node;
    GString *string;

    string = g_string_new("(");
    node = list;
    while (node) {
        g_string_append_printf(string, "%d", GPOINTER_TO_INT(node->data));
        node = g_list_next(node);
        if (node)
            g_string_append(string, ", ");
    }
    g_string_append(string, ")");

    return g_string_free(string, FALSE);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

