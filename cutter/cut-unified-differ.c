/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2010  Kouhei Sutou <kou@clear-code.com>
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

#include "cut-unified-differ.h"
#include "cut-string-diff-writer.h"
#include "cut-utils.h"

#define CUT_UNIFIED_DIFFER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_UNIFIED_DIFFER, CutUnifiedDifferPrivate))

typedef struct _CutUnifiedDifferPrivate         CutUnifiedDifferPrivate;
struct _CutUnifiedDifferPrivate
{
    gchar *from_label;
    gchar *to_label;
};

G_DEFINE_TYPE(CutUnifiedDiffer, cut_unified_differ, CUT_TYPE_DIFFER)

static void dispose     (GObject       *object);

static void diff        (CutDiffer     *differ,
                         CutDiffWriter *writer);

static void
cut_unified_differ_class_init (CutUnifiedDifferClass *klass)
{
    GObjectClass *gobject_class;
    CutDifferClass *differ_class;

    gobject_class = G_OBJECT_CLASS(klass);
    differ_class = CUT_DIFFER_CLASS(klass);

    gobject_class->dispose = dispose;

    differ_class->diff = diff;

    g_type_class_add_private(gobject_class, sizeof(CutUnifiedDifferPrivate));
}

static void
cut_unified_differ_init (CutUnifiedDiffer *differ)
{
    CutUnifiedDifferPrivate *priv;

    priv = CUT_UNIFIED_DIFFER_GET_PRIVATE(differ);
    priv->from_label = NULL;
    priv->to_label = NULL;
}

static void
dispose (GObject *object)
{
    CutUnifiedDifferPrivate *priv;

    priv = CUT_UNIFIED_DIFFER_GET_PRIVATE(object);

    if (priv->from_label) {
        g_free(priv->from_label);
        priv->from_label = NULL;
    }

    if (priv->to_label) {
        g_free(priv->to_label);
        priv->to_label = NULL;
    }

    G_OBJECT_CLASS(cut_unified_differ_parent_class)->dispose(object);
}

CutDiffer *
cut_unified_differ_new (const gchar *from, const gchar *to)
{
    return g_object_new(CUT_TYPE_UNIFIED_DIFFER,
                        "from", from,
                        "to", to,
                        NULL);
}

void
cut_unified_differ_set_from_label (CutDiffer *differ, const gchar *label)
{
    CutUnifiedDifferPrivate *priv;

    priv = CUT_UNIFIED_DIFFER_GET_PRIVATE(differ);
    g_free(priv->from_label);
    priv->from_label = g_strdup(label);
}

const gchar *
cut_unified_differ_get_from_label (CutDiffer *differ)
{
    return CUT_UNIFIED_DIFFER_GET_PRIVATE(differ)->from_label;
}

void
cut_unified_differ_set_to_label (CutDiffer *differ, const gchar *label)
{
    CutUnifiedDifferPrivate *priv;

    priv = CUT_UNIFIED_DIFFER_GET_PRIVATE(differ);
    g_free(priv->to_label);
    priv->to_label = g_strdup(label);
}

const gchar *
cut_unified_differ_get_to_label (CutDiffer *differ)
{
    return CUT_UNIFIED_DIFFER_GET_PRIVATE(differ)->to_label;
}

static void
mark_equal (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, " ", NULL, lines, begin, end,
                               CUT_DIFF_WRITER_TAG_EQUAL_LINE);
}

static void
mark_inserted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "+", NULL, lines, begin, end,
                               CUT_DIFF_WRITER_TAG_INSERTED_LINE);
}

static void
mark_deleted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "-", NULL, lines, begin, end,
                               CUT_DIFF_WRITER_TAG_DELETED_LINE);
}

static void
write_header (CutUnifiedDifferPrivate *priv, CutDiffWriter *writer)
{
    const gchar *separator, *label;

    separator = NULL;
    label = "";
    if (priv->from_label && priv->from_label[0]) {
        separator = " ";
        label = priv->from_label;
    }
    cut_diff_writer_mark_line(writer, "---", separator, label,
                              CUT_DIFF_WRITER_TAG_DELETED_LINE);

    separator = NULL;
    label = "";
    if (priv->to_label && priv->to_label[0]) {
        separator = " ";
        label = priv->to_label;
    }
    cut_diff_writer_mark_line(writer, "+++", separator, label,
                              CUT_DIFF_WRITER_TAG_INSERTED_LINE);
}

static void
diff (CutDiffer *differ, CutDiffWriter *writer)
{
    CutUnifiedDifferPrivate *priv;
    CutSequenceMatcher *matcher;
    const GList *groups;
    gchar **from, **to;

    from = cut_differ_get_from(differ);
    to = cut_differ_get_to(differ);
    matcher = cut_differ_get_sequence_matcher(differ);
    groups = cut_sequence_matcher_get_grouped_operations(matcher);
    if (cut_differ_util_is_same_content(groups))
        return;

    priv = CUT_UNIFIED_DIFFER_GET_PRIVATE(differ);
    write_header(priv, writer);
    for (; groups; groups = g_list_next(groups)) {
        const GList *operations = groups->data;

        cut_differ_write_summary(differ, writer, operations);
        for (; operations; operations = g_list_next(operations)) {
            CutSequenceMatchOperation *operation = operations->data;

            switch (operation->type) {
            case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
                mark_equal(writer, from,
                           operation->from_begin, operation->from_end);
                break;
            case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
                mark_inserted(writer, to,
                              operation->to_begin, operation->to_end);
                break;
            case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
                mark_deleted(writer, from,
                             operation->from_begin, operation->from_end);
                break;
            case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
                mark_deleted(writer, from,
                             operation->from_begin, operation->from_end);
                mark_inserted(writer, to,
                              operation->to_begin, operation->to_end);
                break;
            default:
                g_error("unknown operation type: %d", operation->type);
                break;
            }
        }
    }
    cut_diff_writer_finish(writer);
}

gchar *
cut_diff_unified (const gchar *from, const gchar *to,
                  const gchar *from_label, const gchar *to_label)
{
    CutDiffWriter *writer;
    CutDiffer *differ;
    gchar *diff;

    differ = cut_unified_differ_new(from, to);
    cut_unified_differ_set_from_label(differ, from_label);
    cut_unified_differ_set_to_label(differ, to_label);
    writer = cut_string_diff_writer_new();
    cut_differ_diff(differ, writer);
    diff = g_strdup(cut_string_diff_writer_get_result(writer));
    g_object_unref(writer);
    g_object_unref(differ);

    return diff;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
