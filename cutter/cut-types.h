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

#ifndef __CUT_TYPES_H__
#define __CUT_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION: cut-types
 * @title: Types
 * @short_description: Types that is used in test.
 *
 * There are some types to be used in test.
 */

/**
 * CutDestroyFunction:
 * @data: the data element to be destroyed.
 *
 * Specifies the type of function which is called when a
 * data element is destroyed. It is passed the pointer to
 * the data element and should free any memory and resources
 * allocated for it.
 */
typedef void   (*CutDestroyFunction)           (void *data);

#ifdef __cplusplus
}
#endif

#endif /* __CUT_TYPES_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
