/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

#include "soupcut-assertions-helper.h"
#include "soupcutter.h"


void
soupcut_message_assert_equal_content_type_helper (const gchar *expected,
                                                  const SoupMessage *actual,
                                                  const gchar     *expression_expected,
                                                  const gchar     *expression_actual)
{
    const gchar *content_type;
    content_type = soup_message_headers_get_content_type(actual->response_headers,
                                                         NULL);
    if (cut_utils_equal_string(expected, content_type)) {
        cut_test_pass();
    } else {
        GString *message;
        const gchar *fail_message;
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        message = g_string_new(NULL);
        g_string_append_printf(message,
                               "<%s == %s[response][content-type]>\n",
                               expression_expected,
                               expression_actual);
        inspected_expected = cut_utils_inspect_string(expected);
        inspected_actual = cut_utils_inspect_string(content_type);
        g_string_append_printf(message,
                               "  expected: <%s>\n"
                               "    actual: <%s>",
                               inspected_expected,
                               inspected_actual);
        fail_message = cut_take_string(g_string_free(message, FALSE));
        cut_test_fail(fail_message);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
