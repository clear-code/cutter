/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

G_DEFINE_TYPE(CutUnifiedDiffer, cut_unified_differ, CUT_TYPE_DIFFER)

static void diff        (CutDiffer     *differ,
                         CutDiffWriter *writer);

static void
cut_unified_differ_class_init (CutUnifiedDifferClass *klass)
{
    CutDifferClass *differ_class;

    differ_class = CUT_DIFFER_CLASS(klass);
    differ_class->diff = diff;
}

static void
cut_unified_differ_init (CutUnifiedDiffer *differ)
{
}

CutDiffer *
cut_unified_differ_new (const gchar *from, const gchar *to)
{
    return g_object_new(CUT_TYPE_UNIFIED_DIFFER,
                        "from", from,
                        "to", to,
                        NULL);
}

static void
mark_equal (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, " ", NULL, lines, begin, end,
                               CUT_DIFF_WRITER_TAG_EQUAL_MARK);
}

static void
mark_inserted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "+", NULL, lines, begin, end,
                               CUT_DIFF_WRITER_TAG_INSERTED_MARK);
}

static void
mark_deleted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "-", NULL, lines, begin, end,
                               CUT_DIFF_WRITER_TAG_DELETED_MARK);
}

static void
format_summary (CutDiffer *differ, CutDiffWriter *writer,
                const GList *operations)
{
    GString *format;
    CutSequenceMatchOperation *first_operation, *last_operation;
    gint deleted_lines, inserted_lines;

    first_operation = operations->data;
    while (g_list_next(operations)) {
        operations = g_list_next(operations);
    }
    last_operation = operations->data;

    format = g_string_new("@@ ");
    g_string_append_printf(format, "-%d", first_operation->from_begin + 1);
    deleted_lines = last_operation->from_end - first_operation->from_begin;
    if (deleted_lines > 1)
        g_string_append_printf(format, ",%d", deleted_lines);
    g_string_append_printf(format, " +%d", first_operation->to_begin + 1);
    inserted_lines = last_operation->to_end - first_operation->to_begin;
    if (inserted_lines > 1)
        g_string_append_printf(format, ",%d", inserted_lines);
    g_string_append(format, " @@");
    cut_diff_writer_write_line(writer, format->str,
                               CUT_DIFF_WRITER_TAG_DIFFERENCE); /* FIXME */
    g_string_free(format, TRUE);
}

static gboolean
is_same_contents (const GList *groups)
{
    const GList *group;
    CutSequenceMatchOperation *operation;

    if (g_list_next(groups))
        return FALSE;

    group = groups->data;
    if (group && g_list_next(group))
        return FALSE;

    operation = group->data;
    return operation->type == CUT_SEQUENCE_MATCH_OPERATION_EQUAL &&
        operation->from_begin == operation->to_begin &&
        operation->from_end == operation->to_end;
}

static void
diff (CutDiffer *differ, CutDiffWriter *writer)
{
    CutSequenceMatcher *matcher;
    const GList *groups;
    gchar **from, **to;

    from = cut_differ_get_from(differ);
    to = cut_differ_get_to(differ);
    matcher = cut_differ_get_sequence_matcher(differ);
    groups = cut_sequence_matcher_get_grouped_operations(matcher);
    if (!groups)
        return;
    if (is_same_contents(groups))
        return;

    cut_diff_writer_mark_line(writer, "---", " ", "from",
                              CUT_DIFF_WRITER_TAG_DELETED_MARK);
    cut_diff_writer_mark_line(writer, "+++", " ", "to",
                              CUT_DIFF_WRITER_TAG_DELETED_MARK);
    for (; groups; groups = g_list_next(groups)) {
        const GList *operations = groups->data;

        format_summary(differ, writer, operations);
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
cut_diff_unified (const gchar *from, const gchar *to)
{
    CutDiffWriter *writer;
    CutDiffer *differ;
    gchar *diff;

    differ = cut_unified_differ_new(from, to);
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
