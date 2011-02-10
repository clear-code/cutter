/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2011  Kouhei Sutou <kou@clear-code.com>
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

#include <string.h>

#include <cutter/cut-test.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-runner.h>
#include <gdkcutter-pixbuf.h>
#include <gdkcutter-pixbuf/gdkcut-pixbuf.h>
#include "../lib/cuttest-assertions.h"

void test_equal(void);

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;

static GdkPixbuf *pixbuf1, *pixbuf2;

static gint fail_line;

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))

static gboolean
run (void)
{
    gboolean success;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                       test, test_context);
    cut_test_context_current_pop();

    return success;
}

void
cut_setup (void)
{
    test = NULL;
    run_context = NULL;
    test_context = NULL;
    test_result = NULL;

    pixbuf1 = NULL;
    pixbuf2 = NULL;

    fail_line = 0;

    cut_set_fixture_data_dir(cuttest_get_base_dir(),
                             "fixtures",
                             "pixbuf",
                             NULL);
}

void
cut_teardown (void)
{
    gchar *diff_image_path;

    if (test)
        g_object_unref(test);
    if (run_context)
        g_object_unref(run_context);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);

    if (pixbuf1)
        g_object_unref(pixbuf1);
    if (pixbuf2)
        g_object_unref(pixbuf2);

    diff_image_path = g_strdup_printf("%s-%u.png", __FILE__, fail_line);
    cut_remove_path(diff_image_path, NULL);
    g_free(diff_image_path);
}

static GdkPixbuf *
load_pixbuf (const gchar *path)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    gchar *pixbuf_path;

    pixbuf_path = cut_build_fixture_data_path(path, NULL);
    pixbuf = gdk_pixbuf_new_from_file(pixbuf_path, &error);
    g_free(pixbuf_path);

    gcut_assert_error(error);

    return pixbuf;
}

static void
stub_equal (void)
{
    gdkcut_pixbuf_assert_equal(pixbuf1, pixbuf1, 0);
    gdkcut_pixbuf_assert_equal(pixbuf2, pixbuf2, 0);
    gdkcut_pixbuf_assert_equal(pixbuf1, pixbuf2, 3);
    MARK_FAIL(gdkcut_pixbuf_assert_equal(pixbuf1, pixbuf2, 2));
}

void
test_equal (void)
{
    gchar *inspected_pixbuf1, *inspected_pixbuf2;
    const gchar *message;

    pixbuf1 = load_pixbuf("dark-circle.png");
    pixbuf2 = load_pixbuf("max-3-pixel-value-different-dark-circle.png");

    test = cut_test_new("equal test", stub_equal);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);

    inspected_pixbuf1 = gcut_object_inspect(G_OBJECT(pixbuf1));
    inspected_pixbuf2 = gcut_object_inspect(G_OBJECT(pixbuf2));
    message = cut_take_printf("<pixbuf1 == pixbuf2> (2)\n"
                              "  expected: <%s>\n"
                              "    actual: <%s>\n"
                              " threshold: <2>\n"
                              "diff image: <%s-%u.png>",
                              inspected_pixbuf1, inspected_pixbuf2,
                              __FILE__, fail_line);
    g_free(inspected_pixbuf1);
    g_free(inspected_pixbuf2);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal test",
                           NULL,
                           message,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal",
                           NULL);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
