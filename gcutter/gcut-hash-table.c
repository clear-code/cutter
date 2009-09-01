/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@clear-code.com>
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

#include "gcut-hash-table.h"
#include "gcut-test-utils.h"
#include "gcut-inspect.h"

typedef struct _EqualData
{
    GHashTable *target;
    GEqualFunc equal_func;
    gboolean equal;
} EqualData;

static void
equal_hash_table_pair (gpointer key, gpointer value, gpointer user_data)
{
    EqualData *data = user_data;
    gpointer target_key, target_value;

    if (!g_hash_table_lookup_extended((data->target), key,
                                      &target_key, &target_value)) {
        data->equal = FALSE;
        return;
    }

    if (!data->equal_func(value, target_value)) {
        data->equal = FALSE;
        return;
    }
}

gboolean
gcut_hash_table_equal (GHashTable *hash1, GHashTable *hash2,
                       GEqualFunc equal_func)
{
    EqualData data;

    if (hash1 == NULL && hash2 == NULL)
        return TRUE;

    if (hash1 == NULL || hash2 == NULL)
        return FALSE;

    if (g_hash_table_size(hash1) != g_hash_table_size(hash2))
        return FALSE;

    data.target = hash2;
    data.equal_func = equal_func;
    data.equal = TRUE;
    g_hash_table_foreach(hash1, equal_hash_table_pair, &data);

    return data.equal;
}

typedef struct _InspectData
{
    GString *output;
    GCutInspectFunction key_inspect_func;
    GCutInspectFunction value_inspect_func;
    gpointer user_data;
    guint nth_pair;
    guint total_size;
} InspectData;

static void
inspect_hash_table_pair (gpointer key, gpointer value, InspectData *data)
{
    data->nth_pair++;
    data->key_inspect_func(data->output, key, data->user_data);
    g_string_append(data->output, " => ");
    data->value_inspect_func(data->output, value, data->user_data);
    if (data->nth_pair != data->total_size)
        g_string_append(data->output, ", ");
}

gchar *
gcut_hash_table_inspect (GHashTable *hash,
                         GCutInspectFunction key_inspect_func,
                         GCutInspectFunction value_inspect_func,
                         gpointer user_data)
{
    return gcut_hash_table_inspect_sorted(hash,
                                          key_inspect_func, value_inspect_func,
                                          NULL, user_data);
}

gchar *
gcut_hash_table_inspect_sorted (GHashTable *hash,
                                GCutInspectFunction key_inspect_func,
                                GCutInspectFunction value_inspect_func,
                                GCompareFunc compare_func,
                                gpointer user_data)
{
    GString *string;
    GList *keys, *sorted_keys;
    InspectData data;

    if (!hash)
        return g_strdup("(null)");

    string = g_string_new("{");

    data.output = string;
    data.key_inspect_func = key_inspect_func;
    data.value_inspect_func = value_inspect_func;
    data.user_data = user_data;
    data.nth_pair = 0;
    data.total_size = g_hash_table_size(hash);
    keys = g_hash_table_get_keys(hash);
    if (compare_func) {
        sorted_keys = g_list_sort(keys, compare_func);
    } else {
        sorted_keys = keys;
    }
    for (; sorted_keys; sorted_keys = g_list_next(sorted_keys)) {
        gpointer key = sorted_keys->data;
        inspect_hash_table_pair(key, g_hash_table_lookup(hash, key), &data);
    }
    g_list_free(keys);

    g_string_append(string, "}");

    return g_string_free(string, FALSE);
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
gcut_hash_table_string_equal (GHashTable *hash1, GHashTable *hash2)
{
    return gcut_hash_table_equal(hash1, hash2, equal_string);
}

static int
compare_string (gconstpointer data1, gconstpointer data2)
{
    if (data1 == NULL && data2 == NULL)
        return 0;

    if (data1 == NULL)
        return -1;
    if (data2 == NULL)
        return 1;

    return strcmp(data1, data2);
}

gchar *
gcut_hash_table_string_string_inspect (GHashTable *hash)
{
    return gcut_hash_table_inspect_sorted(hash,
                                          gcut_inspect_string,
                                          gcut_inspect_string,
                                          compare_string,
                                          NULL);
}

static void
gcut_hash_table_copy_entry (gpointer key,
                            gpointer value,
                            gpointer user_data)
{
    GHashTable *hash;
    hash = user_data;

    g_hash_table_insert(hash, g_strdup(key), g_strdup(value));
}

GHashTable *
gcut_hash_table_string_string_copy (GHashTable *hash)
{
    GHashTable *copied_hash;
    copied_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_foreach(hash, gcut_hash_table_copy_entry, copied_hash);

    return copied_hash;
}

GHashTable *
gcut_hash_table_string_string_new (const gchar *key, ...)
{
    GHashTable *table;
    va_list args;

    va_start(args, key);
    table = gcut_hash_table_string_string_new_va_list(key, args);
    va_end(args);

    return table;
}

GHashTable *
gcut_hash_table_string_string_new_va_list (const gchar *key, va_list args)
{
    GHashTable *table;

    table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    while (key) {
        const gchar *value;

        value = va_arg(args, const gchar *);
        g_hash_table_insert(table, g_strdup(key), g_strdup(value));

        key = va_arg(args, const gchar *);
    }

    return table;
}



/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

