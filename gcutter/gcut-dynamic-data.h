/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __GCUT_DYNAMIC_DATA_H__
#define __GCUT_DYNAMIC_DATA_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-dynamic-data
 * @title: Dynamic data structure
 * @short_description: FIXME
 *
 * FIXME
 */

/**
 * GCUT_DYNAMIC_DATA_ERROR:
 *
 * Error domain for key file parsing. Errors in this domain
 * will be from the #GCutDynamicDataError data. See #GError for
 * information on error domains.
 *
 * Since: 1.0.6
 */
#define GCUT_DYNAMIC_DATA_ERROR           (gcut_dynamic_data_error_quark())

#define GCUT_TYPE_DYNAMIC_DATA            (gcut_dynamic_data_get_type())
#define GCUT_DYNAMIC_DATA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCUT_TYPE_DYNAMIC_DATA, GCutDynamicData))
#define GCUT_DYNAMIC_DATA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GCUT_TYPE_DYNAMIC_DATA, GCutDynamicDataClass))
#define GCUT_IS_DATA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCUT_TYPE_DYNAMIC_DATA))
#define GCUT_IS_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GCUT_TYPE_DYNAMIC_DATA))
#define GCUT_DYNAMIC_DATA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_DYNAMIC_DATA, GCutDynamicDataClass))

typedef struct _GCutDynamicData      GCutDynamicData;
typedef struct _GCutDynamicDataClass GCutDynamicDataClass;

struct _GCutDynamicData
{
    GObject object;
};

struct _GCutDynamicDataClass
{
    GObjectClass parent_class;
};

/**
 * GCutDynamicDataError:
 * @GCUT_DYNAMIC_DATA_ERROR_NOT_EXIST: The field doesn't exist.
 *
 * Error codes returned by data related operation.
 *
 * Since: 1.0.6
 */
typedef enum
{
    GCUT_DYNAMIC_DATA_ERROR_NOT_EXIST
} GCutDynamicDataError;

GQuark           gcut_dynamic_data_error_quark  (void);

GType            gcut_dynamic_data_get_type     (void) G_GNUC_CONST;

GCutDynamicData *gcut_dynamic_data_new          (const gchar     *first_field_name,
                                                 ...) G_GNUC_NULL_TERMINATED;
GCutDynamicData *gcut_dynamic_data_new_va_list  (const gchar     *first_field_name,
                                                 va_list          args);

gchar           *gcut_dynamic_data_inspect      (GCutDynamicData  *data);
gboolean         gcut_dynamic_data_equal        (GCutDynamicData  *data1,
                                                 GCutDynamicData  *data2);


const gchar     *gcut_dynamic_data_get_string   (GCutDynamicData  *data,
                                                 const gchar      *field_name,
                                                 GError          **error);
gint             gcut_dynamic_data_get_int      (GCutDynamicData  *data,
                                                 const gchar      *field_name,
                                                 GError          **error);
guint            gcut_dynamic_data_get_uint     (GCutDynamicData  *data,
                                                 const gchar      *field_name,
                                                 GError          **error);
GType            gcut_dynamic_data_get_data_type(GCutDynamicData  *data,
                                                 const gchar      *field_name,
                                                 GError          **error);
guint            gcut_dynamic_data_get_flags    (GCutDynamicData  *data,
                                                 const gchar      *field_name,
                                                 GError          **error);
gint             gcut_dynamic_data_get_enum     (GCutDynamicData  *data,
                                                 const gchar      *field_name,
                                                 GError          **error);
gconstpointer    gcut_dynamic_data_get_pointer  (GCutDynamicData  *data,
                                                 const gchar      *field_name,
                                                 GError          **error);

const gchar     *gcut_data_get_string_helper    (const GCutDynamicData *data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gint             gcut_data_get_int_helper       (const GCutDynamicData *data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
guint            gcut_data_get_uint_helper      (const GCutDynamicData *data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
GType            gcut_data_get_type_helper      (const GCutDynamicData *data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
guint            gcut_data_get_flags_helper     (const GCutDynamicData *data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gint             gcut_data_get_enum_helper      (const GCutDynamicData *data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gconstpointer    gcut_data_get_pointer_helper   (const GCutDynamicData *data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);

G_END_DECLS

#endif /* __GCUT_DYNAMIC_DATA_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
