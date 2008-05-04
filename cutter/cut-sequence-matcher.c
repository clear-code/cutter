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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
