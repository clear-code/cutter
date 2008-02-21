/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2006 Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_MODULE_IMPL_H__
#define __CUT_MODULE_IMPL_H__

#include <glib-object.h>

G_BEGIN_DECLS

#include "cut-module.h"

typedef GList   *(*CutModuleInitFunc)         (GTypeModule *module);
typedef void     (*CutModuleExitFunc)         (void);
typedef GObject *(*CutModuleInstantiateFunc)  (const gchar *first_property,
                                               va_list      var_args);
typedef gchar   *(*CutModuleGetLogDomainFunc) (void);

#define CUT_MODULE_IMPL_INIT           cut_module_impl_init
#define CUT_MODULE_IMPL_EXIT           cut_module_impl_exit
#define CUT_MODULE_IMPL_INSTANTIATE    cut_module_impl_instantiate
#define CUT_MODULE_IMPL_GET_LOG_DOMAIN cut_module_impl_get_log_domain


GList   *CUT_MODULE_IMPL_INIT           (GTypeModule  *module);
void     CUT_MODULE_IMPL_EXIT           (void);
GObject *CUT_MODULE_IMPL_INSTANTIATE    (const gchar *first_property,
                                         va_list      var_args);
gchar   *CUT_MODULE_IMPL_GET_LOG_DOMAIN (void);

G_END_DECLS

#endif /* __CUT_MODULE_IMPL_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
