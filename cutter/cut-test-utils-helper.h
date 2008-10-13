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

#ifndef __CUT_TEST_UTILS_HELPER_H__
#define __CUT_TEST_UTILS_HELPER_H__

#include <cutter/cut-public.h>

#ifdef __cplusplus
extern "C" {
#endif

#define cut_take_helper(test_context, string, destroy_function)     \
    cut_test_context_take(test_context, (object), destroy_function)

#define cut_take_memory_helper(test_context, object)            \
    cut_test_context_take_memory(test_context, (object))

#define cut_take_string_helper(test_context, string)            \
    cut_test_context_take_string(test_context, (string))

#define cut_take_strdup_helper(test_context, string)            \
    cut_test_context_take_strdup(test_context, (string))

#define cut_take_printf_helper(test_context, format, ...)               \
    cut_test_context_take_printf(test_context, (format), __VA_ARGS__)

#define cut_append_diff_helper(test_context, message, from, to)         \
    cut_take_string_helper(test_context,                                \
                           cut_utils_append_diff(message, from, to))

#define cut_inspect_string_array_helper(test_context, strings)          \
    cut_take_string_helper(test_context,                                \
                           cut_utils_inspect_string_array(strings))

#ifdef __cplusplus
}
#endif

#endif /* __CUT_TEST_UTILS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
