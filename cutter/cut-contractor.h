/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2014  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_CONTRACTOR_H__
#define __CUT_CONTRACTOR_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_CONTRACTOR            (cut_contractor_get_type ())
#define CUT_CONTRACTOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_CONTRACTOR, CutContractor))
#define CUT_CONTRACTOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_CONTRACTOR, CutContractorClass))
#define CUT_IS_CONTRACTOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_CONTRACTOR))
#define CUT_IS_CONTRACTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_CONTRACTOR))
#define CUT_CONTRACTOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_CONTRACTOR, CutContractorClass))

typedef struct _CutContractor         CutContractor;
typedef struct _CutContractorClass    CutContractorClass;

struct _CutContractor
{
    GObject object;
};

struct _CutContractorClass
{
    GObjectClass parent_class;
};

GType          cut_contractor_get_type    (void) G_GNUC_CONST;

CutContractor *cut_contractor_new         (void);

GList         *cut_contractor_build_listener_factories
                                                 (CutContractor *contractor);
GList         *cut_contractor_build_all_listener_factories
                                                 (CutContractor *contractor);
GList         *cut_contractor_build_all_loader_customizer_factories
                                                 (CutContractor *contractor);
gboolean       cut_contractor_has_listener_builder
                                                 (CutContractor *contractor,
                                                  const gchar *type_name);
void           cut_contractor_set_option_context (CutContractor *contractor,
                                                  GOptionContext *context);


G_END_DECLS

#endif /* __CUT_CONTRACTOR_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
