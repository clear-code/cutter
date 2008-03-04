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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include "cut-listener.h"

G_DEFINE_ABSTRACT_TYPE (CutListener, cut_listener, G_TYPE_OBJECT)

static void
cut_listener_class_init (CutListenerClass *klass)
{
    klass->attach_to_runner   = NULL;
    klass->detach_from_runner = NULL;
}

static void
cut_listener_init (CutListener *listener)
{
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
