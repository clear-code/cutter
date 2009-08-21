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

#ifndef __SOUPCUT_ASSERTIONS_HELPER_H__
#define __SOUPCUT_ASSERTIONS_HELPER_H__

#include <glib.h>
#include <libsoup/soup.h>

#include <gcutter/gcut-assertions-helper.h>
#include <soupcutter/soupcut-client.h>

G_BEGIN_DECLS

void soupcut_message_assert_equal_content_type_helper (const gchar       *expected,
                                                       SoupMessage *actual,
                                                       const gchar       *expression_expected,
                                                       const gchar       *expression_actual);

void soupcut_client_assert_equal_content_type_helper (const gchar         *expected,
                                                      SoupCutClient *actual,
                                                      const gchar         *expression_expected,
                                                      const gchar         *expression_actual);

void soupcut_client_assert_response_helper (SoupCutClient *client,
                                            const gchar   *expression_client);

void soupcut_client_assert_equal_body_helper (const gchar   *expected,
                                              SoupCutClient *client,
                                              const gchar   *expression_expected,
                                              const gchar   *expression_client);

void soupcut_client_assert_match_body_helper (const gchar   *pattern,
                                              SoupCutClient *client,
                                              const gchar   *expression_pattern,
                                              const gchar   *expression_client);

G_END_DECLS

#endif /* __SOUPCUT_ASSERTIONS_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
