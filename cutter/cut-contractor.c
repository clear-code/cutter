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

#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-factory-builder.h"
#include "cut-contractor.h"

#define CUT_CONTRACTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CONTRACTOR, CutContractorPrivate))

typedef struct _CutContractorPrivate    CutContractorPrivate;
struct _CutContractorPrivate
{
    GList *builders;
};

G_DEFINE_TYPE(CutContractor, cut_contractor, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

static void
cut_contractor_class_init (CutContractorClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutContractorPrivate));
}

static void
cut_contractor_init (CutContractor *builder)
{
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(builder);

    priv->builders = NULL;
}

static void
dispose (GObject *object)
{
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(object);

    if (priv->builders) {
        g_list_free(priv->builders);
        priv->builders = NULL;
    }

    G_OBJECT_CLASS(cut_contractor_parent_class)->dispose(object);
}

CutContractor *
cut_contractor_new (void)
{
    return CUT_CONTRACTOR(g_object_new(CUT_TYPE_CONTRACTOR, NULL));
}

gboolean
cut_contractor_has_builder (CutContractor *contractor, const gchar *type_name)
{
    GList *node;
    CutContractorPrivate *priv = CUT_CONTRACTOR_GET_PRIVATE(contractor);

    for (node = priv->builders; node; node = g_list_next(node)) {
        CutFactoryBuilder *builder = CUT_FACTORY_BUILDER(node->data);
        const gchar *name;
        name = cut_factory_builder_get_type_name(builder);
        if (!strcmp(type_name, name))
            return TRUE;
    }

    return FALSE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
