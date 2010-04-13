/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_TEST_UTILS_HELPER_H__
#define __CUT_TEST_UTILS_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cutter/cut-types.h>

const char *cut_take_replace_helper (const char          *target,
                                     const char          *pattern,
                                     const char          *replacement,
                                     CutCallbackFunction  callback);

const char *cut_take_convert_helper (const char          *string,
                                     const char          *to_code_set,
                                     const char          *from_code_set,
                                     CutCallbackFunction  callback);


#ifdef __cplusplus
}
#endif

#endif /* __CUT_TEST_UTILS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
