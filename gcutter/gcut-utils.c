/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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

#include <cutter/cut-test-context.h>
#include "gcut-public.h"
#include "gcut-error.h"

GString *
gcut_utils_get_fixture_data (CutTestContext *context,
                             const gchar **fixture_data_path,
                             const char *path,
                             ...)
{
    GString *data;
    va_list args;

    va_start(args, path);
    data = gcut_utils_get_fixture_data_va_list(context, fixture_data_path,
                                               path, args);
    va_end(args);

    return data;
}

GString *
gcut_utils_get_fixture_data_va_list (CutTestContext *context,
                                     const gchar **fixture_data_path,
                                     const gchar *path,
                                     va_list args)
{
    GError *error = NULL;
    GString *fixture_data;

    fixture_data =
        cut_test_context_get_fixture_data_va_list(context, &error,
                                                  fixture_data_path,
                                                  path, args);
    if (error) {
        gchar *inspected, *message;

        inspected = gcut_error_inspect(error);
        message = g_strdup_printf("can't get fixture data: %s", inspected);
        g_error_free(error);
        cut_test_context_register_result(context,
                                         CUT_TEST_RESULT_ERROR,
                                         message);
        g_free(inspected);
        g_free(message);
        cut_test_context_long_jump(context);
    }

    return fixture_data;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
