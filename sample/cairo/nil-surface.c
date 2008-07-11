/*
 * Copyright © 2005 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Red Hat, Inc. not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Red Hat, Inc. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * RED HAT, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL RED HAT, INC. BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carl D. Worth <cworth@cworth.org>
 */

#include <stddef.h>
#include <cairo.h>
#ifdef CAIRO_HAS_SVG_SURFACE
#  include <cairo-svg.h>
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
#  include <cairo-pdf.h>
#endif
#ifdef CAIRO_HAS_PS_SURFACE
#  include <cairo-ps.h>
#endif

#include <cutter.h>

void data_nil_surface (void);
void test_nil_surface (void *data);

typedef cairo_surface_t *(*SurfaceCreator) (const char *file_name);

typedef struct _SurfaceTestData
{
    SurfaceCreator creator;
    char *file_name;
} SurfaceTestData;

static SurfaceTestData *
surface_test_data_new (SurfaceCreator creator, const char *file_name)
{
    SurfaceTestData *test_data;

    test_data = malloc(sizeof(SurfaceTestData));
    test_data->creator = creator;
    if (file_name)
        test_data->file_name = strdup(file_name);
    else
        test_data->file_name = NULL;

    return test_data;
}

static void
surface_test_data_free (void *data)
{
    SurfaceTestData *test_data = data;

    free(test_data->file_name);
    free(test_data);
}


static cairo_t *cr;
static cairo_surface_t *surface;
static char *file_name;

void
setup (void)
{
    cr = NULL;
    surface = NULL;
    file_name = NULL;
}

void
teardown (void)
{
    if (cr)
        cairo_destroy (cr);
    if (surface)
        cairo_surface_destroy (surface);
    if (file_name) {
        cut_remove_path (file_name);
        free (file_name);
    }
}

static cairo_surface_t *
image_surface_create (const char *file_name)
{
    return cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 100, 100);
}

#ifdef CAIRO_HAS_SVG_SURFACE
static cairo_surface_t *
svg_surface_create (const char *file_name)
{
    return cairo_svg_surface_create (file_name, 100, 100);
}
#endif

#ifdef CAIRO_HAS_PDF_SURFACE
static cairo_surface_t *
pdf_surface_create (const char *file_name)
{
    return cairo_pdf_surface_create (file_name, 100, 100);
}
#endif

#ifdef CAIRO_HAS_PS_SURFACE
static cairo_surface_t *
ps_surface_create (const char *file_name)
{
    return cairo_ps_surface_create (file_name, 100, 100);
}
#endif

void
data_nil_surface (void)
{
    cut_add_data ("image surface",
                  surface_test_data_new (image_surface_create, NULL),
                  surface_test_data_free);
#ifdef CAIRO_HAS_SVG_SURFACE
    cut_add_data ("SVG surface",
                  surface_test_data_new (svg_surface_create, "nil-surface.svg"),
                  surface_test_data_free);
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    cut_add_data ("PDF surface",
                  surface_test_data_new (pdf_surface_create, "nil-surface.pdf"),
                  surface_test_data_free);
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    cut_add_data ("PS surface",
                  surface_test_data_new (ps_surface_create, "nil-surface.ps"),
                  surface_test_data_free);
#endif
}

/* Test to verify fixes for the following similar bugs:
 *
 *	https://bugs.freedesktop.org/show_bug.cgi?id=4088
 *	https://bugs.freedesktop.org/show_bug.cgi?id=3915
 *	https://bugs.freedesktop.org/show_bug.cgi?id=9906
 */

static void
draw (cairo_t *cr)
{
    cairo_surface_t *surface;
    cairo_pattern_t *pattern;
    cairo_t *cr2;

    /*
     * 1. Test file-not-found from surface->pattern->cairo_t
     */

    /* Make a custom context to not interfere with the one passed in. */
    cr2 = cairo_create (cairo_get_target (cr));

    /* First, let's make a nil surface. */
    surface = cairo_image_surface_create_from_png ("___THIS_FILE_DOES_NOT_EXIST___");

    /* Let the error propagate into a nil pattern. */
    pattern = cairo_pattern_create_for_surface (surface);

    /* Then let it propagate into the cairo_t. */
    cairo_set_source (cr2, pattern);
    cairo_paint (cr2);

    cairo_pattern_destroy (pattern);
    cairo_surface_destroy (surface);

    /* Check that the error made it all that way. */
    cut_assert_equal_int (CAIRO_STATUS_FILE_NOT_FOUND, cairo_status (cr2),
			  "Error: Received status of \"%s\" rather than expected \"%s\"\n",
			  cairo_status_to_string (cairo_status (cr2)),
			  cairo_status_to_string (CAIRO_STATUS_FILE_NOT_FOUND));

    cairo_destroy (cr2);

    /*
     * 2. Test NULL pointer pattern->cairo_t
     */
    cr2 = cairo_create (cairo_get_target (cr));

    /* First, trigger the NULL pointer status. */
    pattern = cairo_pattern_create_for_surface (NULL);

    /* Then let it propagate into the cairo_t. */
    cairo_set_source (cr2, pattern);
    cairo_paint (cr2);

    cairo_pattern_destroy (pattern);

    /* Check that the error made it all that way. */
    cut_assert_equal_int (CAIRO_STATUS_NULL_POINTER, cairo_status (cr2),
    			  "Error: Received status of \"%s\" rather than expected \"%s\"\n",
			  cairo_status_to_string (cairo_status (cr2)),
			  cairo_status_to_string (CAIRO_STATUS_NULL_POINTER));
    cairo_destroy (cr2);

    /*
     * 3. Test that cairo_surface_finish can accept NULL or a nil
     *    surface without crashing.
     */

    cairo_surface_finish (NULL);

    surface = cairo_image_surface_create_from_png ("___THIS_FILE_DOES_NOT_EXIST___");
    cairo_surface_finish (surface);
    cairo_surface_destroy (surface);

    /*
     * 4. OK, we're straying from the original name, but it's still a
     * similar kind of testing of error paths. Here we're making sure
     * we can still call a cairo_get_* function after triggering an
     * INVALID_RESTORE error.
     */
    cr2 = cairo_create (cairo_get_target (cr));

    /* Trigger invalid restore. */
    cairo_restore (cr2);
    cut_assert_equal_int (CAIRO_STATUS_INVALID_RESTORE, cairo_status (cr2),
		          "Error: Received status of \"%s\" rather than expected \"%s\"\n",
			  cairo_status_to_string (cairo_status (cr2)),
			  cairo_status_to_string (CAIRO_STATUS_INVALID_RESTORE));

    /* Test that we can still call cairo_get_fill_rule without crashing. */
    cairo_get_fill_rule (cr2);

    cairo_destroy (cr2);

    /*
     * 5. Create a cairo_t for the NULL surface.
     */
    cr2 = cairo_create (NULL);

    cut_assert_equal_int (CAIRO_STATUS_NULL_POINTER, cairo_status (cr2),
		          "Error: Received status of \"%s\" rather than expected \"%s\"\n",
			  cairo_status_to_string (cairo_status (cr2)),
			  cairo_status_to_string (CAIRO_STATUS_NULL_POINTER));

    /* Test that get_target returns something valid */
    cut_assert (cairo_get_target (cr2),
		"Error: cairo_get_target() returned NULL\n");

    /* Test that push_group doesn't crash */
    cairo_push_group (cr2);
    cairo_stroke (cr2);
    cairo_pop_group (cr2);

    cairo_destroy (cr2);
}

void
test_nil_surface (void *data)
{
    SurfaceTestData *test_data = data;

    if (test_data->file_name) {
        cut_remove_path (test_data->file_name);
        file_name = strdup (test_data->file_name);
    }

    surface = test_data->creator (test_data->file_name);
    cut_assert_not_null (surface);
    draw (cairo_create (surface));
    cairo_surface_destroy (surface);
    surface = NULL;
}
