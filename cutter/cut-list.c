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
#include <string.h>

#include "cut-list.h"

typedef void (*InspectFunc) (GString *string, gconstpointer data, gpointer user_data);

static gboolean
equal_list (const GList *list1, const GList *list2,
            GCompareDataFunc compare_func, gpointer user_data)
{
    const GList *node1, *node2;

    for (node1 = list1, node2 = list2;
         node1 && node2;
         node1 = g_list_next(node1), node2 = g_list_next(node2)) {
        if (!compare_func(node1->data, node2->data, user_data))
            return FALSE;
    }

    return node1 == NULL && node2 == NULL;
}

static gchar *
inspect_list (const GList *list, InspectFunc inspect_func, gpointer user_data)
{
    const GList *node;
    GString *string;

    string = g_string_new("(");
    node = list;
    while (node) {
        inspect_func(string, node->data, user_data);
        node = g_list_next(node);
        if (node)
            g_string_append(string, ", ");
    }
    g_string_append(string, ")");

    return g_string_free(string, FALSE);
}

static gboolean
equal_int (gconstpointer data1, gconstpointer data2, gpointer user_data)
{
    return GPOINTER_TO_INT(data1) == GPOINTER_TO_INT(data2);
}

gboolean
cut_list_equal_int (const GList *list1, const GList *list2)
{
    return equal_list(list1, list2, equal_int, NULL);
}

static void
inspect_int (GString *string, gconstpointer data, gpointer user_data)
{
    g_string_append_printf(string, "%d", GPOINTER_TO_INT(data));
}

gchar *
cut_list_inspect_int (const GList *list)
{
    return inspect_list(list, inspect_int, NULL);
}

static gboolean
equal_string (gconstpointer data1, gconstpointer data2, gpointer user_data)
{
    if (data1 == NULL && data2 == NULL)
        return TRUE;

    if (data1 == NULL || data2 == NULL)
        return FALSE;

    return strcmp(data1, data2) == 0;
}

gboolean
cut_list_equal_string (const GList *list1, const GList *list2)
{
    return equal_list(list1, list2, equal_string, NULL);
}

static void
inspect_string (GString *string, gconstpointer data, gpointer user_data)
{
    const gchar *value = data;
    g_string_append_printf(string, "\"%s\"", value);
}

gchar *
cut_list_inspect_string (const GList *list)
{
    return inspect_list(list, inspect_string, NULL);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

