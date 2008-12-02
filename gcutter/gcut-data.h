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

#ifndef __GCUT_DATA_H__
#define __GCUT_DATA_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-data
 * @title: Dynamic data structure
 * @short_description: FIXME
 *
 * FIXME
 */

/**
 * GCUT_DATA_ERROR:
 *
 * Error domain for key file parsing. Errors in this domain
 * will be from the #GCutDataError data. See #GError for
 * information on error domains.
 *
 * Since: 1.0.6
 */
#define GCUT_DATA_ERROR           (gcut_data_error_quark())

#define GCUT_TYPE_DATA            (gcut_data__get_type())
#define GCUT_DATA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCUT_TYPE_DATA, GCutData))
#define GCUT_DATA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GCUT_TYPE_DATA, GCutDataClass))
#define GCUT_IS_DATA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCUT_TYPE_DATA))
#define GCUT_IS_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GCUT_TYPE_DATA))
#define GCUT_DATA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_DATA, GCutDataClass))

typedef struct _GCutData      GCutData;
typedef struct _GCutDataClass GCutDataClass;

struct _GCutData
{
    GObject object;
};

struct _GCutDataClass
{
    GObjectClass parent_class;
};

/**
 * GCutDataError:
 * @GCUT_DATA_ERROR_NOT_EXIST: The field doesn't exist.
 *
 * Error codes returned by data related operation.
 *
 * Since: 1.0.6
 */
typedef enum
{
    GCUT_DATA_ERROR_NOT_EXIST
} GCutDataError;

GQuark         gcut_data_error_quark           (void);

GType          gcut_data__get_type             (void) G_GNUC_CONST;

GCutData      *gcut_data_new                   (const gchar   *first_field_name,
                                                ...) G_GNUC_NULL_TERMINATED;
GCutData      *gcut_data_new_va_list           (const gchar   *first_field_name,
                                                va_list         args);

gchar         *gcut_data_inspect               (GCutData      *data);
gboolean       gcut_data_equal                 (GCutData      *data1,
                                                GCutData      *data2);


const gchar   *gcut_data_get_string_with_error (GCutData      *data,
                                                const gchar   *field_name,
                                                GError       **error);
GType          gcut_data_get_type_with_error   (GCutData      *data,
                                                const gchar   *field_name,
                                                GError       **error);
guint          gcut_data_get_flags_with_error  (GCutData      *data,
                                                const gchar   *field_name,
                                                GError       **error);
gint           gcut_data_get_enum_with_error   (GCutData      *data,
                                                const gchar   *field_name,
                                                GError       **error);

const gchar   *gcut_data_get_string_helper     (const GCutData *data,
                                                const gchar   *field_name,
                                                CutCallbackFunction callback);
GType          gcut_data_get_type_helper       (const GCutData *data,
                                                const gchar   *field_name,
                                                CutCallbackFunction callback);
guint          gcut_data_get_flags_helper      (const GCutData *data,
                                                const gchar   *field_name,
                                                CutCallbackFunction callback);
gint           gcut_data_get_enum_helper       (const GCutData *data,
                                                const gchar   *field_name,
                                                CutCallbackFunction callback);

G_END_DECLS

#endif /* __GCUT_DATA_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
