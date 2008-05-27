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

#ifndef __GST_CUTTER_CONSOLE_OUTPUT_H__
#define __GST_CUTTER_CONSOLE_OUTPUT_H__

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS

#define GST_TYPE_CUTTER_CONSOLE_OUTPUT            (gst_cutter_console_output_get_type())
#define GST_CUTTER_CONSOLE_OUTPUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_CUTTER_CONSOLE_OUTPUT, GstCutterConsoleOutput))
#define GST_CUTTER_CONSOLE_OUTPUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_CUTTER_CONSOLE_OUTPUT, GstCutterConsoleOutputClass))
#define GST_CUTTER_CONSOLE_OUTPUT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_CUTTER_CONSOLE_OUTPUT, GstCutterConsoleOutputClass))
#define GST_IS_CUTTER_CONSOLE_OUTPUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_CUTTER_CONSOLE_OUTPUT))
#define GST_IS_CUTTER_CONSOLE_OUTPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_CUTTER_CONSOLE_OUTPUT))

typedef struct _GstCutterConsoleOutput      GstCutterConsoleOutput;
typedef struct _GstCutterConsoleOutputClass GstCutterConsoleOutputClass;

struct _GstCutterConsoleOutput {
    GstBaseSink parent;
};

struct _GstCutterConsoleOutputClass {
  GstBaseSinkClass parent_class;
};

GType gst_cutter_console_output_get_type (void);

G_END_DECLS

#endif /* __GST_CUTTER_CONSOLE_OUTPUT_H */
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
