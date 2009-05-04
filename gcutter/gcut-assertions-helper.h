/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __GCUT_ASSERTIONS_HELPER_H__
#define __GCUT_ASSERTIONS_HELPER_H__

#include <glib.h>

#include <cutter.h>
#include <gcutter/gcut-value-equal.h>
#include <gcutter/gcut-list.h>
#include <gcutter/gcut-hash-table.h>
#include <gcutter/gcut-public.h>
#include <gcutter/gcut-test-utils.h>
#include <gcutter/gcut-error.h>
#include <gcutter/gcut-enum.h>
#include <gcutter/gcut-object.h>
#include <gcutter/gcut-egg.h>
#include <gcutter/gcut-io.h>
#include <gcutter/gcut-key-file.h>
#include <gcutter/gcut-inspect.h>
#include <gcutter/gcut-data.h>

G_BEGIN_DECLS

void      gcut_assert_equal_type_helper     (GType           expected,
                                             GType           actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_value_helper    (GValue         *expected,
                                             GValue         *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_list_helper     (const GList    *expected,
                                             const GList    *actual,
                                             GEqualFunc      equal_function,
                                             GCutInspectFunction inspect_function,
                                             gpointer        inspect_user_data,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *expression_equal_function);
void      gcut_assert_equal_list_int_helper (const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_list_uint_helper(const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_list_string_helper
                                            (const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_list_enum_helper
                                            (GType           type,
                                             const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_type,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_list_flags_helper
                                            (GType           type,
                                             const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_type,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_list_object_helper
                                            (const GList    *expected,
                                             const GList    *actual,
                                             GEqualFunc      equal_function,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_hash_table_helper
                                            (GHashTable     *expected,
                                             GHashTable     *actual,
                                             GEqualFunc      equal_function,
                                             GCutInspectFunction key_inspect_function,
                                             GCutInspectFunction value_inspect_function,
                                             gpointer        inspect_user_data,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *expression_equal_function);
void      gcut_assert_equal_hash_table_string_string_helper
                                            (GHashTable     *expected,
                                             GHashTable     *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_error_helper          (GError         *error,
                                             const gchar    *expression_error);
void      gcut_assert_equal_error_helper    (const GError   *expected,
                                             const GError   *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_remove_path_helper    (const gchar    *path,
                                             ...) G_GNUC_NULL_TERMINATED;
void      gcut_assert_equal_time_val_helper (GTimeVal       *expected,
                                             GTimeVal       *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *actual_expected);
void      gcut_assert_equal_enum_helper     (GType           enum_type,
                                             gint            expected,
                                             gint            actual,
                                             const gchar    *expression_enum_type,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);
void      gcut_assert_equal_flags_helper    (GType           flags_type,
                                             gint            expected,
                                             gint            actual,
                                             const gchar    *expression_flags_type,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual);

void      gcut_assert_equal_object_helper   (GObject        *expected,
                                             GObject        *actual,
                                             GEqualFunc      equal_function,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *expression_equal_function);
void      gcut_assert_equal_int64_helper    (gint64          expected,
                                             gint64          actual,
                                             const char     *expression_expected,
                                             const char     *expression_actual);
void      gcut_assert_equal_uint64_helper   (guint64         expected,
                                             guint64         actual,
                                             const char     *expression_expected,
                                             const char     *expression_actual);
void      gcut_assert_equal_pid_helper      (GPid            expected,
                                             GPid            actual,
                                             const char     *expression_expected,
                                             const char     *expression_actual);
void      gcut_assert_not_equal_pid_helper  (GPid            expected,
                                             GPid            actual,
                                             const char     *expression_expected,
                                             const char     *expression_actual);

G_END_DECLS

#endif /* __GCUT_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
