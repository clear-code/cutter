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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <errno.h>

#include "gcut-string-io-channel.h"

typedef struct _GCutStringIOChannel GCutStringIOChannel;
typedef struct _GCutStringSource GCutStringSource;

struct _GCutStringIOChannel
{
    GIOChannel channel;
    GString *string;
    gsize write_offset;
    gsize read_offset;
    gsize buffer_limit;
    gsize limit;
    gboolean read_fail;
    GIOFlags flags;
    gboolean reach_eof;
    gboolean pipe_mode;
};

struct _GCutStringSource
{
    GSource       source;
    GIOChannel   *channel;
    GIOCondition  condition;
    GIOCondition  available_condition;
};

static gboolean
source_is_available (GSource *source)
{
    GCutStringSource *string_source = (GCutStringSource *)source;
    GIOChannel *channel;
    GCutStringIOChannel *string_channel;
    GIOCondition buffer_condition;
    GIOCondition all_input_condition, all_error_condition;

    channel = string_source->channel;
    string_channel = (GCutStringIOChannel *)(channel);
    string_source->available_condition = 0;
    buffer_condition = g_io_channel_get_buffer_condition(channel);

    all_input_condition = (G_IO_IN | G_IO_PRI);
    if (string_source->condition & all_input_condition &&
        channel->is_readable) {
        string_source->available_condition |= buffer_condition;
        if (string_channel->string &&
            ((string_channel->string->len > string_channel->read_offset) ||
             (!string_channel->pipe_mode &&
              string_channel->string->len == string_channel->read_offset &&
              !string_channel->reach_eof)))
            string_source->available_condition |= all_input_condition;
    }

    if (string_source->condition & G_IO_OUT && channel->is_writeable)
        string_source->available_condition |= G_IO_OUT;

    all_error_condition = (G_IO_ERR | G_IO_HUP | G_IO_NVAL);
    if (string_source->condition & all_error_condition) {
        string_source->available_condition |= buffer_condition;
        if (string_channel->string == NULL)
            string_source->available_condition |= all_error_condition;
    }

    return string_source->available_condition > 0;
}

static gboolean
gcut_source_string_prepare (GSource *source, gint *timeout)
{
    *timeout = -1;
    return source_is_available(source);
}

static gboolean
gcut_source_string_check (GSource *source)
{
    return source_is_available(source);
}

static gboolean
gcut_source_string_dispatch (GSource *source, GSourceFunc callback,
                             gpointer user_data)

{
    GCutStringSource *string_source = (GCutStringSource *)source;
    GIOFunc func = (GIOFunc)callback;

    if (func)
        return func(string_source->channel,
                    string_source->available_condition &
                    string_source->condition,
                    user_data);
    else
        return FALSE;
}

static void
gcut_source_string_finalize (GSource *source)
{
    GCutStringSource *string_source = (GCutStringSource *)source;

    g_io_channel_unref(string_source->channel);
}

static GSourceFuncs gcut_source_string_funcs = {
    gcut_source_string_prepare,
    gcut_source_string_check,
    gcut_source_string_dispatch,
    gcut_source_string_finalize
};


static GIOStatus
gcut_string_io_channel_read (GIOChannel *channel, gchar *buf, gsize count,
                             gsize *bytes_read, GError **error)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;
    gsize rest;

    if (string_channel->read_fail) {
        *bytes_read = 0;
        g_set_error(error,
                    G_IO_CHANNEL_ERROR,
                    g_io_channel_error_from_errno(EIO),
                    "%s", g_strerror(EIO));
        return G_IO_STATUS_ERROR;
    }

    rest = string_channel->string->len - string_channel->read_offset;
    *bytes_read = MIN(count, rest);
    memcpy(buf,
           string_channel->string->str + string_channel->read_offset,
           *bytes_read);
    string_channel->read_offset += *bytes_read;
    if (!string_channel->pipe_mode)
        string_channel->write_offset += *bytes_read;

    if (string_channel->string->len > string_channel->read_offset ||
        (string_channel->string->len == string_channel->read_offset &&
         *bytes_read > 0)) {
        string_channel->reach_eof = FALSE;
        return G_IO_STATUS_NORMAL;
    } else {
        if (string_channel->pipe_mode &&
            string_channel->flags & G_IO_FLAG_NONBLOCK) {
            string_channel->reach_eof = FALSE;
            return G_IO_STATUS_AGAIN;
        } else {
            string_channel->reach_eof = TRUE;
            return G_IO_STATUS_EOF;
        }
    }
}

static GIOStatus
determine_write_size (GCutStringIOChannel *string_channel,
                      gsize size, gsize *write_size)
{
    while (TRUE) {
        gsize buffer_size, available_size;

        if (string_channel->buffer_limit == 0) {
            *write_size = size;
            break;
        }

        if (string_channel->string->len > string_channel->write_offset) {
            buffer_size =
                string_channel->string->len - string_channel->write_offset;
        } else {
            buffer_size = 0;
        }

        if (string_channel->buffer_limit >= buffer_size) {
            available_size = string_channel->buffer_limit - buffer_size;
        } else {
            available_size = 0;
        }

        if (string_channel->flags & G_IO_FLAG_NONBLOCK) {
            if (available_size > 0) {
                *write_size = MIN(size, available_size);
                break;
            } else {
                return G_IO_STATUS_AGAIN;
            }
        } else {
            if (available_size >= size) {
                *write_size = size;
                break;
            }
        }

        g_main_context_iteration(NULL, TRUE);
    }

    return G_IO_STATUS_NORMAL;
}

static GIOStatus
gcut_string_io_channel_write (GIOChannel *channel, const gchar *buf, gsize count,
                              gsize *bytes_written, GError **error)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;
    GIOStatus status;
    gsize write_size = 0;

    *bytes_written = 0;

    status = determine_write_size(string_channel, count, &write_size);
    if (status != G_IO_STATUS_NORMAL)
        return status;

    while (0 < string_channel->limit &&
           string_channel->limit < string_channel->string->len + write_size) {
        if (string_channel->flags & G_IO_FLAG_NONBLOCK) {
            g_set_error(error,
                        G_IO_CHANNEL_ERROR,
                        g_io_channel_error_from_errno(ENOSPC),
                        "%s", g_strerror(ENOSPC));
            return G_IO_STATUS_ERROR;
        } else {
            g_main_context_iteration(NULL, TRUE);
        }
    }

    g_string_overwrite_len(string_channel->string, string_channel->write_offset,
                           buf, write_size);
    *bytes_written = write_size;
    string_channel->write_offset += write_size;
    if (!string_channel->pipe_mode)
        string_channel->read_offset += write_size;

    return G_IO_STATUS_NORMAL;
}

static GIOStatus
gcut_string_io_channel_seek (GIOChannel *channel, gint64 offset,
                             GSeekType type, GError **error)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    switch (type) {
      case G_SEEK_SET:
        string_channel->write_offset = offset;
        string_channel->read_offset = offset;
        break;
      case G_SEEK_CUR:
        string_channel->write_offset += offset;
        string_channel->read_offset += offset;
        break;
      case G_SEEK_END:
        string_channel->write_offset = string_channel->string->len + offset;
        string_channel->read_offset = string_channel->string->len + offset;
        break;
      default:
        g_set_error(error, G_IO_CHANNEL_ERROR,
                    g_io_channel_error_from_errno(EINVAL),
                    "%s", g_strerror(EINVAL));
        return G_IO_STATUS_ERROR;
        break;
    }

    string_channel->reach_eof = FALSE;

    return G_IO_STATUS_NORMAL;
}


static GIOStatus
gcut_string_io_channel_close (GIOChannel *channel, GError **err)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    g_string_free(string_channel->string, TRUE);
    string_channel->string = NULL;
    string_channel->write_offset = 0;
    string_channel->read_offset = 0;

    return G_IO_STATUS_NORMAL;
}

static GSource *
gcut_string_io_channel_create_watch (GIOChannel *channel, GIOCondition condition)
{
    GSource *source;
    GCutStringSource *string_source;

    source = g_source_new(&gcut_source_string_funcs, sizeof(GCutStringSource));
    string_source = (GCutStringSource *)source;

    string_source->channel = channel;
    g_io_channel_ref(channel);

    string_source->condition = condition;

    return source;
}

static void
gcut_string_io_channel_free (GIOChannel *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    if (string_channel->string)
        g_string_free(string_channel->string, TRUE);
    g_free(string_channel);
}

static GIOStatus
gcut_string_io_channel_set_flags (GIOChannel *channel, GIOFlags flags,
                                  GError **error)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    string_channel->flags = flags;

    return G_IO_STATUS_NORMAL;
}

static GIOFlags
gcut_string_io_channel_get_flags (GIOChannel *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    return string_channel->flags;
}

static GIOFuncs gcut_string_io_channel_funcs = {
    gcut_string_io_channel_read,
    gcut_string_io_channel_write,
    gcut_string_io_channel_seek,
    gcut_string_io_channel_close,
    gcut_string_io_channel_create_watch,
    gcut_string_io_channel_free,
    gcut_string_io_channel_set_flags,
    gcut_string_io_channel_get_flags
};

GIOChannel *
gcut_string_io_channel_new (const gchar *initial)
{
    GIOChannel *channel;
    GCutStringIOChannel *string_channel;

    string_channel = g_new(GCutStringIOChannel, 1);
    channel = (GIOChannel *)string_channel;

    g_io_channel_init(channel);
    channel->funcs = &gcut_string_io_channel_funcs;
    channel->is_readable = TRUE;
    channel->is_writeable = TRUE;
    channel->is_seekable = TRUE;

    string_channel->string = g_string_new(initial);
    string_channel->write_offset = string_channel->string->len;
    string_channel->read_offset = 0;
    string_channel->buffer_limit = 0;
    string_channel->limit = 0;
    string_channel->read_fail = FALSE;
    string_channel->flags = 0;
    string_channel->reach_eof = FALSE;
    string_channel->pipe_mode = FALSE;

    return channel;
}

GString *
gcut_string_io_channel_get_string (GIOChannel *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    return string_channel->string;
}

void
gcut_string_io_channel_clear (GIOChannel  *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    g_string_truncate(string_channel->string, 0);
    string_channel->write_offset = 0;
    string_channel->read_offset = 0;
}

gsize
gcut_string_io_channel_get_buffer_limit (GIOChannel *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    return string_channel->buffer_limit;
}

void
gcut_string_io_channel_set_buffer_limit (GIOChannel *channel, gsize limit)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    string_channel->buffer_limit = limit;
}

gsize
gcut_string_io_channel_get_limit (GIOChannel *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    return string_channel->limit;
}

void
gcut_string_io_channel_set_limit (GIOChannel *channel, gsize limit)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    string_channel->limit = limit;
}

gboolean
gcut_string_io_channel_get_read_fail (GIOChannel *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    return string_channel->read_fail;
}

void
gcut_string_io_channel_set_read_fail (GIOChannel *channel, gboolean read_fail)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    string_channel->read_fail = read_fail;
}

gboolean
gcut_string_io_channel_get_pipe_mode (GIOChannel *channel)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    return string_channel->pipe_mode;
}

void
gcut_string_io_channel_set_pipe_mode (GIOChannel *channel, gboolean pipe_mode)
{
    GCutStringIOChannel *string_channel = (GCutStringIOChannel *)channel;

    string_channel->pipe_mode = pipe_mode;
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
