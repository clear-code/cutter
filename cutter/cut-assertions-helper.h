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

#ifndef __CUT_ASSERTIONS_HELPER_H__
#define __CUT_ASSERTIONS_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define cut_inspect_string_array(strings)                               \
    cut_test_context_inspect_string_array(get_current_test_context(),   \
                                          strings)

#define cut_take_string(string)                                         \
    cut_test_context_take_string(get_current_test_context(), string)

#define cut_test_pass() \
    cut_test_context_pass_assertion(get_current_test_context())

#define cut_test_register_result(status, ...) do            \
{                                                           \
    cut_test_context_register_result(                       \
        get_current_test_context(),                         \
        CUT_TEST_RESULT_ ## status,                         \
        __PRETTY_FUNCTION__, __FILE__, __LINE__,            \
        __VA_ARGS__, NULL);                                 \
} while (0)

#define cut_test_fail(status, ...) do                               \
{                                                                   \
    cut_test_register_result(status, ## __VA_ARGS__);               \
    cut_test_context_long_jump(get_current_test_context());         \
} while (0)


#ifdef __cplusplus
}
#endif

#endif /* __CUT_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
