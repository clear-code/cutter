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

#include <errno.h>
#include <gcutter.h>

void test_new(void);
void test_new_null(void);
void test_read_write(void);
void test_clear(void);
void test_source(void);
void test_buffer_limit_block(void);
void test_buffer_limit_non_block(void);
void test_limit(void);

static GIOChannel *channel;
static gchar *data;
static guint watch_id;
static guint timeout_id;
static GError *expected_error;
static GError *actual_error;

void
setup (void)
{
    channel = NULL;
    data = NULL;
    watch_id = 0;
    timeout_id = 0;
    expected_error = NULL;
    actual_error = NULL;
}

void
teardown (void)
{
    if (channel) {
        gcut_string_io_channel_set_buffer_limit(channel, 0);
        gcut_string_io_channel_set_limit(channel, 0);
        g_io_channel_unref(channel);
    }
    if (data)
        g_free(data);
    if (watch_id > 0)
        g_source_remove(watch_id);
    if (timeout_id > 0)
        g_source_remove(timeout_id);

    if (expected_error)
        g_error_free(expected_error);
    if (actual_error)
        g_error_free(actual_error);
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

void
test_clear (void)
{
    gsize length;
    GError *error = NULL;

    channel = gcut_io_channel_string_new("XXX");
    cut_assert_equal_string("XXX",
                            gcut_string_io_channel_get_string(channel)->str);

    gcut_string_io_channel_clear(channel);
    cut_assert_equal_string("",
                            gcut_string_io_channel_get_string(channel)->str);

    g_io_channel_read_to_end(channel, &data, &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_string("", data);
}

static gboolean
watch_func (GIOChannel *channel, GIOCondition condition, gpointer data)
{
    GIOCondition *target_condition = data;

    *target_condition = condition;

    return FALSE;
}

void
test_source (void)
{
    gchar write_data[] = "data";
    gsize length;
    GIOCondition target_condition = 0;
    GError *error = NULL;

    channel = gcut_io_channel_string_new(NULL);
    g_io_channel_set_encoding(channel, NULL, &error);
    gcut_assert_error(error);

    watch_id = g_io_add_watch(channel,
                              G_IO_IN | G_IO_PRI |
                              G_IO_ERR | G_IO_HUP | G_IO_NVAL,
                              watch_func, &target_condition);

    g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                             &length, &error);
    gcut_assert_error(error);

    cut_assert_equal_uint(0, target_condition);
    g_main_context_iteration(NULL, FALSE);
    cut_assert_equal_uint(0, target_condition);

    g_io_channel_seek_position(channel, 0, G_SEEK_SET, &error);
    gcut_assert_error(error);
    g_main_context_iteration(NULL, FALSE);
    cut_assert_equal_uint(G_IO_IN | G_IO_PRI, target_condition);
}

static gboolean
cb_timeout_detect (gpointer user_data)
{
    gboolean *timed_out = user_data;
    gsize limit;

    *timed_out = TRUE;
    limit = gcut_string_io_channel_get_buffer_limit(channel);
    gcut_string_io_channel_set_buffer_limit(channel, limit + 1);
    return FALSE;
}
void
test_buffer_limit_block (void)
{
    gchar write_data[] = "data\n";
    gsize length;
    GIOStatus status;
    gboolean timed_out = FALSE;
    GError *error = NULL;

    channel = gcut_io_channel_string_new(NULL);
    g_io_channel_set_encoding(channel, NULL, &error);
    gcut_assert_error(error);
    g_io_channel_set_buffered(channel, FALSE);

    gcut_string_io_channel_set_buffer_limit(channel, sizeof(write_data));
    status = g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                                      &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_uint(G_IO_STATUS_NORMAL, status);
    cut_assert_equal_size(sizeof(write_data), length);


    g_io_channel_seek_position(channel, 0, G_SEEK_SET, &error);
    gcut_assert_error(error);

    timeout_id = g_timeout_add(100, cb_timeout_detect, &timed_out);
    gcut_string_io_channel_set_buffer_limit(channel, length * 2 -1);
    status = g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                                      &length, &error);
    gcut_assert_error(error);

    cut_assert_equal_uint(G_IO_STATUS_NORMAL, status);
    cut_assert_equal_size(sizeof(write_data), length);
    cut_assert_true(timed_out);
}

void
test_buffer_limit_non_block (void)
{
    gchar write_data[] = "data\n";
    gsize length;
    gsize delta = 2;
    GIOStatus status;
    GError *error = NULL;

    channel = gcut_io_channel_string_new(NULL);
    g_io_channel_set_encoding(channel, NULL, &error);
    gcut_assert_error(error);
    g_io_channel_set_buffered(channel, FALSE);

    g_io_channel_set_flags(channel, G_IO_FLAG_NONBLOCK, &error);
    gcut_assert_error(error);

    gcut_string_io_channel_set_buffer_limit(channel, sizeof(write_data));
    status = g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                                      &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_uint(G_IO_STATUS_NORMAL, status);
    cut_assert_equal_size(sizeof(write_data), length);


    gcut_string_io_channel_set_buffer_limit(channel, delta);
    status = g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                                      &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_uint(G_IO_STATUS_NORMAL, status);
    cut_assert_equal_size(delta, length);


    g_io_channel_seek_position(channel, 0, G_SEEK_SET, &error);
    gcut_assert_error(error);

    status = g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                                      &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_uint(G_IO_STATUS_AGAIN, status);
    cut_assert_equal_size(0, length);
}

void
test_limit (void)
{
    gchar write_data[] = "data\n";
    gsize length;
    GIOStatus status;
    GError *error = NULL;

    channel = gcut_io_channel_string_new(NULL);
    g_io_channel_set_encoding(channel, NULL, &error);
    gcut_assert_error(error);
    g_io_channel_set_buffered(channel, FALSE);

    gcut_string_io_channel_set_limit(channel, sizeof(write_data) + 1);

    status = g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                                      &length, &error);
    gcut_assert_error(error);
    cut_assert_equal_uint(G_IO_STATUS_NORMAL, status);
    cut_assert_equal_size(sizeof(write_data), length);


    status = g_io_channel_write_chars(channel, write_data, sizeof(write_data),
                                      &length, &actual_error);
    expected_error = g_error_new(G_IO_CHANNEL_ERROR,
                                 g_io_channel_error_from_errno(ENOSPC),
                                 g_strerror(ENOSPC));
    gcut_assert_equal_error(expected_error, actual_error);
    cut_assert_equal_uint(G_IO_STATUS_ERROR, status);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
