/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
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

#include "gst-cutter-test-runner.h"
#include "gst-cutter-console-output.h"
#include "gst-cutter-server.h"

GST_DEBUG_CATEGORY(cutter_debug);

static gboolean
plugin_init (GstPlugin * plugin)
{
    gst_element_register(plugin, "cutter-test-runner", GST_RANK_NONE,
                         GST_TYPE_CUTTER_TEST_RUNNER);
    gst_element_register(plugin, "cutter-console-output", GST_RANK_NONE,
                         GST_TYPE_CUTTER_CONSOLE_OUTPUT);
    gst_element_register(plugin, "cutter-server", GST_RANK_NONE,
                         GST_TYPE_CUTTER_SERVER);
    GST_DEBUG_CATEGORY_INIT(cutter_debug, "cutter-test", 0, "Cutter elements");

    return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR, 
                   "cutter-test", "Cutter element",
                   plugin_init, VERSION, "LGPL",
                   "GstCutterTest", "http://cutter.sf.net");

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
