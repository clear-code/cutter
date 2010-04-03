/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
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
#include <gmodule.h>

#ifdef HAVE_LIBBFD
#  include <bfd.h>
#else
#  include <glib/gstdio.h>
#endif

#include "cut-loader.h"
#include "cut-elf-loader.h"
#include "cut-mach-o-loader.h"
#include "cut-pe-loader.h"
#include "cut-test-iterator.h"
#include "cut-experimental.h"

#define TEST_SUITE_SO_NAME_PREFIX "suite"
#define TEST_NAME_PREFIX "test_"
#define DATA_SETUP_FUNCTION_NAME_PREFIX "data_"
#define ATTRIBUTES_SETUP_FUNCTION_NAME_PREFIX "attributes_"
#define CUT_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_LOADER, CutLoaderPrivate))

typedef enum {
    CUT_BINARY_TYPE_UNKNOWN,
    CUT_BINARY_TYPE_MACH_O_BUNDLE,
    CUT_BINARY_TYPE_MS_WINDOWS_DLL
} CutBinaryType;

typedef struct _SymbolNames SymbolNames;
struct _SymbolNames
{
    gchar *namespace;
    gchar *test_name;
    gchar *test_function_name;
    gchar *data_setup_function_name;
    gchar *attributes_setup_function_name;
    gboolean require_data_setup_function;
    gboolean cpp;
    gboolean gcc;
};

typedef struct _CutLoaderPrivate	CutLoaderPrivate;
struct _CutLoaderPrivate
{
    gchar *so_filename;
    GList *symbols;
    GModule *module;
    CutBinaryType binary_type;
    CutELFLoader *elf_loader;
    CutMachOLoader *mach_o_loader;
    CutPELoader *pe_loader;
    gboolean keep_opening;
    gboolean enable_convenience_attribute_definition;
    gchar *base_directory;
};

enum
{
    PROP_0,
    PROP_SO_FILENAME,
    PROP_BASE_DIRECTORY
};

G_DEFINE_TYPE (CutLoader, cut_loader, G_TYPE_OBJECT)

static SymbolNames *
symbol_names_new (gchar *namespace,
                  gchar *test_name,
                  gchar *test_function_name,
                  gchar *data_setup_function_name,
                  gchar *attributes_setup_function_name,
                  gboolean require_data_setup_function,
                  gboolean cpp,
                  gboolean gcc)
{
    SymbolNames *names;

    names = g_new0(SymbolNames, 1);
    names->namespace = namespace;
    names->test_name = test_name;
    names->test_function_name = test_function_name;
    names->data_setup_function_name = data_setup_function_name;
    names->attributes_setup_function_name = attributes_setup_function_name;
    names->require_data_setup_function = require_data_setup_function;
    names->cpp = cpp;
    names->gcc = gcc;

    return names;
}

static void
symbol_names_free (SymbolNames *names)
{
    g_free(names->namespace);
    g_free(names->test_name);
    g_free(names->test_function_name);
    g_free(names->data_setup_function_name);
    g_free(names->attributes_setup_function_name);
    g_free(names);
}

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

    spec = g_param_spec_string("base-directory",
                               "Base directory",
                               "The base directory of shared object",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_BASE_DIRECTORY, spec);

    g_type_class_add_private(gobject_class, sizeof(CutLoaderPrivate));
}

static void
cut_loader_init (CutLoader *loader)
{
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
    priv->binary_type = CUT_BINARY_TYPE_UNKNOWN;
    priv->elf_loader = NULL;
    priv->mach_o_loader = NULL;
    priv->pe_loader = NULL;
    priv->keep_opening = FALSE;
    priv->enable_convenience_attribute_definition = FALSE;
    priv->base_directory = NULL;
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

    if (priv->elf_loader) {
        g_object_unref(priv->elf_loader);
        priv->elf_loader = NULL;
    }

    if (priv->mach_o_loader) {
        g_object_unref(priv->mach_o_loader);
        priv->mach_o_loader = NULL;
    }

    if (priv->pe_loader) {
        g_object_unref(priv->pe_loader);
        priv->pe_loader = NULL;
    }

    if (priv->module) {
        if (!priv->keep_opening)
            g_module_close(priv->module);
        priv->module = NULL;
    }

    free_symbols(priv);

    if (priv->base_directory) {
        g_free(priv->base_directory);
        priv->base_directory = NULL;
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
        priv->so_filename = g_value_dup_string(value);
        break;
      case PROP_BASE_DIRECTORY:
        cut_loader_set_base_directory(CUT_LOADER(object),
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
    CutLoaderPrivate *priv = CUT_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
      case PROP_BASE_DIRECTORY:
        g_value_set_string(value, priv->base_directory);
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

gboolean
cut_loader_get_keep_opening (CutLoader *loader)
{
    return CUT_LOADER_GET_PRIVATE(loader)->keep_opening;
}

void
cut_loader_set_keep_opening (CutLoader *loader, gboolean keep_opening)
{
    CUT_LOADER_GET_PRIVATE(loader)->keep_opening = keep_opening;
}

gboolean
cut_loader_get_enable_convenience_attribute_definition (CutLoader *loader)
{
    return CUT_LOADER_GET_PRIVATE(loader)->enable_convenience_attribute_definition;
}

void
cut_loader_set_enable_convenience_attribute_definition (CutLoader *loader, gboolean enable_convenience_attribute_definition)
{
    CUT_LOADER_GET_PRIVATE(loader)->enable_convenience_attribute_definition =
        enable_convenience_attribute_definition;
}

const gchar *
cut_loader_get_base_directory (CutLoader *loader)
{
    return CUT_LOADER_GET_PRIVATE(loader)->base_directory;
}

void
cut_loader_set_base_directory (CutLoader *loader, const gchar *base_directory)
{
    CutLoaderPrivate *priv;

    priv = CUT_LOADER_GET_PRIVATE(loader);
    if (priv->base_directory)
        g_free(priv->base_directory);
    priv->base_directory = g_strdup(base_directory);
}

static inline const gchar *
skip_cpp_namespace_gcc (const gchar *name, GString *namespaces)
{
    if (g_str_has_prefix(name, "_Z")) {
        name += strlen("_Z");
    } else {
        return NULL;
    }

    if (!name[0])
        return NULL;

    while (name[0] == 'N') {
        gchar *namespace_name_start;
        guint64 namespace_name_length;

        name++;
        namespace_name_length = g_ascii_strtoull(name,
                                                 &namespace_name_start,
                                                 10);
        if (namespace_name_length == 0)
            return NULL;
        if (namespaces) {
            g_string_append_len(namespaces,
                                namespace_name_start,
                                namespace_name_length);
            g_string_append(namespaces, "::");
        }
        name = namespace_name_start + namespace_name_length;
    }

    return name;
}

static inline gboolean
find_cpp_test_name_gcc (const gchar *name, gchar **test_name_start,
                        guint64 *test_name_length, GString *namespaces)
{
    guint64 length;

    name = skip_cpp_namespace_gcc(name, namespaces);
    if (!name)
        return FALSE;

    length = g_ascii_strtoull(name, test_name_start, 10);
    if (length == 0)
        return FALSE;

    if (test_name_length)
        *test_name_length = length;

    return g_str_has_prefix(*test_name_start, TEST_NAME_PREFIX);
}

static inline SymbolNames *
detect_cpp_test_function_symbol_names_gcc (const gchar *name)
{
    const gchar *original_name = name;
    gchar *test_name_start;
    guint64 test_name_length;
    gchar *data_setup_function_name = NULL;
    gchar *attributes_setup_function_name = NULL;
    gboolean require_data_setup_function = FALSE;
    gchar *namespace;
    GString *test_name;

    test_name = g_string_new(NULL);
    if (!find_cpp_test_name_gcc(name, &test_name_start, &test_name_length,
                                test_name)) {
        g_string_free(test_name, TRUE);
        return NULL;
    }

    name = test_name_start + test_name_length;
    if (g_str_equal(name, "Ev")) {
    } else if (g_str_equal(name, "EPv") || g_str_equal(name, "EPKv")) {
        GString *data_setup_function_name_string;
        size_t test_name_prefix_length;

        require_data_setup_function = TRUE;

        test_name_prefix_length = strlen(TEST_NAME_PREFIX);

        data_setup_function_name_string = g_string_new(NULL);
        g_string_append_len(data_setup_function_name_string,
                            original_name, original_name - test_name_start);
        g_string_append(data_setup_function_name_string,
                        DATA_SETUP_FUNCTION_NAME_PREFIX);
        g_string_append_len(data_setup_function_name_string,
                            test_name_start + test_name_prefix_length,
                            test_name_length - test_name_prefix_length);
        g_string_append(data_setup_function_name_string, "Ev");

        data_setup_function_name = g_string_free(data_setup_function_name_string,
                                                 FALSE);
    } else {
        g_string_free(test_name, TRUE);
        return NULL;
    }

    {
        GString *attributes_setup_function_name_string;
        size_t test_name_prefix_length;

        test_name_prefix_length = strlen(TEST_NAME_PREFIX);

        attributes_setup_function_name_string = g_string_new(NULL);
        g_string_append_len(attributes_setup_function_name_string,
                            original_name, original_name - test_name_start);
        g_string_append(attributes_setup_function_name_string,
                        ATTRIBUTES_SETUP_FUNCTION_NAME_PREFIX);
        g_string_append_len(attributes_setup_function_name_string,
                            test_name_start + test_name_prefix_length,
                            test_name_length - test_name_prefix_length);
        g_string_append(attributes_setup_function_name_string, "Ev");

        attributes_setup_function_name =
            g_string_free(attributes_setup_function_name_string, FALSE);
    }

    if (test_name->len == 0) {
        namespace = g_strdup("");
    } else {
        namespace = g_strndup(test_name->str, test_name->len - 2);
    }
    g_string_append_len(test_name, test_name_start, test_name_length);
    return symbol_names_new(namespace,
                            g_string_free(test_name, FALSE),
                            g_strdup(original_name),
                            data_setup_function_name,
                            attributes_setup_function_name,
                            require_data_setup_function,
                            TRUE,
                            TRUE);
}

static inline gboolean
is_gcc_cpp_symbol (const gchar *name)
{
    return g_str_has_prefix(name, "_Z");
}

/*
  format:
    '?FUNCTION_NAME@NAMESPACE_N@NAMESPACE_N-1@...@NAMESPACE_0@@SIGNATURE'
  SIGNATURE_NAME:
    'YAXXZ': void XXX(void);
    'YAXPAX@Z': void XXX(void *);
    'YAXPBX@Z': void XXX(const void *);
*/
static inline SymbolNames *
detect_cpp_test_function_symbol_names_vcc (const gchar *name)
{
    const gchar *original_name = name;
    const gchar *test_name_start;
    guint test_name_length;
    gchar *data_setup_function_name = NULL;
    gchar *attributes_setup_function_name = NULL;
    gboolean require_data_setup_function = FALSE;
    GString *test_name;

    test_name_start = name + strlen("?");
    if (!g_str_has_prefix(test_name_start, TEST_NAME_PREFIX))
        return NULL;
    test_name_length = 0;
    while (test_name_start[test_name_length] &&
           test_name_start[test_name_length] != '@') {
        test_name_length++;
    }
    if (test_name_start[test_name_length] != '@')
        return NULL;

    test_name = g_string_new(NULL);
    g_string_append_len(test_name, test_name_start, test_name_length);

    name = test_name_start + test_name_length;
    while (name[0]) {
        const gchar *namespace;
        guint namespace_length = 0;

        if (name[0] != '@') {
            g_string_free(test_name, TRUE);
            return NULL;
        }
        namespace = name + strlen("@");

        if (namespace[0] == '@') {
            name = namespace + 1;
            break;
        }

        while (namespace[namespace_length] &&
               namespace[namespace_length] != '@') {
            namespace_length++;
        }
        g_string_prepend(test_name, "::");
        g_string_prepend_len(test_name, namespace, namespace_length);

        name = namespace + namespace_length;
    }

    if (g_str_equal(name, "YAXXZ")) {
    } else if (g_str_equal(name, "YAXPAX@Z") || g_str_equal(name, "YAXPBX@Z")) {
        GString *data_setup_function_name_string;
        size_t test_name_prefix_length;

        require_data_setup_function = TRUE;

        test_name_prefix_length = strlen(TEST_NAME_PREFIX);

        data_setup_function_name_string = g_string_new(NULL);
        g_string_append_len(data_setup_function_name_string,
                            original_name, original_name - test_name_start);
        g_string_append(data_setup_function_name_string,
                        DATA_SETUP_FUNCTION_NAME_PREFIX);
        g_string_append_len(data_setup_function_name_string,
                            test_name_start + test_name_prefix_length,
                            name - test_name_start + test_name_prefix_length);
        g_string_append(data_setup_function_name_string, "YAXXZ");

        data_setup_function_name = g_string_free(data_setup_function_name_string,
                                                 FALSE);
    } else {
        g_string_free(test_name, TRUE);
        return NULL;
    }

    {
        GString *attributes_setup_function_name_string;
        size_t test_name_prefix_length;

        test_name_prefix_length = strlen(TEST_NAME_PREFIX);

        attributes_setup_function_name_string = g_string_new(NULL);
        g_string_append_len(attributes_setup_function_name_string,
                            original_name, original_name - test_name_start);
        g_string_append(attributes_setup_function_name_string,
                        ATTRIBUTES_SETUP_FUNCTION_NAME_PREFIX);
        g_string_append_len(attributes_setup_function_name_string,
                            test_name_start + test_name_prefix_length,
                            name - test_name_start + test_name_prefix_length);
        g_string_append(attributes_setup_function_name_string, "YAXXZ");

        attributes_setup_function_name =
            g_string_free(attributes_setup_function_name_string, FALSE);
    }

    {
        gchar *last_separator;
        gchar *test_case_name;

        last_separator = g_strrstr(test_name->str, "::");
        if (last_separator) {
            test_case_name = g_strndup(test_name->str,
                                       last_separator - test_name->str);
        } else {
            test_case_name = g_strdup("");
        }
        return symbol_names_new(test_case_name,
                                g_string_free(test_name, FALSE),
                                g_strdup(original_name),
                                data_setup_function_name,
                                attributes_setup_function_name,
                                require_data_setup_function,
                                TRUE,
                                FALSE);
    }
}

static inline gboolean
is_vcc_cpp_symbol (const gchar *name)
{
    return g_str_has_prefix(name, "?");
}

static inline SymbolNames *
detect_cpp_test_function_symbol_names (const gchar *name)
{
    if (is_gcc_cpp_symbol(name)) {
        return detect_cpp_test_function_symbol_names_gcc(name);
    } else if (is_vcc_cpp_symbol(name)) {
        return detect_cpp_test_function_symbol_names_vcc(name);
    }

    return NULL;
}

static inline SymbolNames *
detect_test_function_symbol_names (const gchar *name)
{
    if (!name)
        return NULL;

    if (g_str_has_prefix(name, TEST_NAME_PREFIX)) {
        gchar *data_setup_function_name;
        gchar *attributes_setup_function_name;

        data_setup_function_name =
            g_strconcat(DATA_SETUP_FUNCTION_NAME_PREFIX,
                        name + strlen(TEST_NAME_PREFIX),
                        NULL);
        attributes_setup_function_name =
            g_strconcat(ATTRIBUTES_SETUP_FUNCTION_NAME_PREFIX,
                        name + strlen(TEST_NAME_PREFIX),
                        NULL);
        return symbol_names_new(NULL,
                                g_strdup(name),
                                g_strdup(name),
                                data_setup_function_name,
                                attributes_setup_function_name,
                                FALSE,
                                FALSE,
                                FALSE);
    }

    return detect_cpp_test_function_symbol_names(name);
}

#ifdef HAVE_LIBBFD

static gboolean
cut_loader_support_attribute_bfd (CutLoader *loader)
{
    CutLoaderPrivate *priv;

    priv = CUT_LOADER_GET_PRIVATE(loader);
    return priv->binary_type != CUT_BINARY_TYPE_MACH_O_BUNDLE;
}

static GList *
collect_symbols_bfd (CutLoaderPrivate *priv)
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
        priv->binary_type = CUT_BINARY_TYPE_MACH_O_BUNDLE;

    for (i = 0; i < number_of_symbols; i++) {
        symbol_info info;

        bfd_symbol_info(symbol_table[i], &info);
        if (info.type == 'T' ||
            (priv->binary_type == CUT_BINARY_TYPE_MACH_O_BUNDLE &&
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

static gboolean
cut_loader_support_attribute_scan (CutLoader *loader)
{
    CutLoaderPrivate *priv;

    priv = CUT_LOADER_GET_PRIVATE(loader);
    return priv->binary_type == CUT_BINARY_TYPE_MS_WINDOWS_DLL;
}

static inline CutBinaryType
guess_binary_type (char *buffer, size_t size)
{
    if (size >= 4 && ((guint32 *)buffer)[0] == 0xfeedface)
        return CUT_BINARY_TYPE_MACH_O_BUNDLE;
    if (size >= 4 &&
        ((guint8 *)buffer)[0] == 'M' &&
        ((guint8 *)buffer)[1] == 'Z' &&
        ((guint8 *)buffer)[4] & 0x2)
        return CUT_BINARY_TYPE_MS_WINDOWS_DLL;
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
    return name->len >= 4;
}

static GList *
collect_symbols_scan (CutLoaderPrivate *priv)
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
            priv->binary_type = guess_binary_type(buffer, size);
            first_buffer = FALSE;
        }

        for (i = 0; i < size; i++) {
            if (is_valid_char_for_cutter_symbol(name, buffer, i,
                                                size, priv->binary_type)) {
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

gboolean
cut_loader_support_attribute (CutLoader *loader)
{
    CutLoaderPrivate *priv;

    priv = CUT_LOADER_GET_PRIVATE(loader);
    if (!priv->enable_convenience_attribute_definition)
        return FALSE;

    if (priv->elf_loader) {
        return cut_elf_loader_support_attribute(priv->elf_loader);
    } else if (priv->mach_o_loader) {
        return cut_mach_o_loader_support_attribute(priv->mach_o_loader);
    } else if (priv->pe_loader) {
        return cut_pe_loader_support_attribute(priv->pe_loader);
    } else {
#ifdef HAVE_LIBBFD
        return cut_loader_support_attribute_bfd(loader);
#else
        return cut_loader_support_attribute_scan(loader);
#endif
    }
}

static GList *
collect_symbols (CutLoaderPrivate *priv)
{
    if (priv->elf_loader) {
        return cut_elf_loader_collect_symbols(priv->elf_loader);
    } else if (priv->mach_o_loader) {
        return cut_mach_o_loader_collect_symbols(priv->mach_o_loader);
    } else if (priv->pe_loader) {
        return cut_pe_loader_collect_symbols(priv->pe_loader);
    } else {
#ifdef HAVE_LIBBFD
        return collect_symbols_bfd(priv);
#else
        return collect_symbols_scan(priv);
#endif
    }
}

static GList *
collect_test_functions (CutLoaderPrivate *priv)
{
    GList *node, *test_names = NULL;

    for (node = priv->symbols; node; node = g_list_next(node)) {
        gchar *name = node->data;
        SymbolNames *names;
        names = detect_test_function_symbol_names(name);
        if (names) {
            test_names = g_list_prepend(test_names, names);
        }
    }
    return test_names;
}

static gboolean
is_including_test_name (const gchar *function_name, SymbolNames *names)
{
    gboolean included = FALSE;
    GString *suffix;
    gchar *base_name;

    if (names->cpp) {
        const gchar *target_namespace_end;
        GString *target_namespace;
        GString *test_namespace;
        guint64 function_name_length;

        target_namespace = g_string_new(NULL);
        target_namespace_end = skip_cpp_namespace_gcc(function_name,
                                                      target_namespace);
        if (!target_namespace_end) {
            g_string_free(target_namespace, TRUE);
            return FALSE;
        }

        test_namespace = g_string_new(NULL);
        skip_cpp_namespace_gcc(names->test_function_name, test_namespace);
        included = g_str_equal(target_namespace->str, test_namespace->str);
        g_string_free(target_namespace, TRUE);
        g_string_free(test_namespace, TRUE);
        if (!included)
            return FALSE;

        function_name_length = g_ascii_strtoull(target_namespace_end,
                                                &base_name, 10);
        if (function_name_length == 0)
            return FALSE;
    } else {
        base_name = (gchar *)function_name;
    }

    suffix = g_string_new("_");
    g_string_append(suffix, names->test_name + strlen(TEST_NAME_PREFIX));
    if (names->cpp)
        g_string_append(suffix, "Ev");
    included = g_str_has_suffix(base_name, suffix->str);
    g_string_free(suffix, TRUE);

    return included;
}

static gboolean
is_valid_attribute_function_name (const gchar *function_name, SymbolNames *names)
{
    gchar *base_name;

    if (!function_name)
        return FALSE;

    if (names->cpp) {
        if (!find_cpp_test_name_gcc(function_name, &base_name, NULL, NULL))
            return FALSE;
    } else {
        base_name = (gchar *)function_name;
    }

    return
        !g_str_has_prefix(base_name, TEST_NAME_PREFIX) &&
        !g_str_has_prefix(base_name,
                          ATTRIBUTES_SETUP_FUNCTION_NAME_PREFIX) &&
        !g_str_has_prefix(base_name, DATA_SETUP_FUNCTION_NAME_PREFIX) &&
        is_including_test_name(function_name, names);
}

static gchar *
get_attribute_name (const gchar *attribute_function_name, SymbolNames *names)
{
    gchar *base_name;
    gchar *test_base_name;

    if (names->cpp) {
        const gchar *namespace_end;

        namespace_end = skip_cpp_namespace_gcc(attribute_function_name, NULL);
        g_ascii_strtoull(namespace_end, &base_name, 10);
    } else {
        base_name = (gchar *)attribute_function_name;
    }

    test_base_name = g_strrstr(base_name,
                               names->test_name + strlen(TEST_NAME_PREFIX)) - 1;
    return g_strndup(base_name, test_base_name - base_name);
}

typedef const gchar *(*CutAttributeItemFunction)     (void);

static void
apply_attributes (CutLoaderPrivate *priv, CutTest *test, SymbolNames *names)
{
    GList *node;

    if (!test)
        return;
    if (!names)
        return;

    for (node = priv->symbols; node; node = g_list_next(node)) {
        gchar *function_name = node->data;
        if (is_valid_attribute_function_name(function_name, names)) {
            CutAttributeItemFunction function = NULL;
            g_module_symbol(priv->module, function_name, (gpointer)&function);
            if (function) {
                gchar *name;
                const gchar *value;

                name = get_attribute_name(function_name, names);
                value = function();
                cut_test_set_attribute(test, name, value);
                g_free(name);
            }
        }
    }
}

static void
register_valid_test (CutLoader *loader, CutTestCase *test_case,
                     SymbolNames *names)
{
    CutLoaderPrivate *priv;
    CutTest *test;
    CutTestFunction test_function = NULL;
    CutDataSetupFunction data_setup_function = NULL;
    CutAttributesSetupFunction attributes_setup_function = NULL;

    priv = CUT_LOADER_GET_PRIVATE(loader);

    g_module_symbol(priv->module, names->test_function_name,
                    (gpointer)&test_function);
    if (!test_function)
        return;

    if (names->data_setup_function_name)
        g_module_symbol(priv->module, names->data_setup_function_name,
                        (gpointer)&data_setup_function);
    if (names->require_data_setup_function && !data_setup_function)
        return;

    if (names->attributes_setup_function_name)
        g_module_symbol(priv->module, names->attributes_setup_function_name,
                        (gpointer)&attributes_setup_function);

    if (data_setup_function) {
        CutTestIterator *test_iterator;
        CutIteratedTestFunction iterated_test_function;

        iterated_test_function = (CutIteratedTestFunction)test_function;
        test_iterator = cut_test_iterator_new(names->test_name,
                                              iterated_test_function,
                                              data_setup_function);
        test = CUT_TEST(test_iterator);
    } else {
        test = cut_test_new(names->test_name, test_function);
    }
    cut_test_set_base_directory(test, priv->base_directory);

    if (attributes_setup_function) {
        CutTestContext *test_context;

        if (data_setup_function) {
            test_context = cut_test_context_new(NULL, NULL, test_case,
                                                CUT_TEST_ITERATOR(test), NULL);
        } else {
            test_context = cut_test_context_new(NULL, NULL, test_case,
                                                NULL, test);
        }
        cut_test_context_current_push(test_context);
        attributes_setup_function();
        cut_test_context_current_pop();
        g_object_unref(test_case);
    }

    if (cut_loader_support_attribute(loader))
        apply_attributes(priv, test, names);

    cut_test_case_add_test(test_case, test);

    g_object_unref(test);
}

static const gchar *
mangle_gcc (GString *buffer, const gchar *namespace, const gchar *function)
{
    gchar **components, **component;

    if (!namespace)
        return function;

    g_string_assign(buffer, "_ZN");
    components = g_strsplit(namespace, "::", 0);
    for (component = components; *component; component++) {
        g_string_append_printf(buffer, "%" G_GSIZE_FORMAT "%s",
                               strlen(*component), *component);
    }
    g_strfreev(components);
    g_string_append_printf(buffer, "%" G_GSIZE_FORMAT "%s",
                           strlen(function), function);
    g_string_append(buffer, "Ev");

    return buffer->str;
}

static const gchar *
mangle_vcc (GString *buffer, const gchar *namespace, const gchar *function)
{
    gchar **components, **component;

    if (!namespace)
        return function;

    g_string_assign(buffer, "@YAXXZ");
    components = g_strsplit(namespace, "::", 0);
    for (component = components; *component; component++) {
        g_string_prepend(buffer, "@");
        g_string_prepend(buffer, *component);
    }
    g_strfreev(components);

    g_string_prepend(buffer, "@");
    g_string_prepend(buffer, function);
    g_string_prepend(buffer, "?");

    return buffer->str;
}

static const gchar *
mangle (GString *buffer, const gchar *namespace, const gchar *function,
        gboolean gcc)
{
    if (!namespace)
        return function;

    if (gcc) {
        return mangle_gcc(buffer, namespace, function);
    } else {
        return mangle_vcc(buffer, namespace, function);
    }
}

static void
cb_complete (CutTestCase *test_case, CutTestContext *test_context,
             gboolean success, gpointer data)
{
    CutLoader *loader = data;
    g_object_unref(loader);
}

static CutTestCase *
create_test_case (CutLoader *loader, const gchar *namespace, gboolean gcc)
{
    CutLoaderPrivate *priv;
    CutTestCase *test_case;
    CutSetupFunction setup = NULL;
    CutStartupFunction startup = NULL;
    CutShutdownFunction shutdown = NULL;
    CutTeardownFunction teardown = NULL;
    gchar *test_case_name;
    GString *buffer;

    priv = CUT_LOADER_GET_PRIVATE(loader);

    buffer = g_string_new(NULL);
#define MANGLE(function_name)                   \
    mangle(buffer, namespace, function_name, gcc)
#define GET_HOOK_FUNCTION(name)                                         \
    if (!g_module_symbol(priv->module, MANGLE("cut_" #name), (gpointer)&name)) \
        g_module_symbol(priv->module, MANGLE(#name), (gpointer)&name)

    GET_HOOK_FUNCTION(setup);
    GET_HOOK_FUNCTION(teardown);
    if (namespace) {
        GET_HOOK_FUNCTION(startup);
        GET_HOOK_FUNCTION(shutdown);
    } else {
        g_module_symbol(priv->module, MANGLE("cut_startup"), (gpointer)&startup);
        g_module_symbol(priv->module, MANGLE("cut_shutdown"), (gpointer)&shutdown);
    }
#undef GET_HOOK_FUNCTION
#undef MANGLE
    g_string_free(buffer, TRUE);

    if (namespace) {
        test_case_name = g_strdup(namespace);
    } else {
        gchar *filename;

        filename = g_path_get_basename(priv->so_filename);
        if (g_str_has_prefix(filename, "lib")) {
            gchar *string;
            string = g_strdup(filename + strlen("lib"));
            g_free(filename);
            filename = string;
        }
        test_case_name =
            g_strndup(filename,
                      strlen(filename) - strlen(G_MODULE_SUFFIX) -1);
        g_free(filename);
    }
    test_case = cut_test_case_new(test_case_name,
                                  setup, teardown,
                                  startup, shutdown);
    g_free(test_case_name);

    cut_test_set_base_directory(CUT_TEST(test_case), priv->base_directory);

    g_object_ref(loader);
    g_signal_connect(test_case, "complete", G_CALLBACK(cb_complete), loader);

    return test_case;
}

static gint
find_cpp_test_case (gconstpointer value1, gconstpointer value2)
{
    CutTestCase *test_case;
    SymbolNames *names;

    test_case = CUT_TEST_CASE(value1);
    names = (SymbolNames *)value2;
    if (g_str_equal(cut_test_get_name(CUT_TEST(test_case)),
                    names->namespace)) {
        return 0;
    } else {
        return 1;
    }
}

GList *
cut_loader_load_test_cases (CutLoader *loader)
{
    CutLoaderPrivate *priv;
    GList *node;
    GList *test_names, *test_cases = NULL;
    CutTestCase *c_test_case = NULL;

    priv = CUT_LOADER_GET_PRIVATE(loader);
    if (!priv->so_filename)
        return NULL;

    priv->module = g_module_open(priv->so_filename,
                                 G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
    if (!priv->module) {
        g_warning("can't load a shared object for test case: %s: %s",
                  priv->so_filename, g_module_error());
        return NULL;
    }

    priv->elf_loader = cut_elf_loader_new(priv->so_filename);
    if (!cut_elf_loader_is_elf(priv->elf_loader)) {
        g_object_unref(priv->elf_loader);
        priv->elf_loader = NULL;
    }

    priv->mach_o_loader = cut_mach_o_loader_new(priv->so_filename);
    if (!cut_mach_o_loader_is_mach_o(priv->mach_o_loader)) {
        g_object_unref(priv->mach_o_loader);
        priv->mach_o_loader = NULL;
    }

    priv->pe_loader = cut_pe_loader_new(priv->so_filename);
    if (!cut_pe_loader_is_dll(priv->pe_loader)) {
        g_object_unref(priv->pe_loader);
        priv->pe_loader = NULL;
    }

    priv->symbols = collect_symbols(priv);
    if (!priv->symbols)
        return NULL;

    test_names = collect_test_functions(priv);
    if (!test_names)
        return NULL;

    for (node = test_names; node; node = g_list_next(node)) {
        SymbolNames *names = node->data;
        CutTestCase *test_case = NULL;

        if (names->namespace) {
            GList *test_case_node;
            test_case_node = g_list_find_custom(test_cases, names,
                                                find_cpp_test_case);
            if (test_case_node)
                test_case = test_case_node->data;
            if (!test_case) {
                test_case = create_test_case(loader, names->namespace,
                                             names->gcc);
                test_cases = g_list_prepend(test_cases, test_case);
            }
        } else {
            if (!c_test_case) {
                c_test_case = create_test_case(loader, NULL, FALSE);
                test_cases = g_list_prepend(test_cases, c_test_case);
            }
            test_case = c_test_case;
        }
        register_valid_test(loader, test_case, names);
        symbol_names_free(names);
    }
    g_list_free(test_names);

    return test_cases;
}

CutTestCase *
cut_loader_load_test_case (CutLoader *loader)
{
    GList* test_cases;
    CutTestCase* test_case;

    test_cases = cut_loader_load_test_cases(loader);
    if (!test_cases)
        return NULL;

    test_case = test_cases->data;
    g_list_foreach(g_list_next(test_cases), (GFunc)g_object_unref, NULL);
    g_list_free(test_cases);

    return test_case;
}

static gchar *
get_suite_prefix (CutLoaderPrivate *priv)
{
    gchar *prefix, *base_name, *original_base_name;
    gchar *character;

    original_base_name = base_name = g_path_get_basename(priv->so_filename);
    if (g_str_has_prefix(base_name, TEST_SUITE_SO_NAME_PREFIX "_") ||
        g_str_has_prefix(base_name, TEST_SUITE_SO_NAME_PREFIX "-"))
        base_name += strlen(TEST_SUITE_SO_NAME_PREFIX "-");
    prefix = g_strndup(base_name,
                       strlen(base_name) - strlen("." G_MODULE_SUFFIX));
    g_free(original_base_name);

    for (character = prefix; *character; character++) {
        if (*character == '-')
            *character = '_';
    }

    return prefix;
}

CutTestSuite *
cut_loader_load_test_suite (CutLoader *loader)
{
    CutLoaderPrivate *priv;
    gchar *prefix, *warmup_function_name, *cooldown_function_name;
    CutTestSuite *test_suite;
    CutWarmupFunction warmup = NULL;
    CutCooldownFunction cooldown = NULL;

    priv = CUT_LOADER_GET_PRIVATE(loader);

    if (!priv->so_filename)
        return NULL;

    priv->module = g_module_open(priv->so_filename, G_MODULE_BIND_LAZY);
    if (!priv->module) {
        g_warning("can't load a shared object for test suite: %s: %s",
                  priv->so_filename, g_module_error());
        return NULL;
    }

    prefix = get_suite_prefix(priv);

    warmup_function_name = g_strconcat(prefix, "_", "warmup", NULL);
    g_module_symbol(priv->module, warmup_function_name, (gpointer)&warmup);
    g_free(warmup_function_name);

    cooldown_function_name = g_strconcat(prefix, "_", "cooldown", NULL);
    g_module_symbol(priv->module, cooldown_function_name, (gpointer)&cooldown);
    g_free(cooldown_function_name);

    g_free(prefix);

    test_suite = cut_test_suite_new(NULL, warmup, cooldown);
    cut_test_set_base_directory(CUT_TEST(test_suite), priv->base_directory);
    return test_suite;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
