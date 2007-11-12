/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "cut-output.h"
#include "cut-context.h"
#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-verbose-level.h"
#include "cut-enum-types.h"

#define CUT_OUTPUT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_OUTPUT, CutOutputPrivate))

#define RED_COLOR "\033[01;31m"
#define GREEN_COLOR "\033[01;32m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define PURPLE_COLOR "\033[01;35m"
#define CYAN_COLOR "\033[01;36m"
#define NORMAL_COLOR "\033[00m"

typedef struct _CutOutputPrivate	CutOutputPrivate;
struct _CutOutputPrivate
{
    CutVerboseLevel verbose_level;
    gboolean use_color;
    gchar *source_directory;
};

enum
{
    PROP_0,
    PROP_VERBOSE_LEVEL,
    PROP_USE_COLOR,
    PROP_SOURCE_DIRECTORY
};

G_DEFINE_TYPE (CutOutput, cut_output, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
cut_output_class_init (CutOutputClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_enum("verbose-level",
                             "Verbose Level",
                             "The number of representing verbosity level",
                             CUT_TYPE_VERBOSE_LEVEL,
                             CUT_VERBOSE_LEVEL_NORMAL,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VERBOSE_LEVEL, spec);

    g_type_class_add_private(gobject_class, sizeof(CutOutputPrivate));
}

static void
cut_output_init (CutOutput *output)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    priv->verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
    priv->use_color = FALSE;
    priv->source_directory = NULL;
}

static void
dispose (GObject *object)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(object);

    if (priv->source_directory) {
        g_free(priv->source_directory);
        priv->source_directory = NULL;
    }

    G_OBJECT_CLASS(cut_output_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_VERBOSE_LEVEL:
        priv->verbose_level = g_value_get_enum(value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_VERBOSE_LEVEL:
        g_value_set_enum(value, priv->verbose_level);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static const gchar *
status_to_name(CutTestResultStatus status)
{
    const gchar *name;

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        name = "Success";
        break;
      case CUT_TEST_RESULT_FAILURE:
        name = "Failure";
        break;
      case CUT_TEST_RESULT_ERROR:
        name = "Error";
        break;
      case CUT_TEST_RESULT_PENDING:
        name = "Pending";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        name = "Notification";
        break;
      default:
        name = "MUST NOT HAPPEN!!!";
        break;
    }

    return name;
}

static const gchar *
status_to_color(CutTestResultStatus status)
{
    const gchar *color;

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        color = GREEN_COLOR;
        break;
      case CUT_TEST_RESULT_FAILURE:
        color = RED_COLOR;
        break;
      case CUT_TEST_RESULT_ERROR:
        color = PURPLE_COLOR;
        break;
      case CUT_TEST_RESULT_PENDING:
        color = YELLOW_COLOR;
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        color = CYAN_COLOR;
        break;
      default:
        color = "";
        break;
    }

    return color;
}

static void
print_for_status(CutOutputPrivate *priv, CutTestResultStatus status,
                 gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    if (priv->use_color) {
        gchar *message;
        message = g_strdup_vprintf(format, args);
        g_print("%s%s%s", status_to_color(status), message, NORMAL_COLOR);
        g_free(message);
    } else {
        g_vprintf(format, args);
    }
    va_end(args);
}


CutOutput *
cut_output_new (void)
{
    return g_object_new(CUT_TYPE_OUTPUT, NULL);
}

void
cut_output_set_verbose_level (CutOutput *output, CutVerboseLevel level)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    priv->verbose_level = level;
}

void
cut_output_set_verbose_level_by_name (CutOutput *output, const gchar *name)
{
    GError *error = NULL;
    CutVerboseLevel level;

    level = cut_verbose_level_parse(name, &error);

    if (error) {
        g_warning("%s\n", error->message);
        g_error_free(error);
    } else {
        cut_output_set_verbose_level(output, level);
    }
}


void
cut_output_set_source_directory (CutOutput *output, const gchar *directory)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->source_directory) {
        g_free(priv->source_directory);
        priv->source_directory = NULL;
    }

    if (directory) {
        priv->source_directory = g_strdup(directory);
    }
}

void
cut_output_set_use_color (CutOutput *output, gboolean use_color)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    priv->use_color = use_color;
}

void
cut_output_on_start_test_suite (CutOutput *output, CutTestSuite *test_suite)
{
}

void
cut_output_on_start_test_case (CutOutput *output, CutTestCase *test_case)
{
}

void
cut_output_on_start_test (CutOutput *output, CutTestCase *test_case,
                          CutTest *test)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);
    const gchar *description;

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    description = cut_test_get_description(test);
    if (description)
        g_print("%s\n", description);

    g_print("%s(%s): ",
            cut_test_get_name(test),
            cut_test_get_name(CUT_TEST(test_case)));
    fflush(stdout);
}

void
cut_output_on_complete_test (CutOutput *output, CutTestCase *test_case,
                             CutTest *test, CutTestResult *result)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (result && cut_test_result_get_status(result) == CUT_TEST_RESULT_ERROR)
        cut_output_on_error(output, test, result);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print(": (%f)\n", cut_test_get_elapsed(test));
    fflush(stdout);
}

void
cut_output_on_success (CutOutput *output, CutTest *test)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_SUCCESS, ".");
    fflush(stdout);
}

void
cut_output_on_failure (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_FAILURE, "F");
    fflush(stdout);
}

void
cut_output_on_error (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_ERROR, "E");
    fflush(stdout);
}

void
cut_output_on_pending (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_PENDING, "P");
    fflush(stdout);
}

void
cut_output_on_notification (CutOutput *output, CutTest *test,
                            CutTestResult *result)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_NOTIFICATION, "N");
    fflush(stdout);
}

void
cut_output_on_complete_test_case (CutOutput *output, CutTestCase *test_case)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

void
cut_output_on_complete_test_suite (CutOutput *output, CutContext *context,
                                   CutTestSuite *test_suite)
{
    gint i;
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications;
    const GList *node;
    CutTestResultStatus status;
    CutOutputPrivate *priv;

    priv = CUT_OUTPUT_GET_PRIVATE(output);
    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    if (priv->verbose_level == CUT_VERBOSE_LEVEL_NORMAL)
        g_print("\n");

    i = 1;
    for (node = cut_context_get_results(context);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;
        gchar *filename;
        const gchar *message;

        if (priv->source_directory)
            filename = g_build_filename(priv->source_directory,
                                        cut_test_result_get_filename(result),
                                        NULL);
        else
            filename = g_strdup(cut_test_result_get_filename(result));

        status = cut_test_result_get_status(result);
        message = cut_test_result_get_message(result);

        g_print("\n%d) ", i);
        print_for_status(priv, status, "%s", status_to_name(status));
        if (message) {
            g_print("\n");
            print_for_status(priv, status, "%s", message);
        }
        g_print("\n%s:%d: %s()\n",
                filename,
                cut_test_result_get_line(result),
                cut_test_result_get_function_name(result));
        i++;
    }

    g_print("\n");
    g_print("Finished in %f seconds",
            cut_test_get_elapsed(CUT_TEST(test_suite)));
    g_print("\n\n");


    n_tests = cut_context_get_n_tests(context);
    n_assertions = cut_context_get_n_assertions(context);
    n_failures = cut_context_get_n_failures(context);
    n_errors = cut_context_get_n_errors(context);
    n_pendings = cut_context_get_n_pendings(context);
    n_notifications = cut_context_get_n_notifications(context);

    if (n_errors > 0) {
        status = CUT_TEST_RESULT_ERROR;
    } else if (n_failures > 0) {
        status = CUT_TEST_RESULT_FAILURE;
    } else if (n_pendings > 0) {
        status = CUT_TEST_RESULT_PENDING;
    } else if (n_notifications > 0) {
        status = CUT_TEST_RESULT_NOTIFICATION;
    } else {
        status = CUT_TEST_RESULT_SUCCESS;
    }
    print_for_status(priv, status,
                     "%d tests, %d assertions, %d failures, "
                     "%d errors, %d pendings, %d notifications",
                     n_tests, n_assertions, n_failures, n_errors,
                     n_pendings, n_notifications);
    g_print("\n");
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
