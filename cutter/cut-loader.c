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

#ifdef HAVE_LIBBFD
#  include <bfd.h>
#endif

#include "cut-loader.h"

#define CUT_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_LOADER, CutLoaderPrivate))

typedef struct _CutLoaderPrivate	CutLoaderPrivate;
struct _CutLoaderPrivate
{
    gchar *so_filename;
    GModule *module;
};

enum
{
    PROP_0,
    PROP_SO_FILENAME
};

G_DEFINE_TYPE (CutLoader, cut_loader, G_TYPE_OBJECT)

static void dispose         (GObject               *object);
static void set_property    (GObject               *object,
                             guint                  prop_id,
                             const GValue          *value,
                             GParamSpec            *pspec);
static void get_property    (GObject               *object,
                             guint                  prop_id,
                             GValue                *value,
                             GParamSpec            *pspec);

static void
cut_loader_class_init (CutLoaderClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("so-filename",
                               ".so filename",
                               "The filename of shared object",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SO_FILENAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutLoaderPrivate));
}

static void
cut_loader_init (CutLoader *loader)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
}

static void
dispose (GObject *object)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

    if (priv->module) {
        g_module_close(priv->module);
        priv->module = NULL;
    }

    G_OBJECT_CLASS(cut_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SO_FILENAME:
        if (priv->so_filename)
            g_free(priv->so_filename);
        priv->so_filename = g_value_dup_string(value);
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
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutLoader *
cut_loader_new (const gchar *soname)
{
    return g_object_new(CUT_TYPE_LOADER,
                        "so-filename", soname,
                        NULL);
}

static inline gboolean
is_test_function_name (const gchar *name)
{
    return name && g_str_has_prefix(name, "test_");
}

#ifdef HAVE_LIBBFD
static GList *
collect_test_functions (CutLoaderPrivate *priv)
{
    GList *test_names = NULL;
    long storage_needed;
    asymbol **symbol_table;
    long number_of_symbols;
    long i;
    bfd *abfd;

    abfd = bfd_openr(priv->so_filename, NULL);
    if (!abfd)
        return NULL;

    if (!bfd_check_format(abfd, bfd_object)) {
        bfd_close(abfd);
        return NULL;
    }

    storage_needed = bfd_get_symtab_upper_bound(abfd);
    if (storage_needed <= 0) {
        bfd_close(abfd);
        return NULL;
    }

    symbol_table = (asymbol **)g_new(char, storage_needed);
    number_of_symbols = bfd_canonicalize_symtab(abfd, symbol_table);

    for (i = 0; i < number_of_symbols; i++) {
        symbol_info info;

        bfd_symbol_info(symbol_table[i], &info);
        if (info.type == 'T' && is_test_function_name(info.name)) {
            test_names = g_list_prepend(test_names, g_strdup(info.name));
        }
    }

    g_free(symbol_table);
    bfd_close(abfd);

    return test_names;
}
#else
static inline gboolean
is_test_function_name_consisted_of (char c)
{
    return g_ascii_isalnum(c) || '_' == c;
}

static inline gboolean
is_test_function_name_string (GString *name)
{
    return name->len > 4 && is_test_function_name(name->str);
}

static GList *
collect_test_functions (CutLoaderPrivate *priv)
{
    FILE *input;
    GString *name;
    char buffer[4096];
    size_t size;
    GList *test_names;
    GHashTable *test_name_table;

    input = g_fopen(priv->so_filename, "rb");
    if (!input)
        return NULL;

    test_name_table = g_hash_table_new(g_str_hash, g_str_equal);
    name = g_string_new("");
    while ((size = fread(buffer, sizeof(*buffer), sizeof(buffer), input)) > 0) {
        size_t i;
        for (i = 0; i < size; i++) {
            if (is_test_function_name_consisted_of(buffer[i])) {
                g_string_append_c(name, buffer[i]);
            } else if (name->len) {
                if (is_test_function_name_string(name)) {
                    g_hash_table_insert(test_name_table,
                                        g_strdup(name->str), NULL);
                }
                g_string_truncate(name, 0);
            }
        }
    }

    if (is_test_function_name_string(name)) {
        g_hash_table_insert(test_name_table, g_strdup(name->str), NULL);
    }
    g_string_free(name, TRUE);

    test_names = g_hash_table_get_keys(test_name_table);
    g_hash_table_unref(test_name_table);

    fclose(input);

    return test_names;
}
#endif

static void
cb_complete (CutTestCase *test_case, gpointer data)
{
    CutLoaderPrivate *priv = data;

    if (priv->module) {
        g_module_close(priv->module);
        priv->module = NULL;
    }
}

static CutTestCase *
create_test_case (CutLoaderPrivate *priv)
{
    CutTestCase *test_case;
    CutSetupFunction setup_function = NULL;
    CutGetCurrentTestContextFunction get_current_test_context_function = NULL;
    CutSetCurrentTestContextFunction set_current_test_context_function = NULL;
    CutTearDownFunction teardown_function = NULL;
    gchar *test_case_name, *filename;

    g_module_symbol(priv->module,
                    "setup",
                    (gpointer)&setup_function);
    g_module_symbol(priv->module,
                    "teardown",
                    (gpointer)&teardown_function);
    g_module_symbol(priv->module,
                    "get_current_test_context",
                    (gpointer)&get_current_test_context_function);
    g_module_symbol(priv->module,
                    "set_current_test_context",
                    (gpointer)&set_current_test_context_function);

    filename = g_path_get_basename(priv->so_filename);
    if (g_str_has_prefix(filename, "lib")) {
        gchar *string;
        string = g_strdup(filename + strlen("lib"));
        g_free(filename);
        filename = string;
    }
    test_case_name = g_strndup(filename,
                               strlen(filename) - strlen(G_MODULE_SUFFIX) -1);
    g_free(filename);
    test_case = cut_test_case_new(test_case_name,
                                  setup_function,
                                  teardown_function,
                                  get_current_test_context_function,
                                  set_current_test_context_function);
    g_free(test_case_name);

    g_signal_connect(test_case, "complete", G_CALLBACK(cb_complete), priv);

    return test_case;
}

CutTestCase *
cut_loader_load_test_case (CutLoader *loader)
{
    GList *node;;
    GList *test_names;
    CutTestCase *test_case;
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(loader);

    if (!priv->so_filename)
        return NULL;

    priv->module = g_module_open(priv->so_filename,
                                 G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
    if (!priv->module)
        return NULL;

    test_names = collect_test_functions(priv);
    if (!test_names)
        return NULL;

    test_case = create_test_case(priv);
    for (node = test_names; node; node = g_list_next(node)) {
        gchar *name;
        CutTest *test;
        CutTestFunction function = NULL;

        name = node->data;
        g_module_symbol(priv->module, name, (gpointer)&function);
        if (function) {
            test = cut_test_new(name, NULL, function);
            cut_test_case_add_test(test_case, test);
        }
        g_free(name);
    }
    g_list_free(test_names);

    return test_case;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
