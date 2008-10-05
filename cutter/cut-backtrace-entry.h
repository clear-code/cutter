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

#ifndef __CUT_BACKTRACE_ENTRY_H__
#define __CUT_BACKTRACE_ENTRY_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_BACKTRACE_ENTRY            (cut_backtrace_entry_get_type ())
#define CUT_BACKTRACE_ENTRY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_BACKTRACE_ENTRY, CutBacktraceEntry))
#define CUT_BACKTRACE_ENTRY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_BACKTRACE_ENTRY, CutBacktraceEntryClass))
#define CUT_IS_BACKTRACE_ENTRY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_BACKTRACE_ENTRY))
#define CUT_IS_BACKTRACE_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_BACKTRACE_ENTRY))
#define CUT_BACKTRACE_ENTRY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_BACKTRACE_ENTRY, CutBacktraceEntryClass))

typedef struct _CutBacktraceEntry      CutBacktraceEntry;
typedef struct _CutBacktraceEntryClass CutBacktraceEntryClass;

struct _CutBacktraceEntry
{
    GObject object;
};

struct _CutBacktraceEntryClass
{
    GObjectClass parent_class;
};

GType              cut_backtrace_entry_get_type  (void) G_GNUC_CONST;

CutBacktraceEntry *cut_backtrace_entry_new       (const gchar *file,
                                                  guint        line,
                                                  const gchar *function,
                                                  const gchar *info);
CutBacktraceEntry *cut_backtrace_entry_new_empty (void);

const gchar       *cut_backtrace_entry_get_file  (CutBacktraceEntry *backtrace);
void               cut_backtrace_entry_set_file  (CutBacktraceEntry *backtrace,
                                                  const gchar       *file);
guint              cut_backtrace_entry_get_line  (CutBacktraceEntry *backtrace);
void               cut_backtrace_entry_set_line  (CutBacktraceEntry *backtrace,
                                                  guint              line);
const gchar       *cut_backtrace_entry_get_function
                                                 (CutBacktraceEntry *backtrace);
void               cut_backtrace_entry_set_function
                                                 (CutBacktraceEntry *backtrace,
                                                  const gchar       *function);
const gchar       *cut_backtrace_entry_get_info  (CutBacktraceEntry *backtrace);
void               cut_backtrace_entry_set_info  (CutBacktraceEntry *backtrace,
                                                  const gchar       *info);

gchar             *cut_backtrace_entry_to_xml    (CutBacktraceEntry *backtrace);
void               cut_backtrace_entry_to_xml_string
                                                 (CutBacktraceEntry *backtrace,
                                                  GString           *string,
                                                  guint              indent);

gchar             *cut_backtrace_entry_format    (CutBacktraceEntry *backtrace);

G_END_DECLS

#endif /* __CUT_BACKTRACE_ENTRY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
