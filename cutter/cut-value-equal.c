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

#include <glib.h>
#include "gbsearcharray.h"

#include "cut-value-equal.h"

typedef struct {
    GType type1;
    GType type2;
    GEqualFunc func;
} EqualEntry;

static gint	equal_entries_cmp (gconstpointer bsearch_node1,
                               gconstpointer bsearch_node2);

static GBSearchArray *equal_entries = NULL;
static GBSearchConfig equal_bconfig = {
    sizeof (EqualEntry),
    equal_entries_cmp,
    0,
};

static GEqualFunc
equal_func_lookup (GType type1, GType type2)
{
    EqualEntry entry;

    entry.type1 = type1;
    do {
        entry.type2 = type2;
        do {
            EqualEntry *found_entry;

            found_entry = g_bsearch_array_lookup(equal_entries,
                                                 &equal_bconfig,
                                                 &entry);
            if (found_entry) {
                if (g_type_value_table_peek(entry.type1) ==
                    g_type_value_table_peek(type1) &&
                    g_type_value_table_peek(entry.type2) ==
                    g_type_value_table_peek(type2))
                    return found_entry->func;
            }

            entry.type2 = g_type_parent(entry.type2);
        } while (entry.type2);

        entry.type1 = g_type_parent(entry.type1);
    } while (entry.type1);

    return NULL;
}

static gint
equal_entries_cmp (gconstpointer bsearch_node1,
                   gconstpointer bsearch_node2)
{
    const EqualEntry *entry1 = bsearch_node1;
    const EqualEntry *entry2 = bsearch_node2;
    gint result;

    result = G_BSEARCH_ARRAY_CMP(entry1->type1, entry2->type1);
    if (result == 0)
        result = G_BSEARCH_ARRAY_CMP(entry1->type2, entry2->type2);

    return result;
}

void
cut_value_register_equal_func (GType type1, GType type2, GEqualFunc equal_func)
{
    EqualEntry entry;

    g_return_if_fail(equal_func != NULL);

    entry.type1 = type1;
    entry.type2 = type2;
    entry.func = equal_func;

    equal_entries = g_bsearch_array_replace(equal_entries,
                                            &equal_bconfig,
                                            &entry);
}

static gboolean
value_equal_int_int (gconstpointer val1, gconstpointer val2)
{
    const GValue *value1 = val1;
    const GValue *value2 = val2;

    return g_value_get_int(value1) == g_value_get_int(value2);
}

static gboolean
value_equal_string_string (gconstpointer val1, gconstpointer val2)
{
    const GValue *value1 = val1;
    const GValue *value2 = val2;

    return g_str_equal(g_value_get_string(value1),
                       g_value_get_string(value2));
}

void
cut_value_equal_init (void)
{
    equal_entries = g_bsearch_array_create(&equal_bconfig);

    cut_value_register_equal_func(G_TYPE_INT, G_TYPE_INT, value_equal_int_int);
    cut_value_register_equal_func(G_TYPE_STRING, G_TYPE_STRING,
                                   value_equal_string_string);
}

gboolean
cut_value_equal (GValue *value1, GValue *value2)
{
    GEqualFunc func;
    GType type1, type2;

    type1 = G_VALUE_TYPE(value1);
    type2 = G_VALUE_TYPE(value2);

    if (type1 == 0 && type2 == 0)
        return TRUE;

    if (type1 == 0 || type2 == 0)
        return FALSE;

    func = equal_func_lookup(type1, type2);
    if (func)
        return func(value1, value2);

    func = equal_func_lookup(type2, type1);
    if (func)
        return func(value2, value1);

    return FALSE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

