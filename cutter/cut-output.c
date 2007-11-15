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

static GList *outputs = NULL;
static gchar *module_dir = NULL;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

void cut_output_init (void)
{
}

void cut_output_quit (void)
{
    cut_output_unload();
    cut_output_set_default_module_dir(NULL);
}

const gchar *
cut_output_get_default_module_dir (void)
{
    return module_dir;
}

void
cut_output_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

static const gchar *
_cut_output_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CUT_OUTPUT_MODULE_DIR");
    if (dir)
        return dir;

    return OUTPUT_MODULEDIR;
}

void
cut_output_load (const gchar *base_dir)
{
    if (!base_dir)
        base_dir = _cut_output_module_dir();

    outputs = g_list_concat(cut_module_load_modules(base_dir), outputs);
}

CutModule *
cut_output_load_module (const gchar *name)
{
    CutModule *module;

    module = cut_module_find(outputs, name);
    if (module)
        return module;

    module = cut_module_load_module(_cut_output_module_dir(), name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            outputs = g_list_prepend(outputs, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
    }

    return module;
}

void
cut_output_unload (void)
{
    g_list_foreach(outputs, (GFunc)cut_module_unload, NULL);
    g_list_free(outputs);
    outputs = NULL;
}

GList *
cut_output_get_registered_types (void)
{
    return cut_module_collect_registered_types(outputs);
}

GList *
cut_output_get_log_domains (void)
{
    return cut_module_collect_log_domains(outputs);
}

#define cut_output_init init
G_DEFINE_ABSTRACT_TYPE (CutOutput, cut_output, G_TYPE_OBJECT)
#undef cut_output_init

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
init (CutOutput *output)
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

CutOutput *
cut_output_new (const gchar *name, const gchar *first_property, ...)
{
    CutModule *module;
    GObject *output;
    va_list var_args;

    module = cut_output_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    output = cut_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return CUT_OUTPUT(output);
}

void
cut_output_set_verbose_level (CutOutput *output, CutVerboseLevel level)
{
    CutOutputPrivate *priv = CUT_OUTPUT_GET_PRIVATE(output);

    priv->verbose_level = level;
}

CutVerboseLevel
cut_output_get_verbose_level (CutOutput *output)
{
    return CUT_OUTPUT_GET_PRIVATE(output)->verbose_level;
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

const gchar *
cut_output_get_source_directory (CutOutput *output)
{
    return CUT_OUTPUT_GET_PRIVATE(output)->source_directory;
}

void
cut_output_set_use_color (CutOutput *output, gboolean use_color)
{
    g_object_set(output, "use_color", use_color, NULL);
}

void
cut_output_on_start_test_suite (CutOutput *output, CutTestSuite *test_suite)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_start_test_suite)
        klass->on_start_test_suite(output, test_suite);
}

void
cut_output_on_start_test_case (CutOutput *output, CutTestCase *test_case)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_start_test_case)
        klass->on_start_test_case(output, test_case);
}

void
cut_output_on_start_test (CutOutput *output, CutTestCase *test_case,
                          CutTest *test)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_start_test)
        klass->on_start_test(output, test_case, test);
}

void
cut_output_on_complete_test (CutOutput *output, CutTestCase *test_case,
                             CutTest *test, CutTestResult *result)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    if (result && cut_test_result_get_status(result) == CUT_TEST_RESULT_ERROR)
        cut_output_on_error(output, test, result);

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_complete_test)
        klass->on_complete_test(output, test_case, test, result);
}

void
cut_output_on_success (CutOutput *output, CutTest *test)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_success)
        klass->on_success(output, test);
}

void
cut_output_on_failure (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_failure)
        klass->on_failure(output, test, result);
}

void
cut_output_on_error (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_error)
        klass->on_error(output, test, result);
}

void
cut_output_on_pending (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_pending)
        klass->on_pending(output, test, result);
}

void
cut_output_on_notification (CutOutput *output, CutTest *test,
                            CutTestResult *result)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_notification)
        klass->on_notification(output, test, result);
}

void
cut_output_on_complete_test_case (CutOutput *output, CutTestCase *test_case)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_complete_test_case)
        klass->on_complete_test_case(output, test_case);
}

void
cut_output_on_complete_test_suite (CutOutput *output, CutContext *context,
                                   CutTestSuite *test_suite)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_complete_test_suite)
        klass->on_complete_test_suite(output, context, test_suite);
}

void
cut_output_on_crashed_test_suite (CutOutput *output, CutContext *context,
                                  CutTestSuite *test_suite)
{
    CutOutputClass *klass;

    g_return_if_fail(CUT_IS_OUTPUT(output));

    klass = CUT_OUTPUT_GET_CLASS(output);
    if (klass->on_crashed_test_suite)
        klass->on_crashed_test_suite(output, context, test_suite);
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
