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

#ifndef __CUT_SEQUENCE_MATCHER_H__
#define __CUT_SEQUENCE_MATCHER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_SEQUENCE_MATCHER            (cut_sequence_matcher_get_type ())
#define CUT_SEQUENCE_MATCHER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_SEQUENCE_MATCHER, CutSequenceMatcher))
#define CUT_SEQUENCE_MATCHER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_SEQUENCE_MATCHER, CutSequenceMatcherClass))
#define CUT_IS_SEQUENCE_MATCHER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_SEQUENCE_MATCHER))
#define CUT_IS_SEQUENCE_MATCHER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_SEQUENCE_MATCHER))
#define CUT_SEQUENCE_MATCHER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_SEQUENCE_MATCHER, CutSequenceMatcherClass))

typedef struct _CutSequenceMatcher         CutSequenceMatcher;
typedef struct _CutSequenceMatcherClass    CutSequenceMatcherClass;

struct _CutSequenceMatcher
{
    GObject object;
};

struct _CutSequenceMatcherClass
{
    GObjectClass parent_class;
};

GType               cut_sequence_matcher_get_type       (void) G_GNUC_CONST;

CutSequenceMatcher *cut_sequence_matcher_new   (GSequence *from,
                                                GSequence *to,
                                                GSequenceIterCompareFunc compare_func,
                                                gpointer user_data,
                                                GHashFunc content_hash_func,
                                                GEqualFunc content_equal_func);
const GList *cut_sequence_matcher_get_to_index (CutSequenceMatcher *matcher,
                                                gpointer to_content);

G_END_DECLS

#endif /* __CUT_SEQUENCE_MATCHER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
