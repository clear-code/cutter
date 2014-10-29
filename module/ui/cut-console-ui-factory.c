/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2011  Kouhei Sutou <kou@clear-code.com>
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

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-console.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_CONSOLE_UI_FACTORY            cut_type_console_ui_factory
#define CUT_CONSOLE_UI_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_CONSOLE_UI_FACTORY, CutConsoleUIFactory))
#define CUT_CONSOLE_UI_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_CONSOLE_UI_FACTORY, CutConsoleUIFactoryClass))
#define CUT_IS_CONSOLE_UI_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_CONSOLE_UI_FACTORY))
#define CUT_IS_CONSOLE_UI_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_CONSOLE_UI_FACTORY))
#define CUT_CONSOLE_UI_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_CONSOLE_UI_FACTORY, CutConsoleUIFactoryClass))

typedef struct _CutConsoleUIFactory CutConsoleUIFactory;
typedef struct _CutConsoleUIFactoryClass CutConsoleUIFactoryClass;

struct _CutConsoleUIFactory
{
    CutModuleFactory     object;

    gboolean             use_color;
    CutVerboseLevel      verbose_level;
    gchar               *notify_command;
    gboolean             show_detail_immediately;
};

struct _CutConsoleUIFactoryClass
{
    CutModuleFactoryClass parent_class;
};

enum
{
    PROP_0,
    PROP_USE_COLOR,
    PROP_VERBOSE_LEVEL,
    PROP_SHOW_DETAIL_IMMEDIATELY
};

static GType cut_type_console_ui_factory = 0;
static CutModuleFactoryClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void       set_option_group (CutModuleFactory    *factory,
                                    GOptionContext      *context);
static GObject   *create           (CutModuleFactory    *factory);

static void
class_init (CutModuleFactoryClass *klass)
{
    GObjectClass *gobject_class;
    CutModuleFactoryClass *factory_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class  = CUT_MODULE_FACTORY_CLASS(klass);

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

    spec = g_param_spec_boolean("show-detail-immediately",
                                "Show Detail Immediately",
                                "Whether shows test details immediately.",
                                TRUE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SHOW_DETAIL_IMMEDIATELY,
                                    spec);
}

static void
init (CutConsoleUIFactory *console)
{
    console->use_color = FALSE;
    console->verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
    console->notify_command = NULL;
    console->show_detail_immediately = TRUE;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutConsoleUIFactoryClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutConsoleUIFactory),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_console_ui_factory =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_MODULE_FACTORY,
                                    "CutConsoleUIFactory",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_console_ui_factory)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_console_ui_factory));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_CONSOLE_UI_FACTORY, first_property, var_args);
}

static void
dispose (GObject *object)
{
    CutConsoleUIFactory *console = CUT_CONSOLE_UI_FACTORY(object);

    if (console->notify_command) {
        g_free(console->notify_command);
        console->notify_command = NULL;
    }
    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutConsoleUIFactory *console = CUT_CONSOLE_UI_FACTORY(object);

    switch (prop_id) {
    case PROP_USE_COLOR:
        console->use_color = g_value_get_boolean(value);
        break;
    case PROP_VERBOSE_LEVEL:
        console->verbose_level = g_value_get_enum(value);
        break;
    case PROP_SHOW_DETAIL_IMMEDIATELY:
        console->show_detail_immediately = g_value_get_boolean(value);
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
    CutConsoleUIFactory *console = CUT_CONSOLE_UI_FACTORY(object);

    switch (prop_id) {
    case PROP_USE_COLOR:
        g_value_set_boolean(value, console->use_color);
        break;
    case PROP_VERBOSE_LEVEL:
        g_value_set_enum(value, console->verbose_level);
        break;
    case PROP_SHOW_DETAIL_IMMEDIATELY:
        g_value_set_boolean(value, console->show_detail_immediately);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gint
guess_term_width (void)
{
    gint term_width = 0;
    const gchar *columns_env;
    const gchar *term_width_env;

    columns_env = g_getenv("COLUMNS");
    if (columns_env)
        term_width = atoi(columns_env);

    if (term_width == 0) {
        term_width_env = g_getenv("TERM_WIDTH");
        if (term_width_env)
            term_width = atoi(term_width_env);
    }

    if (term_width < 0)
        return -1;

    if (term_width == 0) {
        const gchar *emacs_env;

        emacs_env = g_getenv("EMACS");
        if (emacs_env && (g_str_equal(emacs_env, "t")))
            term_width = -1;
        else
            term_width = 79;
    }

    return term_width;
}

static gboolean
parse_verbose_level_arg (const gchar *option_name, const gchar *value,
                         gpointer data, GError **error)
{
    CutConsoleUIFactory *console = data;
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
    CutConsoleUIFactory *console = data;

    return cut_console_parse_color_arg(option_name, value,
                                       &(console->use_color), error);
}

static gboolean
program_exist (const gchar *program)
{
    gchar *path;
    gboolean found = FALSE;

    path = g_find_program_in_path(program);
    if (path) {
        found = TRUE;
        g_free(path);
    }
    return found;
}

static const gchar *
search_notify_command (void)
{
    const gchar *notify_send = "notify-send";
    const gchar *growlnotify = "growlnotify";
    const gchar *terminal_notifier = "terminal-notifier";

    if (program_exist(notify_send) &&
        g_getenv("DBUS_SESSION_BUS_ADDRESS")) {
        return notify_send;
    }

    if (program_exist(growlnotify)) {
        return growlnotify;
    }

    if (program_exist(terminal_notifier)) {
        return terminal_notifier;
    }

    return NULL;
}

static gboolean
parse_notify_arg (const gchar *option_name, const gchar *value,
                  gpointer data, GError **error)
{
    CutConsoleUIFactory *console = data;

    if (value == NULL ||
        g_utf8_collate(value, "yes") == 0 ||
        g_utf8_collate(value, "true") == 0 ||
        g_utf8_collate(value, "auto") == 0) {
        if (!console->notify_command) {
            console->notify_command = g_strdup(search_notify_command());
        }
    } else if (g_utf8_collate(value, "no") == 0 ||
               g_utf8_collate(value, "false") == 0) {
        if (console->notify_command) {
            g_free(console->notify_command);
            console->notify_command = NULL;
        }
    } else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_BAD_VALUE,
                    _("Invalid notify value: %s"), value);
        return FALSE;
    }

    return TRUE;
}

static gboolean
parse_show_detail_immediately (const gchar *option_name, const gchar *value,
                               gpointer data, GError **error)
{
    CutConsoleUIFactory *console = data;

    if (value == NULL ||
        g_utf8_collate(value, "yes") == 0 ||
        g_utf8_collate(value, "true") == 0) {
        console->show_detail_immediately = TRUE;
    } else if (g_utf8_collate(value, "no") == 0 ||
               g_utf8_collate(value, "false") == 0) {
        console->show_detail_immediately = FALSE;
    } else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_BAD_VALUE,
                    _("Invalid boolean value: %s"), value);
        return FALSE;
    }

    return TRUE;
}

static gboolean
pre_parse (GOptionContext *context, GOptionGroup *group, gpointer data,
           GError **error)
{
    CutConsoleUIFactory *console = data;

    console->use_color = cut_console_guess_color_usability();
    console->notify_command = g_strdup(search_notify_command());

    return TRUE;
}

static void
set_option_group (CutModuleFactory *factory, GOptionContext *context)
{
    CutConsoleUIFactory *console = CUT_CONSOLE_UI_FACTORY(factory);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {"verbose", 'v', 0, G_OPTION_ARG_CALLBACK, parse_verbose_level_arg,
         N_("Set verbose level"), "[s|silent|n|normal|v|verbose]"},
        {"color", 'c', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
         parse_color_arg, N_("Output log with colors"),
         "[yes|true|no|false|auto]"},
        {"notify", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
         parse_notify_arg, N_("Notify test result"), "[yes|true|no|false|auto]"},
        {"show-detail-immediately", 0, G_OPTION_FLAG_OPTIONAL_ARG,
         G_OPTION_ARG_CALLBACK, parse_show_detail_immediately,
         N_("Show test detail immediately"), "[yes|true|no|false]"},
        {NULL}
    };

    if (CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group)
        CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group(factory, context);

    group = g_option_group_new(("console-ui"),
                               _("Console UI Options"),
                               _("Show console UI options"),
                               console, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_parse_hooks(group, pre_parse, NULL);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

GObject *
create (CutModuleFactory *factory)
{
    CutConsoleUIFactory *console = CUT_CONSOLE_UI_FACTORY(factory);

    return G_OBJECT(cut_ui_new("console",
                               "use-color", console->use_color,
                               "verbose-level", console->verbose_level,
                               "notify-command", console->notify_command,
                               "progress-row-max", guess_term_width(),
                               "show-detail-immediately",
                               console->show_detail_immediately,
                               NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
