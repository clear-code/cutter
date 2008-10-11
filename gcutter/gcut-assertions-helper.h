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

#ifndef __GCUT_ASSERTIONS_HELPER_H__
#define __GCUT_ASSERTIONS_HELPER_H__

#include <glib.h>

#include <cutter/cut-assertions-helper.h>
#include <gcutter/gcut-value-equal.h>
#include <gcutter/gcut-list.h>
#include <gcutter/gcut-hash-table.h>
#include <gcutter/gcut-public.h>
#include <gcutter/gcut-test-utils.h>
#include <gcutter/gcut-error.h>
#include <gcutter/gcut-enum.h>
#include <gcutter/gcut-object.h>

G_BEGIN_DECLS

void      gcut_assert_equal_type_helper     (CutTestContext *test_context,
                                             GType           expected,
                                             GType           actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *format,
                                             ...);
void      gcut_assert_equal_value_helper    (CutTestContext *test_context,
                                             GValue         *expected,
                                             GValue         *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *format,
                                             ...);
void      gcut_assert_equal_list_int_helper (CutTestContext *test_context,
                                             const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *format,
                                             ...);
void      gcut_assert_equal_list_uint_helper(CutTestContext *test_context,
                                             const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *user_message_format,
                                             ...);
void      gcut_assert_equal_list_string_helper
                                            (CutTestContext *test_context,
                                             const GList    *expected,
                                             const GList    *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *user_message_format,
                                             ...);
void      gcut_assert_equal_list_object_helper
                                            (CutTestContext *test_context,
                                             const GList    *expected,
                                             const GList    *actual,
                                             GEqualFunc      equal_function,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *user_message_format,
                                             ...);
void      gcut_assert_equal_hash_table_string_string_helper
                                            (CutTestContext *test_context,
                                             GHashTable     *expected,
                                             GHashTable     *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *user_message_format,
                                             ...);
void      gcut_assert_error_helper          (CutTestContext *test_context,
                                             GError         *error,
                                             const gchar    *expression_error,
                                             const gchar    *user_message_format,
                                             ...);
void      gcut_assert_equal_error_helper    (CutTestContext *test_context,
                                             const GError   *expected,
                                             const GError   *actual,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *user_message_format,
                                             ...);
void      gcut_assert_equal_enum_helper     (CutTestContext *test_context,
                                             GType           enum_type,
                                             gint            expected,
                                             gint            actual,
                                             const gchar    *expression_enum_type,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *user_message_format,
                                             ...);
void      gcut_assert_equal_flags_helper    (CutTestContext *test_context,
                                             GType           flags_type,
                                             gint            expected,
                                             gint            actual,
                                             const gchar    *expression_flags_type,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *user_message_format,
                                             ...);

void      gcut_assert_equal_object_helper   (CutTestContext *test_context,
                                             GObject        *expected,
                                             GObject        *actual,
                                             GEqualFunc      equal_function,
                                             const gchar    *expression_expected,
                                             const gchar    *expression_actual,
                                             const gchar    *expression_equal_function,
                                             const gchar    *user_message_format,
                                             ...);

G_END_DECLS

#endif /* __GCUT_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
