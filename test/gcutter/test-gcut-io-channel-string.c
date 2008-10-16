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

#include <gcutter.h>

void test_new(void);
void test_new_null(void);
void test_read_write(void);

static GIOChannel *channel;
static gchar *data;

void
setup (void)
{
    channel = NULL;
    data = NULL;
}

void
teardown (void)
{
    if (channel)
        g_io_channel_unref(channel);
    if (data)
        g_free(data);
}

void
test_new (void)
{
    gsize length;
    GError *error = NULL;

    channel = gcut_io_channel_string_new("XXX");
    cut_assert_equal_string("XXX",
                            gcut_string_io_channel_get_string(channel)->str);

    g_io_channel_read_to_end(channel, &data, &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_string("XXX", data);

    cut_assert_equal_string("XXX",
                            gcut_string_io_channel_get_string(channel)->str);
}

void
test_new_null (void)
{
    gchar buffer[1024];
    gsize length;
    GError *error = NULL;

    channel = gcut_io_channel_string_new(NULL);
    cut_assert_equal_string("",
                            gcut_string_io_channel_get_string(channel)->str);

    cut_assert_equal_int(G_IO_STATUS_EOF,
                         g_io_channel_read_chars(channel, buffer, sizeof(buffer),
                                                 &length, &error));
    gcut_assert_error(error);

    g_io_channel_read_to_end(channel, &data, &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_string("", data);

    cut_assert_equal_string("",
                            gcut_string_io_channel_get_string(channel)->str);
}

void
test_read_write (void)
{
    gchar buffer[1024];
    gchar write_data[] = "data";
    gsize length;
    GError *error = NULL;

    channel = gcut_io_channel_string_new(NULL);
    g_io_channel_set_encoding(channel, NULL, &error);
    gcut_assert_error(error);
    cut_assert_equal_string("",
                            gcut_string_io_channel_get_string(channel)->str);

    cut_assert_equal_int(G_IO_STATUS_EOF,
                         g_io_channel_read_chars(channel, buffer, sizeof(buffer),
                                                 &length, &error));
    gcut_assert_error(error);

    g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                             &length, &error);
    gcut_assert_error(error);

    g_io_channel_seek_position(channel, 0, G_SEEK_SET, &error);
    gcut_assert_error(error);

    g_io_channel_read_to_end(channel, &data, &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_string(write_data, data);

    cut_assert_equal_string(write_data,
                            gcut_string_io_channel_get_string(channel)->str);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
