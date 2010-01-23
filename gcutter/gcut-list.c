/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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
#include <string.h>

#include "gcut-list.h"
#include "gcut-object.h"
#include "gcut-enum.h"
#include "gcut-test-utils.h"

GList *
gcut_list_new (gpointer element, ...)
{
    GList *list = NULL;
    va_list args;

    va_start(args, element);
    while (element) {
        list = g_list_prepend(list, element);
        element = va_arg(args, gpointer);
    }
    va_end(args);

    return g_list_reverse(list);
}

gboolean
gcut_list_equal (const GList *list1, const GList *list2, GEqualFunc equal_func)
{
    const GList *node1, *node2;

    for (node1 = list1, node2 = list2;
         node1 && node2;
         node1 = g_list_next(node1), node2 = g_list_next(node2)) {
        if (!equal_func(node1->data, node2->data))
            return FALSE;
    }

    return node1 == NULL && node2 == NULL;
}

gchar *
gcut_list_inspect (const GList *list, GCutInspectFunction inspect_func,
                   gpointer user_data)
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
equal_int (gconstpointer data1, gconstpointer data2)
{
    return GPOINTER_TO_INT(data1) == GPOINTER_TO_INT(data2);
}

gboolean
gcut_list_equal_int (const GList *list1, const GList *list2)
{
    return gcut_list_equal(list1, list2, equal_int);
}

static void
inspect_int (GString *string, gconstpointer data, gpointer user_data)
{
    g_string_append_printf(string, "%d", GPOINTER_TO_INT(data));
}

gchar *
gcut_list_inspect_int (const GList *list)
{
    return gcut_list_inspect(list, inspect_int, NULL);
}

static gboolean
equal_uint (gconstpointer data1, gconstpointer data2)
{
    return GPOINTER_TO_UINT(data1) == GPOINTER_TO_UINT(data2);
}

gboolean
gcut_list_equal_uint (const GList *list1, const GList *list2)
{
    return gcut_list_equal(list1, list2, equal_uint);
}

static void
inspect_uint (GString *string, gconstpointer data, gpointer user_data)
{
    g_string_append_printf(string, "%u", GPOINTER_TO_UINT(data));
}

gchar *
gcut_list_inspect_uint (const GList *list)
{
    return gcut_list_inspect(list, inspect_uint, NULL);
}

static gboolean
equal_string (gconstpointer data1, gconstpointer data2)
{
    if (data1 == NULL && data2 == NULL)
        return TRUE;

    if (data1 == NULL || data2 == NULL)
        return FALSE;

    return strcmp(data1, data2) == 0;
}

gboolean
gcut_list_equal_string (const GList *list1, const GList *list2)
{
    return gcut_list_equal(list1, list2, equal_string);
}

static void
inspect_string (GString *string, gconstpointer data, gpointer user_data)
{
    const gchar *value = data;

    if (value)
        g_string_append_printf(string, "\"%s\"", value);
    else
        g_string_append(string, "NULL");
}

gchar *
gcut_list_inspect_string (const GList *list)
{
    return gcut_list_inspect(list, inspect_string, NULL);
}

GList *
gcut_list_string_new (const gchar *value, ...)
{
    GList *list = NULL;
    va_list args;

    va_start(args, value);
    while (value) {
        list = g_list_prepend(list, g_strdup(value));
        value = va_arg(args, const gchar *);
    }
    va_end(args);

    return g_list_reverse(list);
}

GList *
gcut_list_string_new_array (const gchar **strings)
{
    GList *list = NULL;

    if (!strings)
        return NULL;

    for (; *strings; strings++) {
        list = g_list_prepend(list, g_strdup(*strings));
    }

    return g_list_reverse(list);
}

void
gcut_list_string_free (GList *list)
{
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);
}

static void
object_unref_safe (gpointer object, gpointer user_data)
{
    if (object)
        g_object_unref(object);
}

void
gcut_list_object_free (GList *list)
{
    g_list_foreach(list, object_unref_safe, NULL);
    g_list_free(list);
}

static void
inspect_object (GString *string, gconstpointer data, gpointer user_data)
{
    gchar *inspected_object;

    inspected_object = gcut_object_inspect(data);
    g_string_append(string, inspected_object);
    g_free(inspected_object);
}

gchar *
gcut_list_inspect_object (const GList *list)
{
    return gcut_list_inspect(list, inspect_object, NULL);
}

static void
inspect_enum (GString *string, gconstpointer data, gpointer user_data)
{
    gchar *inspected_enum;
    GType type = GPOINTER_TO_UINT(user_data);

    inspected_enum = gcut_enum_inspect(type, GPOINTER_TO_INT(data));
    g_string_append(string, inspected_enum);
    g_free(inspected_enum);
}

gchar *
gcut_list_inspect_enum (GType type, const GList *list)
{
    return gcut_list_inspect(list, inspect_enum, GUINT_TO_POINTER(type));
}

static void
inspect_flags (GString *string, gconstpointer data, gpointer user_data)
{
    gchar *inspected_flags;
    GType type = GPOINTER_TO_UINT(user_data);

    inspected_flags = gcut_flags_inspect(type, GPOINTER_TO_UINT(data));
    g_string_append(string, inspected_flags);
    g_free(inspected_flags);
}

gchar *
gcut_list_inspect_flags (GType type, const GList *list)
{
    return gcut_list_inspect(list, inspect_flags, GUINT_TO_POINTER(type));
}


/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
