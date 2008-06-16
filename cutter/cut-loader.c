/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gmodule.h>

#ifdef HAVE_LIBBFD
#  include <bfd.h>
#else
#  include <glib/gstdio.h>
#endif

#include "cut-loader.h"
#include "cut-experimental.h"

#define TEST_SUITE_SO_NAME_PREFIX "suite_"
#define TEST_NAME_PREFIX "test_"
#define ATTRIBUTE_PREFIX "attributes_"
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

typedef enum {
    CUT_BINARY_TYPE_UNKNOWN,
    CUT_BINARY_TYPE_MACH_O_BUNDLE
} CutBinaryType;

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
collect_symbols (CutLoaderPrivate *priv, CutBinaryType *binary_type)
{
    GList *symbols = NULL;
    long storage_needed;
    asymbol **symbol_table;
    long number_of_symbols;
    long i;
    char symbol_leading_char;
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

    symbol_leading_char = bfd_get_symbol_leading_char(abfd);
    if (bfd_get_flavour(abfd) == bfd_target_mach_o_flavour)
        *binary_type = CUT_BINARY_TYPE_MACH_O_BUNDLE;

    for (i = 0; i < number_of_symbols; i++) {
        symbol_info info;

        bfd_symbol_info(symbol_table[i], &info);
        if (info.type == 'T' ||
            (*binary_type == CUT_BINARY_TYPE_MACH_O_BUNDLE &&
             info.type == 'U')) {
            const char *name = info.name;

            while (symbol_leading_char == name[0])
                name++;
            symbols = g_list_prepend(symbols, g_strdup(name));
        }
    }

    g_free(symbol_table);
    bfd_close(abfd);

    return symbols;
}

#else

static inline CutBinaryType
guess_binary_type (char *buffer, size_t size)
{
    if (size >= 4 && ((unsigned int *)buffer)[0] == 0xfeedface)
        return CUT_BINARY_TYPE_MACH_O_BUNDLE;
    return CUT_BINARY_TYPE_UNKNOWN;
}

static inline gboolean
is_valid_char_for_cutter_symbol (GString *name, char *buffer,
                                 int i, size_t size, CutBinaryType binary_type)
{
    char c;

    c = buffer[i];
    if (binary_type == CUT_BINARY_TYPE_MACH_O_BUNDLE) {
        if (name->len == 0 && i > 0 && buffer[i - 1] == '\0' && c == '_')
            return FALSE;
    }

    return g_ascii_isalnum(c) || '_' == c;
}

static inline gboolean
is_valid_symbol_name (GString *name)
{
    return name->len > 1;
}

static GList *
collect_symbols (CutLoaderPrivate *priv, CutBinaryType *binary_type)
{
    FILE *input;
    GString *name;
    char buffer[4096];
    size_t size;
    GHashTable *symbol_name_table;
    GList *symbols;
    gboolean first_buffer = TRUE;

    input = g_fopen(priv->so_filename, "rb");
    if (!input)
        return NULL;

    symbol_name_table = g_hash_table_new(g_str_hash, g_str_equal);
    name = g_string_new("");
    while ((size = fread(buffer, sizeof(*buffer), sizeof(buffer), input)) > 0) {
        size_t i;

        if (first_buffer) {
            *binary_type = guess_binary_type(buffer, size);
            first_buffer = FALSE;
        }

        for (i = 0; i < size; i++) {
            if (is_valid_char_for_cutter_symbol(name, buffer, i,
                                                size, *binary_type)) {
                g_string_append_c(name, buffer[i]);
            } else if (name->len > 0) {
                if (is_valid_symbol_name(name)) {
                    g_hash_table_insert(symbol_name_table,
                                        g_strdup(name->str), NULL);
                }
                g_string_truncate(name, 0);
            }
        }
    }

    if (is_valid_symbol_name(name))
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
is_valid_attribute_function_name (const gchar *function_name, const gchar *test_name)
{
    return !g_str_has_prefix(function_name, ATTRIBUTE_PREFIX) &&
        is_including_test_name(function_name, test_name);
}

static gboolean
is_valid_attributes_function_name (const gchar *function_name, const gchar *test_name)
{
    return g_str_has_prefix(function_name, ATTRIBUTE_PREFIX) &&
        is_including_test_name(function_name, test_name);
}

static gchar *
get_attribute_name (const gchar *attribute_function_name, const gchar *test_name)
{
    gchar *pos;

    pos = g_strrstr(attribute_function_name, test_name + strlen(TEST_NAME_PREFIX)) - 1;

    return g_strndup(attribute_function_name, pos - attribute_function_name);
}

typedef const gchar *(*CutAttributeItemFunction)     (void);
typedef CutTestAttribute *(*CutAttributeFunction)     (void);

static CutTestAttribute *
cut_test_attribute_new (const gchar *name, const gchar *value)
{
    CutTestAttribute *attribute = g_new0(CutTestAttribute, 1);
    attribute->name = g_strdup(name);
    attribute->value = g_strdup(value);
    return attribute;
}

static GList *
collect_attributes (CutLoaderPrivate *priv, const gchar *test_name)
{
    GList *attributes = NULL, *node;
    if (!test_name)
        return NULL;

    for (node = priv->symbols; node; node = g_list_next(node)) {
        gchar *function_name = node->data;
        if (is_test_function_name(function_name))
            continue;
        if (is_valid_attribute_function_name(function_name, test_name)) {
            CutAttributeItemFunction function = NULL;
            g_module_symbol(priv->module, function_name, (gpointer)&function);
            if (function) {
                CutTestAttribute *attribute;
                gchar *name = get_attribute_name(function_name, test_name);
                const gchar *value = function();
                attribute = cut_test_attribute_new(name, value);
                g_free(name);
                attributes = g_list_append(attributes, attribute);
            }
        } else if (is_valid_attributes_function_name(function_name, test_name)) {
            CutAttributeFunction function = NULL;
            g_module_symbol(priv->module, function_name, (gpointer)&function);
            if (function) {
                CutTestAttribute *attribute = function();
                while (TRUE) {
                    CutTestAttribute *new_attribute;
                    if (!attribute->name || !attribute->value)
                        break;
                    new_attribute = cut_test_attribute_new(attribute->name, attribute->value);
                    attributes = g_list_append(attributes, new_attribute);
                    attribute++;
                }
            }
        }
    }
    return attributes;
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
    CutSetupFunction setup = NULL;
    CutGetCurrentTestContextFunction get_current_test_context = NULL;
    CutSetCurrentTestContextFunction set_current_test_context = NULL;
    CutStartupFunction startup = NULL;
    CutShutdownFunction shutdown = NULL;
    CutTeardownFunction teardown = NULL;
    gchar *test_case_name, *filename;

    priv = CUT_LOADER_GET_PRIVATE(loader);

    g_module_symbol(priv->module, "setup", (gpointer)&setup);
    g_module_symbol(priv->module, "teardown", (gpointer)&teardown);
    g_module_symbol(priv->module, "get_current_test_context",
                    (gpointer)&get_current_test_context);
    g_module_symbol(priv->module, "set_current_test_context",
                    (gpointer)&set_current_test_context);
    g_module_symbol(priv->module, "startup", (gpointer)&startup);
    g_module_symbol(priv->module, "shutdown", (gpointer)&shutdown);

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
                                  setup, teardown,
                                  get_current_test_context,
                                  set_current_test_context,
                                  startup, shutdown);
    g_free(test_case_name);

    g_object_ref(loader);
    g_signal_connect(test_case, "complete", G_CALLBACK(cb_complete), loader);

    return test_case;
}

static void
set_attributes (CutTest *test, GList *attributes)
{
    GList *node;
    for (node = attributes; node; node = g_list_next(node)) {
        CutTestAttribute *attribute = (CutTestAttribute *)node->data;
        cut_test_set_attribute(test, attribute->name, attribute->value);
        g_free((gchar *)attribute->name);
        g_free((gchar *)attribute->value);
        g_free(attribute);
    }
    g_list_free(attributes);
}

CutTestCase *
cut_loader_load_test_case (CutLoader *loader)
{
    GList *node;
    GList *test_names;
    CutTestCase *test_case;
    CutBinaryType binary_type = CUT_BINARY_TYPE_UNKNOWN;
    CutLoaderPrivate *priv;

    priv = CUT_LOADER_GET_PRIVATE(loader);
    if (!priv->so_filename)
        return NULL;

    priv->module = g_module_open(priv->so_filename,
                                 G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
    if (!priv->module)
        return NULL;

    priv->symbols = collect_symbols(priv, &binary_type);
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
            GList *attributes = NULL;
            test = cut_test_new(name, function);
            if (binary_type != CUT_BINARY_TYPE_MACH_O_BUNDLE)
                attributes = collect_attributes(priv, name);
            if (attributes) {
                set_attributes(test, attributes);
            }
            cut_test_case_add_test(test_case, test);
            g_object_unref(test);
        }
    }
    g_list_free(test_names);

    return test_case;
}

static gchar *
get_suite_prefix (CutLoaderPrivate *priv)
{
    gchar *prefix, *base_name, *original_base_name;

    original_base_name = base_name = g_path_get_basename(priv->so_filename);
    if (g_str_has_prefix(base_name, TEST_SUITE_SO_NAME_PREFIX))
        base_name += strlen(TEST_SUITE_SO_NAME_PREFIX);
    prefix = g_strndup(base_name,
                       strlen(base_name) - strlen("." G_MODULE_SUFFIX));
    g_free(original_base_name);
    return prefix;
}

CutTestSuite *
cut_loader_load_test_suite (CutLoader *loader)
{
    CutLoaderPrivate *priv;
    gchar *prefix, *warmup_function_name, *cooldown_function_name;
    CutWarmupFunction warmup = NULL;
    CutCooldownFunction cooldown = NULL;

    priv = CUT_LOADER_GET_PRIVATE(loader);

    if (!priv->so_filename)
        return NULL;

    priv->module = g_module_open(priv->so_filename, G_MODULE_BIND_LAZY);
    if (!priv->module)
        return NULL;

    prefix = get_suite_prefix(priv);

    warmup_function_name = g_strconcat(prefix, "_", "warmup", NULL);
    g_module_symbol(priv->module, warmup_function_name, (gpointer)&warmup);
    g_free(warmup_function_name);

    cooldown_function_name = g_strconcat(prefix, "_", "cooldown", NULL);
    g_module_symbol(priv->module, cooldown_function_name, (gpointer)&cooldown);
    g_free(cooldown_function_name);

    g_free(prefix);

    return cut_test_suite_new(NULL, warmup, cooldown);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
