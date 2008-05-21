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

#include "gst-cutter-src.h"

#include <gst/video/video.h>

static const GstElementDetails cutter_src_details =
    GST_ELEMENT_DETAILS ("Cutter src",
                         "Source element of Cutter",
                         "Source element of Cutter",
                         "g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>");
static GstStaticPadTemplate cutter_src_src_template_factory =
    GST_STATIC_PAD_TEMPLATE ("src",
                             GST_PAD_SRC,
                             GST_PAD_ALWAYS,
                             GST_STATIC_CAPS (GST_VIDEO_CAPS_YUV ("I420")));

GST_BOILERPLATE (GstCutterSrc, gst_cutter_src, GstElement, GST_TYPE_ELEMENT);

static void
gst_cutter_src_base_init (gpointer klass)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

    gst_element_class_add_pad_template(element_class,
        gst_static_pad_template_get(&cutter_src_src_template_factory));

    gst_element_class_set_details(element_class, &cutter_src_details);
}

static void
gst_cutter_src_init (GstCutterSrc *overlay, GstCutterSrcClass * klass)
{
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
