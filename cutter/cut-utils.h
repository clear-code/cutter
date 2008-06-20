/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_UTILS_H__
#define __CUT_UTILS_H__

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
    CUT_READ,
    CUT_WRITE
} CutPipeMode;

gchar   *cut_utils_create_regex_pattern (const gchar *string);
GList   *cut_utils_filter_to_regexs     (const gchar **filters);
gboolean cut_utils_filter_match         (GList *regexs,
                                         const gchar *name);
void     cut_utils_append_indent        (GString *string,
                                         guint    size);
void     cut_utils_append_xml_element_with_value
                                        (GString     *string,
                                         guint        indent,
                                         const gchar *element_name,
                                         const gchar *value);
void     cut_utils_append_xml_element_with_boolean_value
                                        (GString     *string,
                                         guint        indent,
                                         const gchar *element_name,
                                         gboolean     boolean);
gchar  **cut_utils_strv_concat          (const gchar **str_array,
                                         ...) G_GNUC_MALLOC G_GNUC_NULL_TERMINATED;

void     cut_utils_close_pipe           (int         *pipe,
                                         CutPipeMode  mode);

const gchar *cut_utils_get_cutter_command_path (void);

gchar       *cut_utils_build_pathv      (const gchar *path, va_list *args);
gchar       *cut_utils_expand_path      (const gchar *path);
gchar       *cut_utils_expand_pathv     (const gchar *path, va_list *args);


#ifdef G_OS_WIN32
const gchar *cut_win32_base_path                     (void);
gchar       *cut_win32_build_module_dir_name         (const gchar *type);
gchar       *cut_win32_build_factory_module_dir_name (const gchar *type);
#endif

G_END_DECLS

#endif /* __CUT_UTILS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
