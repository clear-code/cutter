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

#ifndef __CUT_MACROS_H__
#define __CUT_MACROS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#  define CUT_GNUC_PRINTF(format_index, arg_index)                      \
    __attribute__((__format__ (__printf__, format_index, arg_index)))
#  define CUT_GNUC_NORETURN                     \
    __attribute__((__noreturn__))
#else
#  define CUT_GNUC_PRINTF(format_index, arg_index)
#  define CUT_GNUC_NORETURN
#endif

#if __GNUC__ >= 4
#  define CUT_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#else
#  define CUT_GNUC_NULL_TERMINATED
#endif

#ifdef __GNUC__
#  define CUT_FUNCTION __PRETTY_FUNCTION__
#else
#  define CUT_FUNCTION __FUNCTION__
#endif


#ifdef __cplusplus
}
#endif

#endif /* __CUT_MACROS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
