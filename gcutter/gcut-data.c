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
#  include <config.h>
#endif /* HAVE_CONFIG_H */


#include "gcut-assertions.h"

#define GCUT_DATA_GET_PRIVATE(obj)                                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), GCUT_TYPE_DATA, GCutDataPrivate))

typedef struct _FieldValue
{
    GType type;
    union {
        gpointer pointer;
        GType type;
        gint integer;
        guint unsigned_integer;
    } value;
    GDestroyNotify free_function;
} FieldValue;

typedef struct _GCutDataPrivate	GCutDataPrivate;
struct _GCutDataPrivate
{
    GHashTable *fields;
};

static FieldValue *
field_value_new (GType type)
{
    FieldValue *field_value;

    field_value = g_slice_new0(FieldValue);
    field_value->type = type;

    return field_value;
}

static void
field_value_free (FieldValue *field_value)
{
    if (field_value->free_function && field_value->value.pointer)
        field_value->free_function(field_value->value.pointer);

    g_slice_free(FieldValue, field_value);
}

static void
field_value_inspect (GString *string, gconstpointer data, gpointer user_data)
{
    const FieldValue *field_value = data;

    switch (field_value->type) {
      case G_TYPE_STRING:
        gcut_inspect_string(string, field_value->value.pointer, user_data);
        break;
      default:
        if (field_value->type == G_TYPE_GTYPE) {
            gcut_inspect_type(string, &(field_value->value.type), user_data);
        } else if (G_TYPE_IS_FLAGS(field_value->type)) {
            GType flags_type = field_value->type;
            gcut_inspect_flags(string,
                               &(field_value->value.unsigned_integer),
                               &flags_type);
        } else if (G_TYPE_IS_ENUM(field_value->type)) {
            GType enum_type = field_value->type;
            gcut_inspect_enum(string,
                              &(field_value->value.integer),
                              &enum_type);
        } else {
            g_string_append_printf(string,
                                   "[unsupported type: <%s>]",
                                   g_type_name(field_value->type));
        }
        break;
    }
}

static gboolean
field_value_equal (gconstpointer data1, gconstpointer data2)
{
    const FieldValue *field_value1 = data1;
    const FieldValue *field_value2 = data2;
    gboolean result = FALSE;

    if (field_value1 == field_value2)
        return TRUE;

    if (field_value1 == NULL || field_value2 == NULL)
        return FALSE;

    if (field_value1->type != field_value2->type)
        return FALSE;

    switch (field_value1->type) {
      case G_TYPE_STRING:
        result = g_str_equal(field_value1->value.pointer,
                             field_value2->value.pointer);
        break;
      default:
        if (field_value1->type == G_TYPE_GTYPE) {
            result = (field_value1->value.type ==
                      field_value2->value.type);
        } else if (G_TYPE_IS_FLAGS(field_value1->type)) {
            result = (field_value1->value.unsigned_integer ==
                      field_value2->value.unsigned_integer);
        } else if (G_TYPE_IS_ENUM(field_value1->type)) {
            result = (field_value1->value.integer ==
                      field_value2->value.integer);
        } else {
            g_warning("[unsupported type: <%s>]",
                      g_type_name(field_value1->type));
            result = FALSE;
        }
        break;
    }

    return result;
}

#undef gcut_data_get_type
#define gcut_data_get_type gcut_data__get_type
G_DEFINE_TYPE(GCutData, gcut_data, G_TYPE_OBJECT)
#undef gcut_data_get_type

static void dispose        (GObject         *object);

static void
gcut_data_class_init (GCutDataClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(GCutDataPrivate));
}

static void
gcut_data_init (GCutData *data)
{
    GCutDataPrivate *priv = GCUT_DATA_GET_PRIVATE(data);

    priv->fields = g_hash_table_new_full(g_str_hash, g_str_equal,
                                         g_free,
                                         (GDestroyNotify)field_value_free);
}

static void
dispose (GObject *object)
{
    GCutDataPrivate *priv;

    priv = GCUT_DATA_GET_PRIVATE(object);
    if (priv->fields) {
        g_hash_table_unref(priv->fields);
        priv->fields = NULL;
    }

    G_OBJECT_CLASS(gcut_data_parent_class)->dispose(object);
}

GQuark
gcut_data_error_quark (void)
{
    return g_quark_from_static_string("gcut-data-error-quark");
}

GCutData *
gcut_data_new (const gchar *first_field_name, ...)
{
    GCutData *data;
    va_list args;

    va_start(args, first_field_name);
    data = gcut_data_new_va_list(first_field_name, args);
    va_end(args);

    return data;
}

GCutData *
gcut_data_new_va_list (const gchar *first_field_name, va_list args)
{
    GCutDataPrivate *priv;
    GCutData *data;
    const gchar *name;

    data = g_object_new(GCUT_TYPE_DATA, NULL);
    priv = GCUT_DATA_GET_PRIVATE(data);
    name = first_field_name;
    while (name) {
        FieldValue *field_value;

        field_value = field_value_new(va_arg(args, GType));
        switch (field_value->type) {
          case G_TYPE_STRING:
            field_value->value.pointer = g_strdup(va_arg(args, const gchar *));
            field_value->free_function = g_free;
            break;
          default:
            if (field_value->type == G_TYPE_GTYPE) {
                field_value->value.type = va_arg(args, GType);
            } else if (G_TYPE_IS_FLAGS(field_value->type)) {
                field_value->value.unsigned_integer = va_arg(args, guint);
            } else if (G_TYPE_IS_ENUM(field_value->type)) {
                field_value->value.integer = va_arg(args, gint);
            } else {
                g_warning("unsupported type: <%s>",
                          g_type_name(field_value->type));
                field_value_free(field_value);
                field_value = NULL;
            }
            break;
        }
        if (!field_value)
            break;

        g_hash_table_insert(priv->fields, g_strdup(name), field_value);

        name = va_arg(args, const gchar *);
    }

    return data;
}

gchar *
gcut_data_inspect (GCutData *data)
{
    GCutDataPrivate *priv;
    GString *string;
    gchar *inspected_fields;

    priv = GCUT_DATA_GET_PRIVATE(data);
    string = g_string_new(NULL);
    g_string_append_printf(string, "#<GCutData:0x%p ", data);
    inspected_fields = gcut_hash_table_inspect(priv->fields,
                                               gcut_inspect_string,
                                               field_value_inspect,
                                               NULL);
    g_string_append(string, inspected_fields);
    g_free(inspected_fields);
    g_string_append(string, ">");

    return g_string_free(string, FALSE);
}

gboolean
gcut_data_equal (GCutData *data1, GCutData *data2)
{
    GCutDataPrivate *priv1;
    GCutDataPrivate *priv2;

    if (data1 == data2)
        return TRUE;

    if (data1 == NULL || data2 == NULL)
        return FALSE;

    priv1 = GCUT_DATA_GET_PRIVATE(data1);
    priv2 = GCUT_DATA_GET_PRIVATE(data2);
    return gcut_hash_table_equal(priv1->fields,
                                 priv2->fields,
                                 field_value_equal);
}

static FieldValue *
lookup (GCutData *data, const gchar *field_name, GError **error)
{
    GCutDataPrivate *priv;
    FieldValue *field_value;

    priv = GCUT_DATA_GET_PRIVATE(data);
    field_value = g_hash_table_lookup(priv->fields, field_name);
    if (!field_value) {
        g_set_error(error,
                    GCUT_DATA_ERROR,
                    GCUT_DATA_ERROR_NOT_EXIST,
                    "requested field doesn't exist: <%s>", field_name);
        return NULL;
    }

    return field_value;
}

const gchar *
gcut_data_get_string_with_error (GCutData *data, const gchar *field_name,
                                 GError **error)
{
    FieldValue *field_value;

    field_value = lookup(data, field_name, error);
    if (!field_value)
        return NULL;

    return field_value->value.pointer;
}

GType
gcut_data_get_type_with_error (GCutData *data, const gchar *field_name,
                               GError **error)
{
    FieldValue *field_value;

    field_value = lookup(data, field_name, error);
    if (!field_value)
        return G_TYPE_INVALID;

    return field_value->value.type;
}

guint
gcut_data_get_flags_with_error (GCutData *data, const gchar *field_name,
                                GError **error)
{
    FieldValue *field_value;

    field_value = lookup(data, field_name, error);
    if (!field_value)
        return 0;

    return field_value->value.unsigned_integer;
}

gint
gcut_data_get_enum_with_error (GCutData *data, const gchar *field_name,
                               GError **error)
{
    FieldValue *field_value;

    field_value = lookup(data, field_name, error);
    if (!field_value)
        return 0;

    return field_value->value.integer;
}

#define DEFINE_GETTER_HELPER(type_name, type)                           \
type                                                                    \
gcut_data_get_ ## type_name ## _helper (const GCutData *data,           \
                                        const gchar *field_name,        \
                                        CutCallbackFunction callback)   \
{                                                                       \
    GError *error = NULL;                                               \
    type value;                                                         \
                                                                        \
    value = gcut_data_get_ ## type_name ## _with_error(GCUT_DATA(data), \
                                                       field_name,      \
                                                       &error);         \
    gcut_assert_error_helper(error, "error");                           \
    callback();                                                         \
                                                                        \
    return value;                                                       \
}

DEFINE_GETTER_HELPER(string, const gchar *)
DEFINE_GETTER_HELPER(type, GType)
DEFINE_GETTER_HELPER(flags, guint)
DEFINE_GETTER_HELPER(enum, gint)

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
