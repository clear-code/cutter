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
#include "cut-experimental.h"

#define TEST_NAME_PREFIX "test_"
#define METADATA_PREFIX "meta_"
#define CUT_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_LOADER, CutLoaderPrivate))

typedef struct _CutLoaderPrivate	CutLoaderPrivate;
struct _CutLoaderPrivate
{
    gchar *so_filename;
    GList *symbols;
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
free_symbols (CutLoaderPrivate *priv)
{
    if (priv->symbols) {
        g_list_foreach(priv->symbols, (GFunc)g_free, NULL);
        g_list_free(priv->symbols);
        priv->symbols = NULL;
    }
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

    free_symbols(priv);

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
    return name && g_str_has_prefix(name, TEST_NAME_PREFIX);
}

#ifdef HAVE_LIBBFD
static GList *
collect_symbols (CutLoaderPrivate *priv)
{
    GList *symbols = NULL;
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
        if (info.type == 'T') {
            symbols = g_list_prepend(symbols, g_strdup(info.name));
        }
    }

    g_free(symbol_table);
    bfd_close(abfd);

    return symbols;
}

#else

static inline gboolean
is_valid_char_for_cutter_symbol (char c)
{
    return g_ascii_isalnum(c) || '_' == c;
}

static GList *
collect_symbols (CutLoaderPrivate *priv)
{
    FILE *input;
    GString *name;
    char buffer[4096];
    size_t size;
    GHashTable *symbol_name_table;
    GList *symbols;

    input = g_fopen(priv->so_filename, "rb");
    if (!input)
        return NULL;

    symbol_name_table = g_hash_table_new(g_str_hash, g_str_equal);
    name = g_string_new("");
    while ((size = fread(buffer, sizeof(*buffer), sizeof(buffer), input)) > 0) {
        size_t i;
        for (i = 0; i < size; i++) {
            if (is_valid_char_for_cutter_symbol(buffer[i])) {
                g_string_append_c(name, buffer[i]);
            } else if (name->len > 0) {
                if (name->len > 1) {
                    g_hash_table_insert(symbol_name_table,
                                        g_strdup(name->str), NULL);
                }
                g_string_truncate(name, 0);
            }
        }
    }

    g_hash_table_insert(symbol_name_table, g_strdup(name->str), NULL);
    g_string_free(name, TRUE);

    symbols = g_hash_table_get_keys(symbol_name_table);
    g_hash_table_unref(symbol_name_table);

    fclose(input);

    return symbols;
}
#endif

static GList *
collect_test_functions (CutLoaderPrivate *priv)
{
    GList *node, *test_names = NULL;

    for (node = priv->symbols; node; node = g_list_next(node)) {
        gchar *name = node->data;
        if (is_test_function_name(name)) {
            test_names = g_list_prepend(test_names, name);
        }
    }
    return test_names;
}

static gboolean
is_including_test_name (const gchar *function_name, const gchar *test_name)
{
    return (strlen(function_name) > strlen(test_name) - strlen(TEST_NAME_PREFIX)) &&
           g_str_has_suffix(function_name, test_name + strlen(TEST_NAME_PREFIX));
}

static gboolean
is_valid_metadata_item_function_name (const gchar *function_name, const gchar *test_name)
{
    return !g_str_has_prefix(function_name, METADATA_PREFIX) && 
           is_including_test_name(function_name, test_name);
}

static gboolean
is_valid_metadata_function_name (const gchar *function_name, const gchar *test_name)
{
    return g_str_has_prefix(function_name, METADATA_PREFIX) &&
           is_including_test_name(function_name, test_name);
}

static gchar *
get_metadata_name (const gchar *metadata_function_name, const gchar *test_name)
{
    gchar *pos;

    pos = g_strrstr(metadata_function_name, test_name + strlen(TEST_NAME_PREFIX)) - 1;

    return g_strndup(metadata_function_name, pos - metadata_function_name);
}

typedef const gchar *(*CutMetadataItemFunction)     (void);
typedef CutTestMetadata *(*CutMetadataFunction)     (void);

static CutTestMetadata *
cut_test_metadata_new (const gchar *name, const gchar *value)
{
    CutTestMetadata *metadata = g_new0(CutTestMetadata, 1);
    metadata->name = g_strdup(name);
    metadata->value = g_strdup(value);
    return metadata;
}

static GList *
collect_metadata (CutLoaderPrivate *priv, const gchar *test_name)
{
    GList *metadata_list = NULL, *node;
    if (!test_name)
        return NULL;

    for (node = priv->symbols; node; node = g_list_next(node)) {
        gchar *function_name = node->data;
        if (is_test_function_name(function_name))
            continue;
        if (is_valid_metadata_item_function_name(function_name, test_name)) {
            CutMetadataItemFunction function = NULL;
            g_module_symbol(priv->module, function_name, (gpointer)&function);
            if (function) {
                CutTestMetadata *metadata;
                gchar *name = get_metadata_name(function_name, test_name);
                const gchar *value = function();
                metadata = cut_test_metadata_new(name, value);
                g_free(name);
                metadata_list = g_list_append(metadata_list, metadata);
            }
        } else if (is_valid_metadata_function_name(function_name, test_name)) {
            CutMetadataFunction function = NULL;
            g_module_symbol(priv->module, function_name, (gpointer)&function);
            if (function) {
                CutTestMetadata *metadata = function();
                while (TRUE) {
                    CutTestMetadata *new_metadata;
                    if (!metadata->name || !metadata->value)
                        break;
                    new_metadata = cut_test_metadata_new(metadata->name, metadata->value);
                    metadata_list = g_list_append(metadata_list, new_metadata);
                    metadata++;
                }
            }
        }
    }
    return metadata_list;
}

static void
cb_complete (CutTestCase *test_case, gpointer data)
{
    CutLoader *loader = data;
    g_object_unref(loader);
}

static CutTestCase *
create_test_case (CutLoader *loader)
{
    CutLoaderPrivate *priv;
    CutTestCase *test_case;
    CutSetupFunction setup_function = NULL;
    CutGetCurrentTestContextFunction get_current_test_context_function = NULL;
    CutSetCurrentTestContextFunction set_current_test_context_function = NULL;
    CutTearDownFunction teardown_function = NULL;
    gchar *test_case_name, *filename;

    priv = CUT_LOADER_GET_PRIVATE(loader);

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

    g_object_ref(loader);
    g_signal_connect(test_case, "complete", G_CALLBACK(cb_complete), loader);

    return test_case;
}

static void
set_metadata (CutTest *test, GList *metadata_list)
{
    GList *node;
    for (node = metadata_list; node; node = g_list_next(node)) {
        CutTestMetadata *metadata = (CutTestMetadata *)node->data;
        cut_test_set_metadata(test, metadata->name, metadata->value);
        g_free((gchar *)metadata->name);
        g_free((gchar *)metadata->value);
    }
    g_list_free(metadata_list);
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

    priv->symbols = collect_symbols(priv);
    if (!priv->symbols)
        return NULL;

    test_names = collect_test_functions(priv);
    if (!test_names)
        return NULL;

    test_case = create_test_case(loader);
    for (node = test_names; node; node = g_list_next(node)) {
        gchar *name;
        CutTest *test;
        CutTestFunction function = NULL;

        name = node->data;
        g_module_symbol(priv->module, name, (gpointer)&function);
        if (function) {
            GList *metadata_list;
            test = cut_test_new(name, NULL, function);
            metadata_list = collect_metadata(priv, name);
            if (metadata_list) {
                set_metadata(test, metadata_list);
            }
            cut_test_case_add_test(test_case, test);
        }
    }
    g_list_free(test_names);

    return test_case;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
