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

#include "cut-sequence-matcher.h"
#include "cut-colorize-differ.h"
#include "cut-utils.h"

G_DEFINE_TYPE(CutColorizeDiffer, cut_colorize_differ, CUT_TYPE_READABLE_DIFFER)

static gdouble get_best_ratio    (CutDiffer *differ);
static gdouble get_cut_off_ratio (CutDiffer *differ);

static void diff_line   (CutDiffer     *differ,
                         CutDiffWriter *writer,
                         gchar         *from_line,
                         gchar         *to_line);

static void
cut_colorize_differ_class_init (CutColorizeDifferClass *klass)
{
    CutDifferClass *differ_class;
    CutReadableDifferClass *readable_differ_class;

    differ_class = CUT_DIFFER_CLASS(klass);
    differ_class->get_best_ratio = get_best_ratio;
    differ_class->get_cut_off_ratio = get_cut_off_ratio;

    readable_differ_class = CUT_READABLE_DIFFER_CLASS(klass);
    readable_differ_class->diff_line = diff_line;
}

static void
cut_colorize_differ_init (CutColorizeDiffer *differ)
{
}

CutDiffer *
cut_colorize_differ_new (const gchar *from, const gchar *to)
{
    return g_object_new(CUT_TYPE_COLORIZE_DIFFER,
                        "from", from,
                        "to", to,
                        NULL);
}

static gdouble
get_best_ratio (CutDiffer *differ)
{
    return 0.0;
}

static gdouble
get_cut_off_ratio (CutDiffer *differ)
{
    return 0.0;
}

static void
write_equal_segment (CutDiffWriter *writer, const gchar *line,
                     guint begin, guint end)
{
    cut_diff_writer_write_segment(writer, line, begin, end,
                                  CUT_DIFF_WRITER_TAG_EQUAL_SEGMENT);
}

static void
write_inserted_segment (CutDiffWriter *writer, const gchar *line,
                        guint begin, guint end)
{
    cut_diff_writer_write_segment(writer, line, begin, end,
                                  CUT_DIFF_WRITER_TAG_INSERTED_SEGMENT);
}

static void
write_deleted_segment (CutDiffWriter *writer, const gchar *line,
                       guint begin, guint end)
{
    cut_diff_writer_write_segment(writer, line, begin, end,
                                  CUT_DIFF_WRITER_TAG_DELETED_SEGMENT);
}

static void
write_difference_spaces (CutDiffWriter *writer, guint n)
{
    cut_diff_writer_write_character_n_times(
        writer, ' ', n, CUT_DIFF_WRITER_TAG_DIFFERENCE_SEGMENT);
}

static void
write_equal_spaces (CutDiffWriter *writer, guint n)
{
    cut_diff_writer_write_character_n_times(writer, ' ', n,
                                            CUT_DIFF_WRITER_TAG_EQUAL_SEGMENT);
}

typedef enum
{
    WRITE_EQUAL_SPACES,
    WRITE_DIFFERENCE_SPACES,
    WRITE_INSERTED_SEGMENT,
} WriteType;

typedef struct _WriteOperation
{
    WriteType type;
    guint width;
    const gchar *line;
    guint begin;
    guint end;
} WriteOperation;

static WriteOperation *
equal_spaces_write_operation_new (guint width)
{
    WriteOperation *operation;

    operation = g_new0(WriteOperation, 1);
    operation->type = WRITE_EQUAL_SPACES;
    operation->width = width;
    return operation;
}

static WriteOperation *
difference_spaces_write_operation_new (guint width)
{
    WriteOperation *operation;

    operation = g_new0(WriteOperation, 1);
    operation->type = WRITE_DIFFERENCE_SPACES;
    operation->width = width;
    return operation;
}

static WriteOperation *
inserted_segment_write_operation_new (const gchar *line, guint begin, guint end)
{
    WriteOperation *operation;

    operation = g_new0(WriteOperation, 1);
    operation->type = WRITE_INSERTED_SEGMENT;
    operation->line = line;
    operation->begin = begin;
    operation->end = end;
    return operation;
}

static void
diff_line (CutDiffer *differ, CutDiffWriter *writer,
           gchar *from_line, gchar *to_line)
{
    GList *write_operations = NULL;
    CutSequenceMatcher *matcher;
    const GList *operations;
    gboolean no_replace = TRUE;

    matcher =
        cut_sequence_matcher_char_new_full(from_line, to_line,
                                           cut_differ_util_is_space_character,
                                           NULL);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;

        if (operation->type == CUT_SEQUENCE_MATCH_OPERATION_REPLACE) {
            no_replace = FALSE;
            break;
        }
    }

    cut_diff_writer_write_mark(writer, "?", " ",
                               CUT_DIFF_WRITER_TAG_DIFFERENCE_MARK);
#define APPEND_WRITE_OPERATION(write_operation)                         \
    write_operations = g_list_append(write_operations, write_operation)

    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;
        guint from_width, to_width;

        from_width = cut_differ_util_compute_width(from_line,
                                                   operation->from_begin,
                                                   operation->from_end);
        to_width = cut_differ_util_compute_width(to_line,
                                                 operation->to_begin,
                                                 operation->to_end);
        switch (operation->type) {
        case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            write_equal_segment(writer, from_line,
                                operation->from_begin,
                                operation->from_end);
            if (!no_replace)
                APPEND_WRITE_OPERATION(
                    equal_spaces_write_operation_new(to_width));
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            if (no_replace) {
                write_inserted_segment(writer, to_line,
                                       operation->to_begin,
                                       operation->to_end);
            } else {
                write_difference_spaces(writer, to_width);
                APPEND_WRITE_OPERATION(
                    inserted_segment_write_operation_new(to_line,
                                                         operation->to_begin,
                                                         operation->to_end));
            }
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            write_deleted_segment(writer, from_line,
                                  operation->from_begin,
                                  operation->from_end);
            if (!no_replace)
                APPEND_WRITE_OPERATION(
                    difference_spaces_write_operation_new(from_width));
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            write_deleted_segment(writer, from_line,
                                  operation->from_begin,
                                  operation->from_end);
            if (from_width < to_width)
                write_difference_spaces(writer, to_width - from_width);

            APPEND_WRITE_OPERATION(
                inserted_segment_write_operation_new(to_line,
                                                     operation->to_begin,
                                                     operation->to_end));
            if (to_width < from_width)
                APPEND_WRITE_OPERATION(
                    difference_spaces_write_operation_new(from_width - to_width));
            break;
        default:
            g_error("unknown operation type: %d", operation->type);
            break;
        }
    }
#undef APPEND_WRITE_OPERATION
    cut_diff_writer_write_line(writer, "", CUT_DIFF_WRITER_TAG_NONE);

    if (write_operations) {
        GList *node;

        cut_diff_writer_write_mark(writer, "?", " ",
                                   CUT_DIFF_WRITER_TAG_DIFFERENCE_MARK);
        for (node = write_operations; node; node = g_list_next(node)) {
            WriteOperation *operation = node->data;

            switch (operation->type) {
            case WRITE_EQUAL_SPACES:
                write_equal_spaces(writer, operation->width);
                break;
            case WRITE_DIFFERENCE_SPACES:
                write_difference_spaces(writer, operation->width);
                break;
            case WRITE_INSERTED_SEGMENT:
                write_inserted_segment(writer,
                                       operation->line,
                                       operation->begin,
                                       operation->end);
                break;
            }
            g_free(operation);
        }
        g_list_free(write_operations);
        cut_diff_writer_write_line(writer, "", CUT_DIFF_WRITER_TAG_NONE);
    }

    g_object_unref(matcher);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
