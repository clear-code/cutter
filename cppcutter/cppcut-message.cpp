/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2016  Kouhei Sutou <kou@clear-code.com>
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

#include <glib.h>

#include <cutter.h>
#include "cppcut-message.h"

cut::Message::Message() : buffer_()
{
}

void
cut::Message::printf(const char *format, ...)
{
    gchar *formatted_message;
    va_list args;

    if (!format)
        return;

    va_start(args, format);
    formatted_message = g_strdup_vprintf(format, args);
    va_end(args);
    buffer_ << formatted_message;
    g_free(formatted_message);
}

cut::Message::~Message()
{
    cut_message("%s", buffer_.str().c_str());
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
