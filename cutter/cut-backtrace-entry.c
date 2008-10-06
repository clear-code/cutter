/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-backtrace-entry.h"
#include "cut-utils.h"

#define CUT_BACKTRACE_ENTRY_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_BACKTRACE_ENTRY, CutBacktraceEntryPrivate))

typedef struct _CutBacktraceEntryPrivate	CutBacktraceEntryPrivate;
struct _CutBacktraceEntryPrivate
{
    gchar *file;
    guint line;
    gchar *function;
    gchar *info;
};

enum
{
    PROP_0,
    PROP_FILE,
    PROP_LINE,
    PROP_FUNCTION,
    PROP_INFO
};


G_DEFINE_TYPE(CutBacktraceEntry, cut_backtrace_entry, G_TYPE_OBJECT)

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
cut_backtrace_entry_class_init (CutBacktraceEntryClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("file",
                               "File",
                               "The file name of the backtrace entry",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FILE, spec);

    spec = g_param_spec_uint("line",
                             "Line number",
                             "The line number of the backtrace entry",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LINE, spec);

    spec = g_param_spec_string("function",
                               "Function",
                               "The function name of the backtrace entry",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FUNCTION, spec);

    spec = g_param_spec_string("info",
                               "Information",
                               "The information of the backtrace entry",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_INFO, spec);

    g_type_class_add_private(gobject_class, sizeof(CutBacktraceEntryPrivate));
}

static void
cut_backtrace_entry_init (CutBacktraceEntry *entry)
{
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry);
    priv->file = NULL;
    priv->line = 0;
    priv->function = NULL;
    priv->info = NULL;
}

static void
dispose (GObject *object)
{
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(object);
    if (priv->file) {
        g_free(priv->file);
        priv->file = NULL;
    }

    if (priv->function) {
        g_free(priv->function);
        priv->function = NULL;
    }

    if (priv->info) {
        g_free(priv->info);
        priv->info = NULL;
    }

    G_OBJECT_CLASS(cut_backtrace_entry_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(object);
    switch (prop_id) {
      case PROP_FILE:
        cut_backtrace_entry_set_file(CUT_BACKTRACE_ENTRY(object),
                                     g_value_get_string(value));
        break;
      case PROP_LINE:
        priv->line = g_value_get_uint(value);
        break;
      case PROP_FUNCTION:
        cut_backtrace_entry_set_function(CUT_BACKTRACE_ENTRY(object),
                                         g_value_get_string(value));
        break;
      case PROP_INFO:
        cut_backtrace_entry_set_info(CUT_BACKTRACE_ENTRY(object),
                                     g_value_get_string(value));
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
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(object);
    switch (prop_id) {
      case PROP_FILE:
        g_value_set_string(value, priv->file);
        break;
      case PROP_LINE:
        g_value_set_uint(value, priv->line);
        break;
      case PROP_FUNCTION:
        g_value_set_string(value, priv->function);
        break;
      case PROP_INFO:
        g_value_set_string(value, priv->info);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutBacktraceEntry *
cut_backtrace_entry_new (const gchar *file, guint line,
                         const gchar *function, const gchar *info)
{
    return g_object_new(CUT_TYPE_BACKTRACE_ENTRY,
                        "file", file,
                        "line", line,
                        "function", function,
                        "info", info,
                        NULL);
}

CutBacktraceEntry *
cut_backtrace_entry_new_empty (void)
{
    return cut_backtrace_entry_new(NULL, 0, NULL, NULL);
}

const gchar *
cut_backtrace_entry_get_file (CutBacktraceEntry *entry)
{
    return CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry)->file;
}

void
cut_backtrace_entry_set_file (CutBacktraceEntry *entry, const gchar *file)
{
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry);
    if (priv->file) {
        g_free(priv->file);
        priv->file = NULL;
    }
    if (file)
        priv->file = g_strdup(file);
}

guint
cut_backtrace_entry_get_line (CutBacktraceEntry *entry)
{
    return CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry)->line;
}

void
cut_backtrace_entry_set_line (CutBacktraceEntry *entry, guint line)
{
    CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry)->line = line;
}

const gchar *
cut_backtrace_entry_get_function (CutBacktraceEntry *entry)
{
    return CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry)->function;
}

void
cut_backtrace_entry_set_function (CutBacktraceEntry *entry,
                                  const gchar *function)
{
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry);
    if (priv->function) {
        g_free(priv->function);
        priv->function = NULL;
    }
    if (function)
        priv->function = g_strdup(function);
}

const gchar *
cut_backtrace_entry_get_info (CutBacktraceEntry *entry)
{
    return CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry)->info;
}

void
cut_backtrace_entry_set_info (CutBacktraceEntry *entry, const gchar *info)
{
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry);
    if (priv->info) {
        g_free(priv->info);
        priv->info = NULL;
    }
    if (info)
        priv->info = g_strdup(info);
}

gchar *
cut_backtrace_entry_to_xml (CutBacktraceEntry *entry)
{
    GString *string;

    string = g_string_new(NULL);
    cut_backtrace_entry_to_xml_string(entry, string, 0);
    return g_string_free(string, FALSE);
}

static void
append_element_valist (GString *string, guint indent,
                       const gchar *element_name, va_list var_args)
{
    const gchar *name;

    name = element_name;

    while (name) {
        const gchar *value = va_arg(var_args, gchar *);
        if (value)
            cut_utils_append_xml_element_with_value(string, indent, name, value);
        name = va_arg(var_args, gchar *);
    }
}

static void
append_element_with_children (GString *string, guint indent,
                              const gchar *element_name,
                              const gchar *first_child_element, ...)
{
    gchar *escaped;
    va_list var_args;

    escaped = g_markup_escape_text(element_name, -1);
    cut_utils_append_indent(string, indent);
    g_string_append_printf(string, "<%s>\n", escaped);

    va_start(var_args, first_child_element);
    append_element_valist(string, indent + 2, first_child_element, var_args);
    va_end(var_args);

    cut_utils_append_indent(string, indent);
    g_string_append_printf(string, "</%s>\n", escaped);
    g_free(escaped);
}

void
cut_backtrace_entry_to_xml_string (CutBacktraceEntry *entry, GString *string,
                                   guint indent)
{
    CutBacktraceEntryPrivate *priv;
    gchar *line_string = NULL;
    gchar *info_string = NULL;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry);
    if (priv->file == NULL && priv->function == NULL)
        return;

    if (priv->line > 0)
        line_string = g_strdup_printf("%d", priv->line);
    if (priv->function)
        info_string = g_strdup_printf("%s()", priv->function);
    if (priv->info) {
        if (info_string) {
            gchar *function_string = info_string;
            info_string = g_strconcat(function_string, ": ", priv->info, NULL);
            g_free(function_string);
        } else {
            info_string = g_strdup(priv->info);
        }
    }

    append_element_with_children(string, indent, "entry",
                                 "file", priv->file,
                                 "line", line_string,
                                 "info", info_string,
                                 NULL);

    if (line_string)
        g_free(line_string);
    if (info_string)
        g_free(info_string);
}

gchar *
cut_backtrace_entry_format (CutBacktraceEntry *entry)
{
    GString *string;

    string = g_string_new(NULL);
    cut_backtrace_entry_format_string(entry, string);
    return g_string_free(string, FALSE);
}

void
cut_backtrace_entry_format_string (CutBacktraceEntry *entry, GString *string)
{
    CutBacktraceEntryPrivate *priv;

    priv = CUT_BACKTRACE_ENTRY_GET_PRIVATE(entry);

    g_string_append_printf(string,
                           "%s:%d",
                           priv->file ? priv->file : "(null)",
                           priv->line);
    if (priv->function)
        g_string_append_printf(string, ": %s()", priv->function);
    if (priv->info)
        g_string_append_printf(string, ": %s", priv->info);
    if (priv->function == NULL && priv->info == NULL)
        g_string_append(string, ":");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
