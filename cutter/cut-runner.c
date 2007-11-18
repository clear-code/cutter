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

#include "cut-runner.h"
#include "cut-context.h"
#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-verbose-level.h"
#include "cut-enum-types.h"

#define CUT_RUNNER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_RUNNER, CutRunnerPrivate))

typedef struct _CutRunnerPrivate	CutRunnerPrivate;
struct _CutRunnerPrivate
{
    CutVerboseLevel verbose_level;
    gchar *source_directory;
};

enum
{
    PROP_0,
    PROP_VERBOSE_LEVEL,
    PROP_SOURCE_DIRECTORY
};

static GList *runners = NULL;
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

void cut_runner_init (void)
{
}

void cut_runner_quit (void)
{
    cut_runner_unload();
    cut_runner_set_default_module_dir(NULL);
}

const gchar *
cut_runner_get_default_module_dir (void)
{
    return module_dir;
}

void
cut_runner_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

static const gchar *
_cut_runner_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CUT_RUNNER_MODULE_DIR");
    if (dir)
        return dir;

    return RUNNER_MODULEDIR;
}

void
cut_runner_load (const gchar *base_dir)
{
    if (!base_dir)
        base_dir = _cut_runner_module_dir();

    runners = g_list_concat(cut_module_load_modules(base_dir), runners);
}

CutModule *
cut_runner_load_module (const gchar *name)
{
    CutModule *module;

    module = cut_module_find(runners, name);
    if (module)
        return module;

    module = cut_module_load_module(_cut_runner_module_dir(), name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            runners = g_list_prepend(runners, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
    }

    return module;
}

void
cut_runner_unload (void)
{
    g_list_foreach(runners, (GFunc)cut_module_unload, NULL);
    g_list_free(runners);
    runners = NULL;
}

GList *
cut_runner_get_registered_types (void)
{
    return cut_module_collect_registered_types(runners);
}

GList *
cut_runner_get_log_domains (void)
{
    return cut_module_collect_log_domains(runners);
}

#define cut_runner_init init
G_DEFINE_ABSTRACT_TYPE (CutRunner, cut_runner, G_TYPE_OBJECT)
#undef cut_runner_init

static void
cut_runner_class_init (CutRunnerClass *klass)
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

    g_type_class_add_private(gobject_class, sizeof(CutRunnerPrivate));
}

static void
init (CutRunner *runner)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    priv->verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
    priv->source_directory = NULL;
}

static void
dispose (GObject *object)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(object);

    if (priv->source_directory) {
        g_free(priv->source_directory);
        priv->source_directory = NULL;
    }

    G_OBJECT_CLASS(cut_runner_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(object);

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
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_VERBOSE_LEVEL:
        g_value_set_enum(value, priv->verbose_level);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutRunner *
cut_runner_new (const gchar *name, const gchar *first_property, ...)
{
    CutModule *module;
    GObject *runner;
    va_list var_args;

    module = cut_runner_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    runner = cut_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return CUT_RUNNER(runner);
}

void
cut_runner_set_verbose_level (CutRunner *runner, CutVerboseLevel level)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    priv->verbose_level = level;
}

CutVerboseLevel
cut_runner_get_verbose_level (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->verbose_level;
}

void
cut_runner_set_verbose_level_by_name (CutRunner *runner, const gchar *name)
{
    GError *error = NULL;
    CutVerboseLevel level;

    level = cut_verbose_level_parse(name, &error);

    if (error) {
        g_warning("%s\n", error->message);
        g_error_free(error);
    } else {
        cut_runner_set_verbose_level(runner, level);
    }
}

void
cut_runner_set_source_directory (CutRunner *runner, const gchar *directory)
{
    CutRunnerPrivate *priv = CUT_RUNNER_GET_PRIVATE(runner);

    if (priv->source_directory) {
        g_free(priv->source_directory);
        priv->source_directory = NULL;
    }

    if (directory) {
        priv->source_directory = g_strdup(directory);
    }
}

const gchar *
cut_runner_get_source_directory (CutRunner *runner)
{
    return CUT_RUNNER_GET_PRIVATE(runner)->source_directory;
}

gboolean
cut_runner_run (CutRunner *runner, CutTestSuite *test_suite, CutContext *context)
{
    CutRunnerClass *klass;

    g_return_val_if_fail(CUT_IS_RUNNER(runner), FALSE);

    klass = CUT_RUNNER_GET_CLASS(runner);
    if (klass->run)
        return klass->run(runner, test_suite, context);
    else
        return FALSE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
