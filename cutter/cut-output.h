/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef __CUT_OUTPUT_H__
#define __CUT_OUTPUT_H__

#include <glib-object.h>
#include "cutter.h"
#include "cut-test.h"

G_BEGIN_DECLS

#define CUT_TYPE_OUTPUT            (cut_output_get_type ())
#define CUT_OUTPUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_OUTPUT, CutOutput))
#define CUT_OUTPUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_OUTPUT, CutOutputClass))
#define CUT_IS_OUTPUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_OUTPUT))
#define CUT_IS_OUTPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_OUTPUT))
#define CUT_OUTPUT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_OUTPUT, CutOutputClass))

typedef struct _CutOutput         CutOutput;
typedef struct _CutOutputClass    CutOutputClass;

struct _CutOutput
{
    GObject object;
};

struct _CutOutputClass
{
    GObjectClass parent_class;
};

typedef enum {
    CUT_VERBOSE_LEVEL_SILENT,
    CUT_VERBOSE_LEVEL_NORMAL,
    CUT_VERBOSE_LEVEL_VERBOSE
} CutVerboseLevel;

GType        cut_output_get_type  (void) G_GNUC_CONST;

CutOutput   *cut_output_new (void);

void  cut_output_set_verbose_level         (CutOutput *output,
                                            CutVerboseLevel level);
void  cut_output_set_verbose_level_by_name (CutOutput *output,
                                            const gchar *name);
void  cut_output_set_source_directory      (CutOutput *output,
                                            const gchar *directory);
void  cut_output_set_use_color             (CutOutput *output,
                                            gboolean   use_color);

void  cut_output_on_start_test             (CutOutput *output, CutTest *test);
void  cut_output_on_complete_test          (CutOutput *output, CutTest *test);
void  cut_output_on_success                (CutOutput *output, CutTest *test);
void  cut_output_on_failure                (CutOutput *output, CutTest *test);
void  cut_output_on_error                  (CutOutput *output, CutTest *test);
void  cut_output_on_pending                (CutOutput *output, CutTest *test);

G_END_DECLS

#endif /* __CUT_OUTPUT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
