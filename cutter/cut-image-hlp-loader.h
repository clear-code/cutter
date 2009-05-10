/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_IMAGE_HLP_LOADER_H__
#define __CUT_IMAGE_HLP_LOADER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_IMAGE_HLP_LOADER            (cut_image_hlp_loader_get_type ())
#define CUT_IMAGE_HLP_LOADER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_IMAGE_HLP_LOADER, CutImageHlpLoader))
#define CUT_IMAGE_HLP_LOADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_IMAGE_HLP_LOADER, CutImageHlpLoaderClass))
#define CUT_IS_IMAGE_HLP_LOADER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_IMAGE_HLP_LOADER))
#define CUT_IS_IMAGE_HLP_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_IMAGE_HLP_LOADER))
#define CUT_IMAGE_HLP_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_IMAGE_HLP_LOADER, CutImageHlpLoaderClass))

typedef struct _CutImageHlpLoader      CutImageHlpLoader;
typedef struct _CutImageHlpLoaderClass CutImageHlpLoaderClass;

struct _CutImageHlpLoader
{
    GObject object;
};

struct _CutImageHlpLoaderClass
{
    GObjectClass parent_class;
};

GType              cut_image_hlp_loader_get_type          (void) G_GNUC_CONST;

CutImageHlpLoader *cut_image_hlp_loader_new               (const gchar    *so_filename);

gboolean           cut_image_hlp_loader_is_dll            (CutImageHlpLoader *loader);

gboolean           cut_image_hlp_loader_support_attribute (CutImageHlpLoader *loader);
GList             *cut_image_hlp_loader_collect_symbols   (CutImageHlpLoader *loader);

G_END_DECLS

#endif /* __CUT_IMAGE_HLP_LOADER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
