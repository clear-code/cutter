/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_ASSERTIONS_HELPER_H__
#define __CUT_ASSERTIONS_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define cut_test_pass()                                         \
    cut_test_context_pass_assertion(get_current_test_context())

#define cut_test_register_result(status, message, ...) do               \
{                                                                       \
    cut_test_context_register_result(                                   \
        get_current_test_context(),                                     \
        CUT_TEST_RESULT_ ## status,                                     \
        __PRETTY_FUNCTION__, __FILE__, __LINE__,                        \
        message, ## __VA_ARGS__, NULL);                                 \
} while (0)

#define cut_test_fail(status, message, ...) do                      \
{                                                                   \
    cut_test_register_result(status, message, ## __VA_ARGS__);      \
    cut_test_context_long_jump(get_current_test_context());         \
} while (0)

#ifdef __cplusplus
}
#endif

#endif /* __CUT_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
