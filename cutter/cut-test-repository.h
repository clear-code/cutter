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

#ifndef __CUT_TEST_REPOSITORY_H__
#define __CUT_TEST_REPOSITORY_H__

#include <glib-object.h>

#include "cut-test.h"
#include "cut-test-case.h"

G_BEGIN_DECLS

#define CUT_TYPE_TEST_REPOSITORY            (cut_test_repository_get_type ())
#define CUT_TEST_REPOSITORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_REPOSITORY, CutTestRepository))
#define CUT_TEST_REPOSITORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_REPOSITORY, CutTestRepositoryClass))
#define CUT_IS_TEST_REPOSITORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_REPOSITORY))
#define CUT_IS_TEST_REPOSITORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_REPOSITORY))
#define CUT_TEST_REPOSITORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_REPOSITORY, CutTestRepositoryClass))

typedef struct _CutTestRepository      CutTestRepository;
typedef struct _CutTestRepositoryClass CutTestRepositoryClass;

struct _CutTestRepository
{
    GObject object;
};

struct _CutTestRepositoryClass
{
    GObjectClass parent_class;
};

GType cut_test_repository_get_type  (void) G_GNUC_CONST;

CutTestRepository *cut_test_repository_new (const gchar *dirname);

G_END_DECLS

#endif /* __CUT_TEST_REPOSITORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
