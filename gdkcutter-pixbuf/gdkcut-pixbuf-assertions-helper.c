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
#  include "../cutter/config.h"
#endif /* HAVE_CONFIG_H */

#include "gdkcut-pixbuf-assertions-helper.h"
#include "gdkcut-pixbuf.h"

void
gdkcut_pixbuf_assert_equal_helper (const GdkPixbuf *expected,
                                   const GdkPixbuf *actual,
                                   guint            threshold,
                                   const gchar     *expression_expected,
                                   const gchar     *expression_actual,
                                   const gchar     *expression_threshold,
                                   const gchar     *user_message_format,
                                   ...)
{
    if (gdkcut_pixbuf_equal_content(expected, actual, threshold)) {
        cut_test_pass();
    } else {
        GString *message;
        const gchar *fail_message;
        gchar *inspected_expected;
        gchar *inspected_actual;

        message = g_string_new(NULL);
        g_string_append_printf(message,
                               "<%s == %s> (%s)\n",
                               expression_expected,
                               expression_actual,
                               expression_threshold);
        inspected_expected = gcut_object_inspect(G_OBJECT(expected));
        inspected_actual = gcut_object_inspect(G_OBJECT(actual));
        g_string_append_printf(message,
                               "  expected: <%s>\n"
                               "    actual: <%s>\n"
                               " threshold: <%u>",
                               inspected_expected,
                               inspected_actual,
                               threshold);
        if (gdkcut_pixbuf_equal_property(expected, actual)) {
            const gchar *filename;
            guint line;
            gchar *diff_image_file_prefix;
            gchar *diff_image_file;
            GError *error = NULL;

            cut_test_context_get_last_backtrace(cut_get_current_test_context(),
                                                &filename, &line,
                                                NULL, NULL);
            diff_image_file_prefix = g_strdup_printf("%s-%u", filename, line);
            diff_image_file =
                gdkcut_pixbuf_save_diff(expected,
                                        actual,
                                        threshold,
                                        diff_image_file_prefix,
                                        &error);
            if (error) {
                gchar *inspected_error;

                inspected_error = gcut_error_inspect(error);
                cut_test_register_result_va_list(NOTIFICATION,
                                                 inspected_error,
                                                 user_message_format);
                g_free(inspected_error);
            } else {
                g_string_append_printf(message,
                                       "\n"
                                       "diff image: <%s>",
                                       diff_image_file);
                g_free(diff_image_file);
            }
            fail_message = cut_take_string(g_string_free(message, FALSE));
        } else if (expected && actual) {
            fail_message = cut_append_diff(message->str,
                                           inspected_expected,
                                           inspected_actual);
            g_string_free(message, TRUE);
        } else {
            fail_message = cut_take_string(g_string_free(message, FALSE));
        }
        cut_test_fail_va_list(fail_message, user_message_format);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
