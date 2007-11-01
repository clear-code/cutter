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

#ifndef __CUT_ASSERTIONS_H__
#define __CUT_ASSERTIONS_H__

#include <glib.h>
#include <string.h>

G_BEGIN_DECLS

#define cut_assert_equal_int(expect, actual)            \
if (expect != actual) {                                 \
    g_print("%s expected: %d but was: %d\n",            \
            __PRETTY_FUNCTION__, expect, actual);       \
} else {                                                \
}

#define cut_assert_equal_string(expect, actual)         \
if (strcmp(expect, actual))           {                 \
    g_print("%s expected: %s but was: %s\n",            \
            __PRETTY_FUNCTION__, expect, actual);       \
} else {                                                \
}

G_END_DECLS

#endif /* __CUT_ASSERTIONS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
