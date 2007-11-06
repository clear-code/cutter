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
#include "cut-enum-types.h"

#define CUT_OUTPUT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_OUTPUT, CutOutputPrivate))

#define RED_COLOR "\033[01;31m"
#define GREEN_COLOR "\033[01;32m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define PURPLE_COLOR "\033[01;35m"
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
      default:
        name = "";
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
        g_print(status_to_color(status));
        g_vprintf(format, args);
        g_print(NORMAL_COLOR);
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
    CutVerboseLevel level;

    if (name == NULL) {
        level = CUT_VERBOSE_LEVEL_NORMAL;
    } else if (g_utf8_collate(name, "s") == 0 ||
               g_utf8_collate(name, "silent") == 0) {
        level = CUT_VERBOSE_LEVEL_SILENT;
    } else if (g_utf8_collate(name, "n") == 0 ||
               g_utf8_collate(name, "normal") == 0) {
        level = CUT_VERBOSE_LEVEL_NORMAL;
    } else if (g_utf8_collate(name, "v") == 0 ||
               g_utf8_collate(name, "verbose") == 0) {
        level = CUT_VERBOSE_LEVEL_VERBOSE;
    } else {
        g_warning("Invalid verbose level name: %s", name);
        return;
    }

    cut_output_set_verbose_level(output, level);
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
cut_output_on_start_test (CutOutput *output, CutTest *test)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print("%s: ", cut_test_get_function_name(test));
}

void
cut_output_on_complete_test (CutOutput *output, CutTest *test)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print("\n");
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
cut_output_on_failure (CutOutput *output, CutTest *test)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_FAILURE, "F");
    fflush(stdout);
}

void
cut_output_on_error (CutOutput *output, CutTest *test)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_ERROR, "E");
    fflush(stdout);
}

void
cut_output_on_pending (CutOutput *output, CutTest *test)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(priv, CUT_TEST_RESULT_PENDING, "P");
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
cut_output_on_complete_test_suite (CutOutput *output, CutTestSuite *test_suite)
{
    gint i;
    gint n_tests, assertions, failures, errors, pendings;
    const GList *test_case_node;
    CutTestContainer *container;
    CutTestResultStatus status;
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    if (priv->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    n_tests = 0;

    i = 1;
    container = CUT_TEST_CONTAINER(test_suite);
    for (test_case_node = cut_test_container_get_children(container);
         test_case_node;
         test_case_node = g_list_next(test_case_node)) {
        const GList *test_node;
        CutTestCase *test_case;

        test_case = test_case_node->data;
        container = CUT_TEST_CONTAINER(test_case);
        for (test_node = cut_test_container_get_children(container);
             test_node;
             test_node = g_list_next(test_node)) {
            CutTest *test = test_node->data;
            const CutTestResult *result;
            gchar *filename;

            n_tests++;

            result = cut_test_get_result(test);
            if (!result)
                continue;

            if (priv->source_directory)
                filename = g_build_filename(priv->source_directory,
                                            result->filename,
                                            NULL);
            else
                filename = g_strdup(result->filename);

            g_print("\n\n%d) ", i);
            print_for_status(priv, result->status,
                             status_to_name(result->status));
            g_print("\n");
            print_for_status(priv, result->status, result->message);
            g_print("\n%s:%d: %s()",
                    filename, result->line, result->function_name);

            i++;
        }
    }

    g_print("\n\n");
    g_print("Finished in %g seconds",
            cut_test_get_elapsed(CUT_TEST(test_suite)));
    g_print("\n\n");

    assertions = cut_test_get_n_assertions(CUT_TEST(test_suite));
    failures = cut_test_get_n_failures(CUT_TEST(test_suite));
    errors = cut_test_get_n_errors(CUT_TEST(test_suite));
    pendings = cut_test_get_n_pendings(CUT_TEST(test_suite));
    if (errors > 0) {
        status = CUT_TEST_RESULT_ERROR;
    } else if (failures > 0) {
        status = CUT_TEST_RESULT_FAILURE;
    } else if (pendings > 0) {
        status = CUT_TEST_RESULT_PENDING;
    } else {
        status = CUT_TEST_RESULT_SUCCESS;
    }
    print_for_status(priv, status,
                     "%d tests, %d assertions, %d failures, "
                     "%d errors, %d pendings",
                     n_tests, assertions, failures, errors, pendings);
    g_print("\n");
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
