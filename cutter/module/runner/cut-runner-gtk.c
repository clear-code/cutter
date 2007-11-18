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
#include <gmodule.h>
#include <gtk/gtk.h>

#include "cut-module-impl.h"
#include "cut-runner.h"
#include "cut-context.h"
#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-verbose-level.h"
#include "cut-enum-types.h"

#define CUT_TYPE_RUNNER_GTK            cut_type_runner_gtk
#define CUT_RUNNER_GTK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUNNER_GTK, CutRunnerGtk))
#define CUT_RUNNER_GTK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUNNER_GTK, CutRunnerGtkClass))
#define CUT_IS_RUNNER_GTK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUNNER_GTK))
#define CUT_IS_RUNNER_GTK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUNNER_GTK))
#define CUT_RUNNER_GTK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUNNER_GTK, CutRunnerGtkClass))

typedef struct _CutRunnerGtk CutRunnerGtk;
typedef struct _CutRunnerGtkClass CutRunnerGtkClass;

struct _CutRunnerGtk
{
    CutRunner     object;
    gchar        *name;
    gboolean      use_color;

    GtkTextBuffer *text_buffer;
};

struct _CutRunnerGtkClass
{
    CutRunnerClass parent_class;
};

enum
{
    PROP_0
};

static GType cut_type_runner_gtk = 0;
static CutRunnerClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void on_start_test_suite    (CutRunner *runner,
                                    CutTestSuite *test_suite);
static void on_start_test_case     (CutRunner *runner,
                                    CutTestCase *test_case);
static void on_start_test          (CutRunner *runner,
                                    CutTestCase *test_case,
                                    CutTest *test);
static void on_complete_test       (CutRunner *runner,
                                    CutTestCase *test_case,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_success             (CutRunner *runner,
                                    CutTest *test);
static void on_failure             (CutRunner *runner,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_error               (CutRunner *runner,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_pending             (CutRunner *runner,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_notification        (CutRunner *runner,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_complete_test_case  (CutRunner *runner,
                                    CutTestCase *test_case);
static void on_complete_test_suite (CutRunner *runner,
                                    CutContext *context,
                                    CutTestSuite *test_suite);

static void
class_init (CutRunnerClass *klass)
{
    GObjectClass *gobject_class;
    CutRunnerClass *runner_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    runner_class  = CUT_RUNNER_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    runner_class->on_start_test_suite    = on_start_test_suite;
    runner_class->on_start_test_case     = on_start_test_case;
    runner_class->on_start_test          = on_start_test;
    runner_class->on_success             = on_success;
    runner_class->on_failure             = on_failure;
    runner_class->on_error               = on_error;
    runner_class->on_pending             = on_pending;
    runner_class->on_notification        = on_notification;
    runner_class->on_complete_test       = on_complete_test;
    runner_class->on_complete_test_case  = on_complete_test_case;
    runner_class->on_complete_test_suite = on_complete_test_suite;
}

static void
init (CutRunnerGtk *runner)
{
    GtkWidget *window;
    GtkWidget *text_view;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(window), text_view);
    gtk_widget_show(text_view);

    runner->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    gtk_widget_show(window);
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutRunnerGtkClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutRunnerGtk),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_runner_gtk = g_type_module_register_type(type_module,
                                                      CUT_TYPE_RUNNER,
                                                      "CutRunnerGtk",
                                                      &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_runner_gtk)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_runner_gtk));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_RUNNER_GTK, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    switch (prop_id) {
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
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
on_start_test_suite (CutRunner *runner, CutTestSuite *test_suite)
{
}

static void
on_start_test_case (CutRunner *runner, CutTestCase *test_case)
{
}

static void
on_start_test (CutRunner *runner, CutTestCase *test_case,
               CutTest *test)
{
}

static void
on_complete_test (CutRunner *runner, CutTestCase *test_case,
                  CutTest *test, CutTestResult *result)
{
}

static void
on_success (CutRunner *runner, CutTest *test)
{
    GtkTextIter iter;
    CutRunnerGtk *runner_gtk = CUT_RUNNER_GTK(runner);

    gtk_text_buffer_get_end_iter(runner_gtk->text_buffer, &iter);
    gtk_text_buffer_insert(runner_gtk->text_buffer, &iter, ".", -1);
}

static void
on_failure (CutRunner *runner, CutTest *test, CutTestResult *result)
{
}

static void
on_error (CutRunner *runner, CutTest *test, CutTestResult *result)
{
}

static void
on_pending (CutRunner *runner, CutTest *test, CutTestResult *result)
{
}

static void
on_notification (CutRunner *runner, CutTest *test,
                 CutTestResult *result)
{
}

static void
on_complete_test_case (CutRunner *runner, CutTestCase *test_case)
{
}

static void
on_complete_test_suite (CutRunner *runner, CutContext *context,
                        CutTestSuite *test_suite)
{
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
