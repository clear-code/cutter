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

#ifndef __CUT_RUNNER_FACTORY_H__
#define __CUT_RUNNER_FACTORY_H__

#include <glib-object.h>

#include <cutter/cut-runner.h>

G_BEGIN_DECLS

#define CUT_TYPE_RUNNER_FACTORY            (cut_runner_factory_get_type ())
#define CUT_RUNNER_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUNNER_FACTORY, CutRunnerFactory))
#define CUT_RUNNER_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUNNER_FACTORY, CutRunnerFactoryClass))
#define CUT_IS_RUNNER_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUNNER_FACTORY))
#define CUT_IS_RUNNER_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUNNER_FACTORY))
#define CUT_RUNNER_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUNNER_FACTORY, CutRunnerFactoryClass))

typedef struct _CutRunnerFactory         CutRunnerFactory;
typedef struct _CutRunnerFactoryClass    CutRunnerFactoryClass;

struct _CutRunnerFactory
{
    GObject object;
};

struct _CutRunnerFactoryClass
{
    GObjectClass parent_class;

    void         (*set_option_group)    (CutRunnerFactory *factory,
                                         GOptionContext   *context);
    CutRunner   *(*create)              (CutRunnerFactory *factory);
};

GType           cut_runner_factory_get_type    (void) G_GNUC_CONST;

void            cut_runner_factory_init        (void);
void            cut_runner_factory_quit        (void);

const gchar    *cut_runner_factory_get_default_module_dir   (void);
void            cut_runner_factory_set_default_module_dir   (const gchar *dir);

void            cut_runner_factory_load        (const gchar *base_dir);
void            cut_runner_factory_unload      (void);
GList          *cut_runner_factory_get_registered_types (void);
GList          *cut_runner_factory_get_log_domains      (void);
GList          *cut_runner_factory_get_names   (void);

CutRunnerFactory *cut_runner_factory_new       (const gchar *name,
                                                const gchar *first_property,
                                                ...);

void         cut_runner_factory_set_option_group (CutRunnerFactory *factory,
                                                  GOptionContext   *context);
CutRunner   *cut_runner_factory_create           (CutRunnerFactory *factory);
const gchar *cut_runner_factory_get_name         (CutRunnerFactory *factory);

G_END_DECLS

#endif /* __CUT_RUNNER_FACTORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
