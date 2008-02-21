/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-ui-factory.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_UI_FACTORY_CONSOLE            cut_type_ui_factory_console
#define CUT_UI_FACTORY_CONSOLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UI_FACTORY_CONSOLE, CutUIFactoryConsole))
#define CUT_UI_FACTORY_CONSOLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_UI_FACTORY_CONSOLE, CutUIFactoryConsoleClass))
#define CUT_IS_UI_FACTORY_CONSOLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UI_FACTORY_CONSOLE))
#define CUT_IS_UI_FACTORY_CONSOLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_UI_FACTORY_CONSOLE))
#define CUT_UI_FACTORY_CONSOLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_UI_FACTORY_CONSOLE, CutUIFactoryConsoleClass))

typedef struct _CutUIFactoryConsole CutUIFactoryConsole;
typedef struct _CutUIFactoryConsoleClass CutUIFactoryConsoleClass;

struct _CutUIFactoryConsole
{
    CutUIFactory     object;

    gboolean             use_color;
    CutVerboseLevel      verbose_level;
};

struct _CutUIFactoryConsoleClass
{
    CutUIFactoryClass parent_class;
};

enum
{
    PROP_0,
    PROP_USE_COLOR,
    PROP_VERBOSE_LEVEL
};

static GType cut_type_ui_factory_console = 0;
static CutUIFactoryClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void       set_option_group (CutUIFactory    *factory,
                                    GOptionContext      *context);
static CutUI     *create           (CutUIFactory    *factory);

static void
class_init (CutUIFactoryClass *klass)
{
    GObjectClass *gobject_class;
    CutUIFactoryClass *factory_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class  = CUT_UI_FACTORY_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    factory_class->set_option_group = set_option_group;
    factory_class->create           = create;

    spec = g_param_spec_boolean("use-color",
                                "Use color",
                                "Whether use color",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_USE_COLOR, spec);

    spec = g_param_spec_enum("verbose-level",
                             "Verbose Level",
                             "The number of representing verbosity level",
                             CUT_TYPE_VERBOSE_LEVEL,
                             CUT_VERBOSE_LEVEL_NORMAL,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VERBOSE_LEVEL, spec);
}

static void
init (CutUIFactoryConsole *console)
{
    console->use_color = FALSE;
    console->verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutUIFactoryConsoleClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutUIFactoryConsole),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_ui_factory_console =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_UI_FACTORY,
                                    "CutUIFactoryConsole",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_ui_factory_console)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_ui_factory_console));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_UI_FACTORY_CONSOLE, first_property, var_args);
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
    CutUIFactoryConsole *console = CUT_UI_FACTORY_CONSOLE(object);

    switch (prop_id) {
      case PROP_USE_COLOR:
        console->use_color = g_value_get_boolean(value);
        break;
      case PROP_VERBOSE_LEVEL:
        console->verbose_level = g_value_get_enum(value);
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
    CutUIFactoryConsole *console = CUT_UI_FACTORY_CONSOLE(object);

    switch (prop_id) {
      case PROP_USE_COLOR:
        g_value_set_boolean(value, console->use_color);
        break;
      case PROP_VERBOSE_LEVEL:
        g_value_set_enum(value, console->verbose_level);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gboolean
guess_color_usability (void)
{
    const gchar *term, *emacs;

    term = g_getenv("TERM");
    if (term && (g_str_has_suffix(term, "term") || g_str_equal(term, "screen")))
        return TRUE;

    emacs = g_getenv("EMACS");
    if (emacs && (g_str_equal(emacs, "t")))
        return TRUE;

    return FALSE;
}

static gboolean
parse_verbose_level_arg (const gchar *option_name, const gchar *value,
                         gpointer data, GError **error)
{
    CutUIFactoryConsole *console = data;
    CutVerboseLevel verbose_level;
    GError *verbose_level_error = NULL;

    verbose_level = cut_verbose_level_parse(value, &verbose_level_error);
    if (!verbose_level_error) {
        console->verbose_level = verbose_level;
        return TRUE;
    }

    g_set_error(error,
                G_OPTION_ERROR,
                G_OPTION_ERROR_BAD_VALUE,
                "%s", verbose_level_error->message);
    g_error_free(verbose_level_error);

    return FALSE;
}

static gboolean
parse_color_arg (const gchar *option_name, const gchar *value,
                 gpointer data, GError **error)
{
    CutUIFactoryConsole *console = data;

    if (value == NULL ||
        g_utf8_collate(value, "yes") == 0 ||
        g_utf8_collate(value, "true") == 0) {
        console->use_color = TRUE;
    } else if (g_utf8_collate(value, "no") == 0 ||
               g_utf8_collate(value, "false") == 0) {
        console->use_color = FALSE;
    } else if (g_utf8_collate(value, "auto") == 0) {
        console->use_color = guess_color_usability();
    } else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_BAD_VALUE,
                    _("Invalid color value: %s"), value);
        return FALSE;
    }

    return TRUE;
}

static gboolean
pre_parse (GOptionContext *context, GOptionGroup *group, gpointer data,
           GError **error)
{
    CutUIFactoryConsole *console = data;

    console->use_color = guess_color_usability();

    return TRUE;
}

static void
set_option_group (CutUIFactory *factory, GOptionContext *context)
{
    CutUIFactoryConsole *console = CUT_UI_FACTORY_CONSOLE(factory);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {"verbose", 'v', 0, G_OPTION_ARG_CALLBACK, parse_verbose_level_arg,
         N_("Set verbose level"), "[s|silent|n|normal|v|verbose]"},
        {"color", 'c', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
         parse_color_arg, N_("Output log with colors"),
         "[yes|true|no|false|auto]"},
        {NULL}
    };

    CUT_UI_FACTORY_CLASS(parent_class)->set_option_group(factory, context);

    group = g_option_group_new(("ui-console"),
                               _("Console UI Options"),
                               _("Show Console UI Options"),
                               console, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_parse_hooks(group, pre_parse, NULL);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

CutUI *
create (CutUIFactory *factory)
{
    CutUIFactoryConsole *console = CUT_UI_FACTORY_CONSOLE(factory);

    return cut_ui_new("console",
                      "use-color", console->use_color,
                      "verbose-level", console->verbose_level,
                      NULL);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
