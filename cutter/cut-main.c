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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <glib.h>

#include "cut.h"
#include "cut-test-suite.h"
#include "cut-repository.h"

GPrivate *private_thread_context = NULL;

static void
private_thread_context_cleanup (gpointer data)
{

}

int
main (int argc, char* argv[])
{
    CutTestSuite *suite;
    CutRepository *repository;

    g_type_init();

    g_thread_init(NULL);

    private_thread_context = g_private_new(private_thread_context_cleanup);

    repository = cut_repository_new(argv[1]);
    suite = cut_repository_create_test_suite(repository);

    if (suite) {
        cut_test_run(CUT_TEST(suite));
        g_object_unref(suite);
    }
    g_object_unref(repository);

    exit(0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
