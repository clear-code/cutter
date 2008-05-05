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
    gpointer compare_func_user_data;
    GHashTable *to_indexes;
    GHashTable *junks;
    GList *matches;
    GList *blocks;
    GList *operations;
    gdouble ratio;
};

G_DEFINE_TYPE (CutSequenceMatcher, cut_sequence_matcher, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

CutSequenceMatchInfo *
cut_sequence_match_info_new (gint begin, gint end, gint size)
{
    CutSequenceMatchInfo *info;

    info = g_slice_new(CutSequenceMatchInfo);
    info->begin = begin;
    info->end = end;
    info->size = size;
    return info;
}

void
cut_sequence_match_info_free (CutSequenceMatchInfo *info)
{
    g_slice_free(CutSequenceMatchInfo, info);
}

CutSequenceMatchOperation *
cut_sequence_match_operation_new (CutSequenceMatchOperationType type,
                                  gint from_begin, gint from_end,
                                  gint to_begin, gint to_end)
{
    CutSequenceMatchOperation *operation;

    operation = g_slice_new(CutSequenceMatchOperation);
    operation->type = type;
    operation->from_begin = from_begin;
    operation->from_end = from_end;
    operation->to_begin = to_begin;
    operation->to_end = to_end;
    return operation;
}

void
cut_sequence_match_operation_free (CutSequenceMatchOperation *operation)
{
    g_slice_free(CutSequenceMatchOperation, operation);
}

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
    priv->junks = NULL;
    priv->matches = NULL;
    priv->blocks = NULL;
    priv->operations = NULL;
    priv->ratio = -1.0;
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

    if (priv->junks) {
        g_hash_table_unref(priv->junks);
        priv->junks = NULL;
    }

    if (priv->matches) {
        g_list_foreach(priv->matches, (GFunc)cut_sequence_match_info_free, NULL);
        g_list_free(priv->matches);
        priv->matches = NULL;
    }

    if (priv->blocks) {
        g_list_foreach(priv->blocks, (GFunc)cut_sequence_match_info_free, NULL);
        g_list_free(priv->blocks);
        priv->blocks = NULL;
    }


    if (priv->operations) {
        g_list_foreach(priv->operations,
                       (GFunc)cut_sequence_match_operation_free, NULL);
        g_list_free(priv->operations);
        priv->operations = NULL;
    }

    G_OBJECT_CLASS(cut_sequence_matcher_parent_class)->dispose(object);
}

static void
remove_junks_from_to_indexes (CutSequenceMatcher *matcher,
                              CutJunkFilterFunc junk_filter_func,
                              gpointer junk_filter_func_user_data)
{
    CutSequenceMatcherPrivate *priv;
    GHashTableIter iter;
    gpointer key, value;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);

    g_hash_table_iter_init(&iter, priv->to_indexes);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        if (junk_filter_func(key, junk_filter_func_user_data)) {
            g_hash_table_insert(priv->junks, key, GINT_TO_POINTER(TRUE));
            g_hash_table_iter_remove(&iter);
        }
    }
}

static void
update_to_indexes (CutSequenceMatcher *matcher,
                   CutJunkFilterFunc junk_filter_func,
                   gpointer junk_filter_func_user_data)
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

    if (junk_filter_func)
        remove_junks_from_to_indexes(matcher,
                                     junk_filter_func,
                                     junk_filter_func_user_data);
}

CutSequenceMatcher *
cut_sequence_matcher_new (GSequence *from, GSequence *to,
                          GSequenceIterCompareFunc compare_func,
                          gpointer compare_func_user_data,
                          GHashFunc content_hash_func,
                          GEqualFunc content_equal_func,
                          CutJunkFilterFunc junk_filter_func,
                          gpointer junk_filter_func_user_data)
{
    CutSequenceMatcher *matcher;
    CutSequenceMatcherPrivate *priv;

    matcher = g_object_new(CUT_TYPE_SEQUENCE_MATCHER, NULL);
    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    priv->from = from;
    priv->to = to;
    priv->compare_func = compare_func;
    priv->compare_func_user_data = compare_func_user_data;
    priv->to_indexes = g_hash_table_new_full(content_hash_func,
                                             content_equal_func,
                                             NULL,
                                             (GDestroyNotify)g_list_free);
    priv->junks = g_hash_table_new(content_hash_func, content_equal_func);
    update_to_indexes(matcher, junk_filter_func, junk_filter_func_user_data);
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
    return cut_sequence_matcher_char_new_full(from, to, NULL, NULL);
}

CutSequenceMatcher *
cut_sequence_matcher_char_new_full (const gchar *from, const gchar *to,
                                    CutJunkFilterFunc junk_filter_func,
                                    gpointer junk_filter_func_user_data)
{
    return cut_sequence_matcher_new(char_sequence_new(from),
                                    char_sequence_new(to),
                                    NULL, NULL,
                                    int_value_hash, int_value_equal,
                                    junk_filter_func,
                                    junk_filter_func_user_data);
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
    return cut_sequence_matcher_string_new_full(from, to, NULL, NULL);
}

CutSequenceMatcher *
cut_sequence_matcher_string_new_full (const gchar **from, const gchar **to,
                                      CutJunkFilterFunc junk_filter_func,
                                      gpointer junk_filter_func_user_data)
{
    return cut_sequence_matcher_new(string_sequence_new(from),
                                    string_sequence_new(to),
                                    NULL, NULL,
                                    g_str_hash, g_str_equal,
                                    junk_filter_func,
                                    junk_filter_func_user_data);
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

static CutSequenceMatchInfo *
find_best_match_position (CutSequenceMatcher *matcher,
                          gint from_begin, gint from_end,
                          gint to_begin, gint to_end)
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

static gboolean
check_junk (CutSequenceMatcherPrivate *priv, gboolean should_junk, gint index)
{
    gpointer key;
    gboolean is_junk;

    key = g_sequence_get(g_sequence_get_iter_at_pos(priv->to, index));
    is_junk = GPOINTER_TO_INT(g_hash_table_lookup(priv->junks, key));
    return should_junk ? is_junk : !is_junk;
}

static gboolean
equal_content (CutSequenceMatcherPrivate *priv, gint from_index, gint to_index)
{
    GSequenceIter *from_iter, *to_iter;

    from_iter = g_sequence_get_iter_at_pos(priv->from, from_index);
    to_iter = g_sequence_get_iter_at_pos(priv->to, to_index);
    return priv->compare_func(from_iter, to_iter,
                              priv->compare_func_user_data) == 0;
}


static void
adjust_best_info_with_junk_predicate (CutSequenceMatcher *matcher,
                                      gboolean should_junk,
                                      CutSequenceMatchInfo *best_info,
                                      gint from_begin, gint from_end,
                                      gint to_begin, gint to_end)
{
    CutSequenceMatcherPrivate *priv;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);

    while (best_info->begin > from_begin &&
           best_info->end > to_begin &&
           check_junk(priv, should_junk, best_info->end - 1) &&
           equal_content(priv, best_info->begin - 1, best_info->end - 1)) {
        best_info->begin--;
        best_info->end--;
        best_info->size++;
    }

    while (best_info->begin + best_info->size < from_end &&
           best_info->end + best_info->size < to_end &&
           check_junk(priv, should_junk, best_info->end + best_info->size) &&
           equal_content(priv,
                         best_info->begin + best_info->size,
                         best_info->end + best_info->size)) {
        best_info->size++;
    }
}

CutSequenceMatchInfo *
cut_sequence_matcher_get_longest_match (CutSequenceMatcher *matcher,
                                        gint from_begin, gint from_end,
                                        gint to_begin, gint to_end)
{
    CutSequenceMatcherPrivate *priv;
    CutSequenceMatchInfo *info;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);

    info = find_best_match_position(matcher,
                                    from_begin, from_end,
                                    to_begin, to_end);
    if (g_hash_table_size(priv->junks) > 0) {
        adjust_best_info_with_junk_predicate(matcher, FALSE, info,
                                             from_begin, from_end,
                                             to_begin, to_end);
        adjust_best_info_with_junk_predicate(matcher, TRUE, info,
                                             from_begin, from_end,
                                             to_begin, to_end);
    }

    return info;
}

typedef struct _MatchingInfo MatchingInfo;
struct _MatchingInfo {
    gint from_begin;
    gint from_end;
    gint to_begin;
    gint to_end;
};

static void
push_matching_info (GQueue *queue,
                    gint from_begin, gint from_end,
                    gint to_begin, gint to_end)
{
    MatchingInfo *info;

    info = g_slice_new(MatchingInfo);
    info->from_begin = from_begin;
    info->from_end = from_end;
    info->to_begin = to_begin;
    info->to_end = to_end;

    g_queue_push_tail(queue, info);
}

static void
pop_matching_info (GQueue *queue, MatchingInfo *info)
{
    MatchingInfo *popped_info;

    popped_info = g_queue_pop_head(queue);
    *info = *popped_info;
    g_slice_free(MatchingInfo, popped_info);
}

const GList *
cut_sequence_matcher_get_matches (CutSequenceMatcher *matcher)
{
    CutSequenceMatcherPrivate *priv;
    GList *matches = NULL;
    GQueue *queue;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    if (priv->matches)
        return priv->matches;

    queue = g_queue_new();
    push_matching_info(queue,
                       0, g_sequence_get_length(priv->from),
                       0, g_sequence_get_length(priv->to));

    while (!g_queue_is_empty(queue)) {
        MatchingInfo info;
        CutSequenceMatchInfo *match_info;

        pop_matching_info(queue, &info);
        match_info = cut_sequence_matcher_get_longest_match(matcher,
                                                            info.from_begin,
                                                            info.from_end - 1,
                                                            info.to_begin,
                                                            info.to_end - 1);
        if (match_info->size == 0) {
            g_free(match_info);
            continue;
        }

        if (info.from_begin < match_info->begin &&
            info.to_begin < match_info->end)
            push_matching_info(queue,
                               info.from_begin, match_info->begin,
                               info.to_begin, match_info->end);
        matches = g_list_prepend(matches, match_info);
        if (match_info->begin + match_info->size < info.from_end &&
            match_info->end + match_info->size < info.to_end)
            push_matching_info(queue,
                               match_info->begin + match_info->size,
                               info.from_end,
                               match_info->end + match_info->size,
                               info.to_end);
    }

    g_queue_free(queue);
    priv->matches = g_list_reverse(matches);

    return priv->matches;
}

static GList *
prepend_match_info (GList *list, gint begin, gint end, gint size)
{
    return g_list_prepend(list, cut_sequence_match_info_new(begin, end, size));
}

const GList *
cut_sequence_matcher_get_blocks (CutSequenceMatcher *matcher)
{
    CutSequenceMatcherPrivate *priv;
    gint begin, end, size;
    const GList *node;
    GList *blocks = NULL;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    if (priv->blocks)
        return priv->blocks;

    begin = end = size = 0;
    for (node = cut_sequence_matcher_get_matches(matcher);
         node;
         node = g_list_next(node)) {
        CutSequenceMatchInfo *info = node->data;

        if (begin + size == info->begin && end + size == info->end) {
            size += info->size;
        } else {
            if (size > 0)
                blocks = prepend_match_info(blocks, begin, end, size);
            begin = info->begin;
            end = info->end;
            size = info->size;
        }
    }

    if (size > 0)
        blocks = prepend_match_info(blocks, begin, end, size);

    blocks = prepend_match_info(blocks,
                                g_sequence_get_length(priv->from),
                                g_sequence_get_length(priv->to),
                                0);
    priv->blocks = g_list_reverse(blocks);

    return priv->blocks;
}

static CutSequenceMatchOperationType
determine_operation_type (gint from_index, gint to_index,
                          CutSequenceMatchInfo *info)
{
    if (from_index < info->begin && to_index < info->end) {
        return CUT_SEQUENCE_MATCH_OPERATION_REPLACE;
    } else if (from_index < info->begin) {
        return CUT_SEQUENCE_MATCH_OPERATION_DELETE;
    } else if (to_index < info->end) {
        return CUT_SEQUENCE_MATCH_OPERATION_INSERT;
    } else {
        return CUT_SEQUENCE_MATCH_OPERATION_EQUAL;
    }
}

static GList *
prepend_operation (GList *list, CutSequenceMatchOperationType type,
                   gint from_begin, gint from_end, gint to_begin, gint to_end)
{
    return g_list_prepend(list,
                          cut_sequence_match_operation_new(type,
                                                           from_begin,
                                                           from_end,
                                                           to_begin,
                                                           to_end));
}

const GList *
cut_sequence_matcher_get_operations (CutSequenceMatcher *matcher)
{
    CutSequenceMatcherPrivate *priv;
    const GList *node;
    GList *operations = NULL;
    gint from_index = 0;
    gint to_index = 0;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    if (priv->operations)
        return priv->operations;

    for (node = cut_sequence_matcher_get_blocks(matcher);
         node;
         node = g_list_next(node)) {
        CutSequenceMatchInfo *info = node->data;
        CutSequenceMatchOperationType type;

        type = determine_operation_type(from_index, to_index, info);
        if (type != CUT_SEQUENCE_MATCH_OPERATION_EQUAL)
            operations = prepend_operation(operations, type,
                                           from_index, info->begin,
                                           to_index, info->end);

        from_index = info->begin + info->size;
        to_index = info->end + info->size;
        if (info->size > 0)
            operations = prepend_operation(operations,
                                           CUT_SEQUENCE_MATCH_OPERATION_EQUAL,
                                           info->begin, from_index,
                                           info->end, to_index);
    }

    priv->operations = g_list_reverse(operations);
    return priv->operations;
}

gdouble
cut_sequence_matcher_get_ratio (CutSequenceMatcher *matcher)
{
    CutSequenceMatcherPrivate *priv;
    const GList *node;
    gint length;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    if (priv->ratio >= 0.0)
        return priv->ratio;

    length = g_sequence_get_length(priv->from) + g_sequence_get_length(priv->to);
    if (length == 0) {
        priv->ratio = 1.0;
    } else {
        gint matches = 0;

        for (node = cut_sequence_matcher_get_blocks(matcher);
             node;
             node = g_list_next(node)) {
            CutSequenceMatchInfo *info = node->data;

            matches += info->size;
        }
        priv->ratio = 2.0 * matches / length;
    }

    return priv->ratio;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
