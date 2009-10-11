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

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-sequence-matcher.h"
#include "../gcutter/gcut-list.h"

#define CUT_SEQUENCE_MATCHER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_SEQUENCE_MATCHER, CutSequenceMatcherPrivate))

typedef struct _CutSequenceMatcherPrivate	CutSequenceMatcherPrivate;
struct _CutSequenceMatcherPrivate
{
    GSequence *from;
    GSequence *to;
    GSequenceIterCompareFunc compare_func;
    gpointer compare_func_user_data;
    GHashTable *to_indices;
    GHashTable *junks;
    GList *matches;
    GList *blocks;
    GList *operations;
    GList *grouped_operations;
    gdouble ratio;
};

enum
{
    PROP_0,
    PROP_FROM_SEQUENCE,
    PROP_TO_SEQUENCE,
    PROP_COMPARE_FUNC,
    PROP_COMPARE_FUNC_USER_DATA,
    PROP_TO_INDICES,
    PROP_JUNKS
};

G_DEFINE_TYPE (CutSequenceMatcher, cut_sequence_matcher, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

CutSequenceMatchInfo *
cut_sequence_match_info_new (gint from_index, gint to_index, gint size)
{
    CutSequenceMatchInfo *info;

    info = g_slice_new(CutSequenceMatchInfo);
    info->from_index = from_index;
    info->to_index = to_index;
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
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_pointer("from-sequence",
                                "From Sequence",
                                "From Sequence",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_FROM_SEQUENCE, spec);

    spec = g_param_spec_pointer("to-sequence",
                                "To Sequence",
                                "To Sequence",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_TO_SEQUENCE, spec);

    spec = g_param_spec_pointer("compare-function",
                                "Compare Fcuntion",
                                "Compare Fcuntion",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_COMPARE_FUNC, spec);

    spec = g_param_spec_pointer("compare-function-user-data",
                                "Compare Fcuntion User Data",
                                "Compare Fcuntion User Data",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_COMPARE_FUNC_USER_DATA, spec);

    spec = g_param_spec_pointer("to-indices",
                                "To Indecies",
                                "To Indecies",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_TO_INDICES, spec);

    spec = g_param_spec_pointer("junks",
                                "Junks",
                                "Junks",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_JUNKS, spec);

    g_type_class_add_private(gobject_class, sizeof(CutSequenceMatcherPrivate));
}

static void
cut_sequence_matcher_init (CutSequenceMatcher *sequence_matcher)
{
    CutSequenceMatcherPrivate *priv;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(sequence_matcher);

    priv->from = NULL;
    priv->to = NULL;
    priv->to_indices = NULL;
    priv->junks = NULL;
    priv->matches = NULL;
    priv->blocks = NULL;
    priv->operations = NULL;
    priv->grouped_operations = NULL;
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

    if (priv->to_indices) {
        g_hash_table_unref(priv->to_indices);
        priv->to_indices = NULL;
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

    if (priv->grouped_operations) {
        GList *node;
        for (node = priv->grouped_operations; node; node = g_list_next(node)) {
            GList *operations = node->data;
            g_list_foreach(operations,
                           (GFunc)cut_sequence_match_operation_free, NULL);
            g_list_free(operations);
        }
        g_list_free(priv->grouped_operations);
        priv->grouped_operations = NULL;
    }

    G_OBJECT_CLASS(cut_sequence_matcher_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutSequenceMatcherPrivate *priv;
    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_FROM_SEQUENCE:
        priv->from = g_value_get_pointer(value);
        break;
      case PROP_TO_SEQUENCE:
        priv->to = g_value_get_pointer(value);
        break;
      case PROP_COMPARE_FUNC:
        priv->compare_func = g_value_get_pointer(value);
        break;
      case PROP_COMPARE_FUNC_USER_DATA:
        priv->compare_func_user_data = g_value_get_pointer(value);
        break;
      case PROP_TO_INDICES:
        priv->to_indices = g_value_get_pointer(value);
        break;
      case PROP_JUNKS:
        priv->junks = g_value_get_pointer(value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    CutSequenceMatcherPrivate *priv;
    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_FROM_SEQUENCE:
        g_value_set_pointer(value, priv->from);
        break;
      case PROP_TO_SEQUENCE:
        g_value_set_pointer(value, priv->to);
        break;
      case PROP_COMPARE_FUNC:
        g_value_set_pointer(value, priv->compare_func);
        break;
      case PROP_COMPARE_FUNC_USER_DATA:
        g_value_set_pointer(value, priv->compare_func_user_data);
        break;
      case PROP_TO_INDICES:
        g_value_set_pointer(value, priv->to_indices);
        break;
      case PROP_JUNKS:
        g_value_set_pointer(value, priv->junks);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
remove_junks_from_to_indices (CutSequenceMatcher *matcher,
                              CutJunkFilterFunc junk_filter_func,
                              gpointer junk_filter_func_user_data)
{
    CutSequenceMatcherPrivate *priv;
    GHashTableIter iter;
    gpointer key, value;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);

    g_hash_table_iter_init(&iter, priv->to_indices);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        if (junk_filter_func(key, junk_filter_func_user_data)) {
            g_hash_table_insert(priv->junks, key, GINT_TO_POINTER(TRUE));
            g_hash_table_iter_remove(&iter);
        }
    }
}

static void
update_to_indices (CutSequenceMatcher *matcher,
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
        gpointer data;
        GList *indices;

        data = g_sequence_get(iter);
        indices = g_hash_table_lookup(priv->to_indices, data);
        indices = g_list_append(indices, GINT_TO_POINTER(i));
        g_hash_table_replace(priv->to_indices, data, g_list_copy(indices));
    }

    if (junk_filter_func)
        remove_junks_from_to_indices(matcher,
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

    matcher = g_object_new(CUT_TYPE_SEQUENCE_MATCHER,
                           "from-sequence", from,
                           "to-sequence", to,
                           "compare-function", compare_func,
                           "compare-function-user-data", compare_func_user_data,
                           "to-indices",
                           g_hash_table_new_full(content_hash_func,
                                                 content_equal_func,
                                                 NULL,
                                                 (GDestroyNotify)g_list_free),
                           "junks", g_hash_table_new(content_hash_func,
                                                     content_equal_func),
                           NULL);
    update_to_indices(matcher, junk_filter_func, junk_filter_func_user_data);
    return matcher;
}

static GSequence *
char_sequence_new (const gchar *string)
{
    GSequence *sequence;

    sequence = g_sequence_new(NULL);

    for (; *string != '\0'; string = g_utf8_next_char(string)) {
        g_sequence_append(sequence, GUINT_TO_POINTER(g_utf8_get_char(string)));
    }

    return sequence;
}

static gint
char_sequence_iter_compare (GSequenceIter *data1, GSequenceIter *data2,
                            gpointer user_data)
{
    gunichar character1, character2;

    character1 = GPOINTER_TO_UINT(g_sequence_get(data1));
    character2 = GPOINTER_TO_UINT(g_sequence_get(data2));

    if (character1 < character2) {
        return -1;
    } else if (character1 == character2) {
        return 0;
    } else {
        return 1;
    }
}

static guint
char_value_hash (gconstpointer v)
{
    return GPOINTER_TO_UINT(v);
}

static gboolean
char_value_equal (gconstpointer v1, gconstpointer v2)
{
    gunichar character1, character2;
    character1 = GPOINTER_TO_UINT(v1);
    character2 = GPOINTER_TO_UINT(v2);
    return character1 == character2;
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
                                    char_sequence_iter_compare, NULL,
                                    char_value_hash, char_value_equal,
                                    junk_filter_func,
                                    junk_filter_func_user_data);
}

static GSequence *
string_sequence_new (gchar **strings)
{
    GSequence *sequence;

    sequence = g_sequence_new(g_free);

    for (; *strings; strings++) {
        g_sequence_append(sequence, g_strdup(*strings));
    }

    return sequence;
}

static gint
string_sequence_iter_compare (GSequenceIter *data1, GSequenceIter *data2,
                              gpointer user_data)
{
    gchar *string1, *string2;

    string1 = g_sequence_get(data1);
    string2 = g_sequence_get(data2);

    return strcmp(string1, string2);
}

CutSequenceMatcher *
cut_sequence_matcher_string_new (gchar **from, gchar **to)
{
    return cut_sequence_matcher_string_new_full(from, to, NULL, NULL);
}

CutSequenceMatcher *
cut_sequence_matcher_string_new_full (gchar **from, gchar **to,
                                      CutJunkFilterFunc junk_filter_func,
                                      gpointer junk_filter_func_user_data)
{
    return cut_sequence_matcher_new(string_sequence_new(from),
                                    string_sequence_new(to),
                                    string_sequence_iter_compare, NULL,
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

    return g_hash_table_lookup(priv->to_indices, to_content);
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

    info = cut_sequence_match_info_new(from_begin, to_begin, 0);

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
                info->from_index = from_index - size + 1;
                info->to_index = to_index - size + 1;
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

    while (best_info->from_index > from_begin &&
           best_info->to_index > to_begin &&
           check_junk(priv, should_junk, best_info->to_index - 1) &&
           equal_content(priv,
                         best_info->from_index - 1,
                         best_info->to_index - 1)) {
        best_info->from_index--;
        best_info->to_index--;
        best_info->size++;
    }

    while (best_info->from_index + best_info->size < from_end &&
           best_info->to_index + best_info->size < to_end &&
           check_junk(priv,
                      should_junk,
                      best_info->to_index + best_info->size) &&
           equal_content(priv,
                         best_info->from_index + best_info->size,
                         best_info->to_index + best_info->size)) {
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

static gint
compare_match_info (gconstpointer data1, gconstpointer data2)
{
    const CutSequenceMatchInfo *info1, *info2;

    info1 = data1;
    info2 = data2;

    if (info1->from_index < info2->from_index) {
        return -1;
    } else if (info1->from_index == info2->from_index) {
        return 0;
    } else {
        return 1;
    }
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
            cut_sequence_match_info_free(match_info);
            continue;
        }

        if (info.from_begin < match_info->from_index &&
            info.to_begin < match_info->to_index)
            push_matching_info(queue,
                               info.from_begin, match_info->from_index,
                               info.to_begin, match_info->to_index);
        matches = g_list_prepend(matches, match_info);
        if (match_info->from_index + match_info->size < info.from_end &&
            match_info->to_index + match_info->size < info.to_end)
            push_matching_info(queue,
                               match_info->from_index + match_info->size,
                               info.from_end,
                               match_info->to_index + match_info->size,
                               info.to_end);
    }

    g_queue_free(queue);
    priv->matches = g_list_sort(matches, compare_match_info);

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
    gint from_index, to_index, size;
    const GList *node;
    GList *blocks = NULL;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    if (priv->blocks)
        return priv->blocks;

    from_index = to_index = size = 0;
    for (node = cut_sequence_matcher_get_matches(matcher);
         node;
         node = g_list_next(node)) {
        CutSequenceMatchInfo *info = node->data;

        if (from_index + size == info->from_index &&
            to_index + size == info->to_index) {
            size += info->size;
        } else {
            if (size > 0)
                blocks = prepend_match_info(blocks, from_index, to_index, size);
            from_index = info->from_index;
            to_index = info->to_index;
            size = info->size;
        }
    }

    if (size > 0)
        blocks = prepend_match_info(blocks, from_index, to_index, size);

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
    if (from_index < info->from_index && to_index < info->to_index) {
        return CUT_SEQUENCE_MATCH_OPERATION_REPLACE;
    } else if (from_index < info->from_index) {
        return CUT_SEQUENCE_MATCH_OPERATION_DELETE;
    } else if (to_index < info->to_index) {
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
                                           from_index, info->from_index,
                                           to_index, info->to_index);

        from_index = info->from_index + info->size;
        to_index = info->to_index + info->size;
        if (info->size > 0)
            operations = prepend_operation(operations,
                                           CUT_SEQUENCE_MATCH_OPERATION_EQUAL,
                                           info->from_index, from_index,
                                           info->to_index, to_index);
    }

    priv->operations = g_list_reverse(operations);
    return priv->operations;
}

static GList *
get_edge_expanded_copied_operations (CutSequenceMatcher *matcher,
                                     gint context_size)
{
    const GList *original_operations = NULL;
    GList *operations = NULL;

    original_operations = cut_sequence_matcher_get_operations(matcher);
    if (original_operations) {
        CutSequenceMatchOperation *operation = original_operations->data;
        const GList *node;
        gint from_begin, from_end, to_begin, to_end;

        from_begin = operation->from_begin;
        from_end = operation->from_end;
        to_begin = operation->to_begin;
        to_end = operation->to_end;
        node = original_operations;
        if (operation->type == CUT_SEQUENCE_MATCH_OPERATION_EQUAL) {
            operations = prepend_operation(operations,
                                           CUT_SEQUENCE_MATCH_OPERATION_EQUAL,
                                           MAX(from_begin,
                                               from_end - context_size),
                                           from_end,
                                           MAX(to_begin, to_end - context_size),
                                           to_end);
            node = g_list_next(node);
        }
        for (; node; node = g_list_next(node)) {
            operation = node->data;
            from_begin = operation->from_begin;
            from_end = operation->from_end;
            to_begin = operation->to_begin;
            to_end = operation->to_end;
            if (!g_list_next(node) &&
                operation->type == CUT_SEQUENCE_MATCH_OPERATION_EQUAL) {
                operations =
                    prepend_operation(operations,
                                      CUT_SEQUENCE_MATCH_OPERATION_EQUAL,
                                      from_begin,
                                      MIN(from_end, from_begin + context_size),
                                      to_begin,
                                      MIN(to_end, to_begin + context_size));
            } else {
                operations = prepend_operation(operations,
                                               operation->type,
                                               from_begin, from_end,
                                               to_begin, to_end);
            }
        }
        operations = g_list_reverse(operations);
    } else {
        operations = prepend_operation(operations,
                                       CUT_SEQUENCE_MATCH_OPERATION_EQUAL,
                                       0, 0, 0, 0);
    }

    return operations;
}

const GList *
cut_sequence_matcher_get_grouped_operations (CutSequenceMatcher *matcher)
{
    CutSequenceMatcherPrivate *priv;
    GList *node;
    GList *operations = NULL;
    GList *groups = NULL;
    GList *group = NULL;
    gint context_size = 3;
    gint group_window;

    priv = CUT_SEQUENCE_MATCHER_GET_PRIVATE(matcher);
    if (priv->grouped_operations)
        return priv->grouped_operations;

    operations = get_edge_expanded_copied_operations(matcher, context_size);
    group_window = context_size * 2;

    for (node = operations; node; node = g_list_next(node)) {
        CutSequenceMatchOperation *operation = node->data;
        guint from_begin, from_end, to_begin, to_end;

        from_begin = operation->from_begin;
        from_end = operation->from_end;
        to_begin = operation->to_begin;
        to_end = operation->to_end;
        if (operation->type == CUT_SEQUENCE_MATCH_OPERATION_EQUAL &&
            ((from_end - from_begin) > group_window)) {
            operation->from_end = MIN(from_end, from_begin + context_size);
            operation->to_end = MIN(to_end, to_begin + context_size);
            group = g_list_prepend(group, operation);
            groups = g_list_prepend(groups, g_list_reverse(group));
            group = NULL;

            from_begin = MAX(from_begin, from_end - context_size);
            to_begin = MAX(to_begin, to_end - context_size);
            group = prepend_operation(group, operation->type,
                                      from_begin, from_end,
                                      to_begin, to_end);
        } else {
            group = g_list_prepend(group, operation);
        }
    }

    if (group)
        groups = g_list_prepend(groups, g_list_reverse(group));

    g_list_free(operations);

    priv->grouped_operations = g_list_reverse(groups);
    return priv->grouped_operations;
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
