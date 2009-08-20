/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

#ifndef __SOUPCUT_ASSERTIONS_H__
#define __SOUPCUT_ASSERTIONS_H__

#include <glib.h>

#include <soupcutter/soupcut-assertions-helper.h>

G_BEGIN_DECLS

/**
 * SECTION: soupcut-assertions
 * @title: Assertions with libsoup support
 * @short_description: Checks that your program works as you
 * expect with libsoup support.
 *
 */

/**
 * soupcut_message_assert_equal_content_type:
 * @expected: an expected content-type.
 * @actual: an actual #SoupMessage.
 *
 * Passes if @expected == @actual->response_headers->content_type.
 *
 * Since: 1.0.8
 */
#define soupcut_message_assert_equal_content_type(expected, actual) do  \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        soupcut_message_assert_equal_content_type_helper(expected, actual, \
                                                 #expected, #actual),   \
        soupcut_message_assert_equal_content_type(expected, actual));   \
} while (0)

/**
 * soupcut_client_assert_equal_content_type:
 * @expected: an expected content-type.
 * @actual: an actual #SoupCutClient.
 *
 * Passes if @expected == content_type of the latest message of @actual.
 *
 * Since: 1.0.8
 */
#define soupcut_client_assert_equal_content_type(expected, actual) do  \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        soupcut_client_assert_equal_content_type_helper(expected, actual, \
                                                 #expected, #actual),   \
        soupcut_client_assert_equal_content_type(expected, actual));   \
} while (0)


/**
 * soupcut_client_assert_response:
 * @client: an #SoupCutClient
 *
 * Passes if status code of @client == 2XX.
 *
 * Since: 1.0.8
 */
#define soupcut_client_assert_response(client) do               \
{                                                               \
    cut_trace_with_info_expression(                             \
        soupcut_client_assert_response_helper(client, #client), \
        soupcut_client_assert_response(client));                \
} while (0)

G_END_DECLS

#endif /* __SOUPCUT_ASSERTIONS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
