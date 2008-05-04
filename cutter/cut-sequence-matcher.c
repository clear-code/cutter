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

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-list.h"
#include "cut-sequence-matcher.h"

#define CUT_SEQUENCE_MATCHER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_SEQUENCE_MATCHER, CutSequenceMatcherPrivate))

typedef struct _CutSequenceMatcherPrivate	CutSequenceMatcherPrivate;
struct _CutSequenceMatcherPrivate
{
    GSequence *from;
    GSequence *to;
    GSequenceIterCompareFunc compare_func;
    gpointer user_data;
    GHashTable *to_indexes;
};

G_DEFINE_TYPE (CutSequenceMatcher, cut_sequence_matcher, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

static void
cut_sequence_matcher_class_init (CutSequenceMatcherClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutSequenceMatcherPrivate));
}

static void
cut_sequence_matcher_init (CutSequenceMatcher *sequence_matcher)
{
    CutSequenceMatcherPrivate *priv;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(sequence_matcher);

    priv->from = NULL;
    priv->to = NULL;
    priv->to_indexes = NULL;
}

static void
dispose (GObject *object)
{
    CutSequenceMatcherPrivate *priv;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(object);

    if (priv->from) {
        g_sequence_free(priv->from);
        priv->from = NULL;
    }

    if (priv->to) {
        g_sequence_free(priv->to);
        priv->to = NULL;
    }

    if (priv->to_indexes) {
        g_hash_table_unref(priv->to_indexes);
        priv->to_indexes = NULL;
    }

    G_OBJECT_CLASS(cut_sequence_matcher_parent_class)->dispose(object);
}

static void
update_to_indexes (CutSequenceMatcher *matcher)
{
    CutSequenceMatcherPrivate *priv;
    int i;
    GSequenceIter *iter, *begin, *end;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);

    if (!priv->to)
        return;

    begin = g_sequence_get_begin_iter(priv->to);
    end = g_sequence_get_end_iter(priv->to);
    for (i = 0, iter = begin;
         iter != end;
         i++, iter = g_sequence_iter_next(iter)) {
        gchar *data;
        GList *indexes;

        data = g_sequence_get(iter);
        indexes = g_hash_table_lookup(priv->to_indexes, data);
        indexes = g_list_append(indexes, GINT_TO_POINTER(i));
        g_hash_table_replace(priv->to_indexes, data, g_list_copy(indexes));
    }
}

CutSequenceMatcher *
cut_sequence_matcher_new (GSequence *from, GSequence *to,
                          GSequenceIterCompareFunc compare_func,
                          gpointer user_data,
                          GHashFunc content_hash_func,
                          GEqualFunc content_equal_func)
{
    CutSequenceMatcher *matcher;
    CutSequenceMatcherPrivate *priv;

    matcher = g_object_new(CUT_TYPE_SEQUENCE_MATCHER, NULL);
    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    priv->from = from;
    priv->to = to;
    priv->compare_func = compare_func;
    priv->user_data = user_data;
    priv->to_indexes = g_hash_table_new_full(content_hash_func,
                                             content_equal_func,
                                             NULL,
                                             (GDestroyNotify)g_list_free);
    update_to_indexes(matcher);
    return matcher;
}

static GSequence *
char_sequence_new (const gchar *string)
{
    GSequence *sequence;

    sequence = g_sequence_new(NULL);

    for (; *string != '\0'; string++) {
        g_sequence_append(sequence, GINT_TO_POINTER(*string));
    }

    return sequence;
}

static guint
int_value_hash (gconstpointer v)
{
    gint integer;
    integer = GPOINTER_TO_INT(v);
    return g_int_hash(&integer);
}

static gboolean
int_value_equal (gconstpointer v1, gconstpointer v2)
{
    gint integer1, integer2;
    integer1 = GPOINTER_TO_INT(v1);
    integer2 = GPOINTER_TO_INT(v2);
    return g_int_equal(&integer1, &integer2);
}

CutSequenceMatcher *
cut_sequence_matcher_char_new (const gchar *from, const gchar *to)
{
    return cut_sequence_matcher_new(char_sequence_new(from),
                                    char_sequence_new(to),
                                    NULL, NULL,
                                    int_value_hash, int_value_equal);
}

static GSequence *
string_sequence_new (const gchar **strings)
{
    GSequence *sequence;

    sequence = g_sequence_new(g_free);

    for (; *strings; strings++) {
        g_sequence_append(sequence, g_strdup(*strings));
    }

    return sequence;
}

CutSequenceMatcher *
cut_sequence_matcher_string_new (const gchar **from, const gchar **to)
{
    return cut_sequence_matcher_new(string_sequence_new(from),
                                    string_sequence_new(to),
                                    NULL, NULL,
                                    g_str_hash, g_str_equal);
}

const GList *
cut_sequence_matcher_get_to_index (CutSequenceMatcher *matcher,
                                   gpointer to_content)
{
    CutSequenceMatcherPrivate *priv;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    if (!priv->to)
        return NULL;

    return g_hash_table_lookup(priv->to_indexes, to_content);
}

CutSequenceMatchInfo *
cut_sequence_matcher_longest_match (CutSequenceMatcher *matcher,
                                    gint from_begin,
                                    gint from_end,
                                    gint to_begin,
                                    gint to_end)
{
    CutSequenceMatcherPrivate *priv;
    CutSequenceMatchInfo *info;
    GSequenceIter *iter, *begin, *end;
    GHashTable *sizes, *current_sizes;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);

    info = g_new(CutSequenceMatchInfo, 1);
    info->begin = from_begin;
    info->end = to_begin;
    info->size = 0;

    sizes = g_hash_table_new(g_direct_hash, g_direct_equal);

    begin = g_sequence_get_iter_at_pos(priv->from, from_begin);
    end = g_sequence_get_iter_at_pos(priv->from, from_end + 1);
    for (iter = begin; iter != end; iter = g_sequence_iter_next(iter)) {
        gint from_index;
        const GList *node;

        from_index = g_sequence_iter_get_position(iter);
        current_sizes = g_hash_table_new(g_direct_hash, g_direct_equal);
        for (node = cut_sequence_matcher_get_to_index(matcher,
                                                      g_sequence_get(iter));
             node;
             node = g_list_next(node)) {
            gint size, to_index;
            gpointer size_as_pointer;

            to_index = GPOINTER_TO_INT(node->data);
            if (to_index < to_begin)
                continue;
            if (to_index > to_end)
                break;

            size_as_pointer = g_hash_table_lookup(sizes,
                                                  GINT_TO_POINTER(to_index - 1));
            size = GPOINTER_TO_INT(size_as_pointer);
            size++;
            size_as_pointer = GINT_TO_POINTER(size);
            g_hash_table_insert(current_sizes,
                                GINT_TO_POINTER(to_index),
                                size_as_pointer);
            if (size > info->size) {
                info->begin = from_index - size + 1;
                info->end = to_index - size + 1;
                info->size = size;
            }
        }
        g_hash_table_unref(sizes);
        sizes = current_sizes;
    }
    g_hash_table_unref(sizes);

    return info;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
