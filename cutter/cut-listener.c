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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include "cut-listener.h"

GType
cut_listener_get_type (void)
{
    static GType listener_type = 0;

    if (!listener_type) {
        const GTypeInfo listener_info = {
            sizeof(CutListenerClass), /* class_size */
            NULL,                     /* base_init */
            NULL,                     /* base_finalize */
        };

        listener_type = g_type_register_static(G_TYPE_INTERFACE, "CutListener",
                                               &listener_info, 0);
    }

    return listener_type;
}

void
cut_listener_attach_to_runner (CutListener *listener,
                               CutRunner   *runner)
{
    if (CUT_LISTENER_GET_CLASS(listener)->attach_to_runner)
        CUT_LISTENER_GET_CLASS(listener)->attach_to_runner(listener, runner);
}

void
cut_listener_detach_from_runner (CutListener *listener,
                                 CutRunner   *runner)
{
    if (CUT_LISTENER_GET_CLASS(listener)->detach_from_runner)
        CUT_LISTENER_GET_CLASS(listener)->detach_from_runner(listener, runner);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
