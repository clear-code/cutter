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
#  include "../cutter/config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <errno.h>

#include "gcut-io-channel-string.h"

typedef struct _GCutIOChannelString GCutIOChannelString;
typedef struct _GCutSourceString GCutSourceString;

struct _GCutIOChannelString
{
    GIOChannel channel;
    GString *string;
    gsize offset;
    GIOFlags flags;
};

struct _GCutSourceString
{
    GSource       source;
    GIOChannel   *channel;
    GIOCondition  condition;
    GIOCondition  available_condition;
};

static gboolean
source_is_available (GSource *source)
{
    GCutSourceString *string_source = (GCutSourceString *)string_source;
    GCutIOChannelString *channel;

    channel = (GCutIOChannelString *)(string_source->channel);
    string_source->available_condition = 0;

    if (string_source->condition & (G_IO_IN | G_IO_PRI)  &&
        channel->string &&
        channel->string->len > channel->offset)
        string_source->available_condition |= G_IO_IN | G_IO_PRI;

    if (string_source->condition & G_IO_OUT)
        string_source->available_condition |= G_IO_OUT;

    if (string_source->condition & (G_IO_ERR | G_IO_HUP | G_IO_NVAL) &&
        channel->string == NULL)
        string_source->available_condition |= G_IO_ERR | G_IO_HUP | G_IO_NVAL;

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
    GCutSourceString *string_source = (GCutSourceString *)source;
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
    GCutSourceString *string_source = (GCutSourceString *)source;

    g_io_channel_unref(string_source->channel);
}

static GSourceFuncs gcut_source_string_funcs = {
    gcut_source_string_prepare,
    gcut_source_string_check,
    gcut_source_string_dispatch,
    gcut_source_string_finalize
};


static GIOStatus
gcut_io_channel_string_read (GIOChannel *channel, gchar *buf, gsize count,
                             gsize *bytes_read, GError **error)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;
    gsize rest;

    rest = string_channel->string->len - string_channel->offset;
    *bytes_read = MIN(count, rest);
    memcpy(buf,
           string_channel->string->str + string_channel->offset,
           *bytes_read);
    string_channel->offset += *bytes_read;

    if (string_channel->string->len > string_channel->offset ||
        (string_channel->string->len == string_channel->offset &&
         *bytes_read > 0))
        return G_IO_STATUS_NORMAL;
    else
        return G_IO_STATUS_EOF;
}

static GIOStatus
gcut_io_channel_string_write (GIOChannel *channel, const gchar *buf, gsize count,
                              gsize *bytes_written, GError **error)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;

    g_string_overwrite_len(string_channel->string, string_channel->offset,
                           buf, count);
    *bytes_written = count;
    string_channel->offset += count;

    return G_IO_STATUS_NORMAL;
}

static GIOStatus
gcut_io_channel_string_seek (GIOChannel *channel, gint64 offset,
                             GSeekType type, GError **error)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;

    switch (type) {
      case G_SEEK_SET:
        string_channel->offset = offset;
        break;
      case G_SEEK_CUR:
        string_channel->offset += offset;
        break;
      case G_SEEK_END:
        string_channel->offset = string_channel->string->len + offset;
        break;
      default:
        g_set_error(error, G_IO_CHANNEL_ERROR,
                    g_io_channel_error_from_errno(EINVAL),
                    "%s", g_strerror(EINVAL));
        return G_IO_STATUS_ERROR;
        break;
    }

    return G_IO_STATUS_NORMAL;
}


static GIOStatus
gcut_io_channel_string_close (GIOChannel *channel, GError **err)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;

    g_string_free(string_channel->string, TRUE);
    string_channel->string = NULL;
    string_channel->offset = 0;

    return G_IO_STATUS_NORMAL;
}

static GSource *
gcut_io_channel_string_create_watch (GIOChannel *channel, GIOCondition condition)
{
    GSource *source;
    GCutSourceString *string_source;

    source = g_source_new(&gcut_source_string_funcs, sizeof(GCutSourceString));
    string_source = (GCutSourceString *)source;

    string_source->channel = channel;
    g_io_channel_ref(channel);

    string_source->condition = condition;

    return source;
}

static void
gcut_io_channel_string_free (GIOChannel *channel)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;

    g_string_free(string_channel->string, TRUE);
    g_free(string_channel);
}

static GIOStatus
gcut_io_channel_string_set_flags (GIOChannel *channel, GIOFlags flags,
                                  GError **error)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;

    string_channel->flags = flags;

    return G_IO_STATUS_NORMAL;
}

static GIOFlags
gcut_io_channel_string_get_flags (GIOChannel *channel)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;

    return string_channel->flags;
}

static GIOFuncs gcut_io_channel_string_funcs = {
    gcut_io_channel_string_read,
    gcut_io_channel_string_write,
    gcut_io_channel_string_seek,
    gcut_io_channel_string_close,
    gcut_io_channel_string_create_watch,
    gcut_io_channel_string_free,
    gcut_io_channel_string_set_flags,
    gcut_io_channel_string_get_flags
};

GIOChannel *
gcut_io_channel_string_new (const gchar *initial)
{
    GIOChannel *channel;
    GCutIOChannelString *string_channel;

    string_channel = g_new(GCutIOChannelString, 1);
    channel = (GIOChannel *)string_channel;

    g_io_channel_init(channel);
    channel->funcs = &gcut_io_channel_string_funcs;
    channel->is_readable = TRUE;
    channel->is_writeable = TRUE;
    channel->is_seekable = TRUE;

    string_channel->string = g_string_new(initial);
    string_channel->offset = 0;

    return channel;
}

GString *
gcut_string_io_channel_get_string (GIOChannel *channel)
{
    GCutIOChannelString *string_channel = (GCutIOChannelString *)channel;

    return string_channel->string;
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
