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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-test-result.h"
#include "cut-enum-types.h"

#define CUT_TEST_RESULT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_RESULT, CutTestResultPrivate))

typedef struct _CutTestResultPrivate	CutTestResultPrivate;
struct _CutTestResultPrivate
{
    CutTestResultStatus status;
    gchar *message;
    gchar *function_name;
    gchar *filename;
    guint line;
};

enum
{
    PROP_0,
    PROP_STATUS,
    PROP_MESSAGE,
    PROP_FUNCTION_NAME,
    PROP_FILENAME,
    PROP_LINE,
};


G_DEFINE_TYPE (CutTestResult, cut_test_result, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
cut_test_result_class_init (CutTestResultClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_enum("status",
                             "Status",
                             "The status of the result",
                             CUT_TYPE_TEST_RESULT_STATUS,
                             CUT_TEST_RESULT_SUCCESS,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_STATUS, spec);

    spec = g_param_spec_string("message",
                               "Message",
                               "The message of the result",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_MESSAGE, spec);

    spec = g_param_spec_string("function-name",
                               "Function name",
                               "The function name of the result",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_FUNCTION_NAME, spec);

    spec = g_param_spec_string("filename",
                               "Filename",
                               "The filename of the result",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_FILENAME, spec);

    spec = g_param_spec_uint("line",
                             "Line number",
                             "The line number of the result",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LINE, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestResultPrivate));
}

static void
cut_test_result_init (CutTestResult *result)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(result);

    priv->status = CUT_TEST_RESULT_SUCCESS;
    priv->message = NULL;
    priv->function_name = NULL;
    priv->filename = NULL;
    priv->line = 0;
}

static void
dispose (GObject *object)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(object);

    if (priv->message) {
        g_free(priv->message);
        priv->message = NULL;
    }

    if (priv->function_name) {
        g_free(priv->function_name);
        priv->function_name = NULL;
    }

    if (priv->filename) {
        g_free(priv->filename);
        priv->filename = NULL;
    }

    G_OBJECT_CLASS(cut_test_result_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_STATUS:
        priv->status = g_value_get_enum(value);
        break;
      case PROP_MESSAGE:
        if (priv->message)
            g_free(priv->message);
        priv->message = g_value_dup_string(value);
      case PROP_FUNCTION_NAME:
        if (priv->function_name)
            g_free(priv->function_name);
        priv->function_name = g_value_dup_string(value);
      case PROP_FILENAME:
        if (priv->filename)
            g_free(priv->filename);
        priv->filename = g_value_dup_string(value);
        break;
      case PROP_LINE:
        priv->line = g_value_get_uint(value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    CutTestResultPrivate *priv = CUT_TEST_RESULT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_STATUS:
        g_value_set_enum(value, priv->status);
        break;
      case PROP_MESSAGE:
        g_value_set_string(value, priv->message);
        break;
      case PROP_FUNCTION_NAME:
        g_value_set_string(value, priv->function_name);
        break;
      case PROP_FILENAME:
        g_value_set_string(value, priv->filename);
        break;
      case PROP_LINE:
        g_value_set_uint(value, priv->line);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestResult *
cut_test_result_new (CutTestResultStatus status,
                     const gchar *result_message,
                     const gchar *function_name,
                     const gchar *filename,
                     guint line)
{
    return g_object_new(CUT_TYPE_TEST_RESULT,
                        "status", status,
                        "message", result_message,
                        "function-name", function_name,
                        "filename", filename,
                        "line", line,
                        NULL);
}

CutTestResultStatus
cut_test_result_get_status (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->status;
}

const gchar *
cut_test_result_get_message (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->message;
}

const gchar *
cut_test_result_get_function_name (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->function_name;
}

const gchar *
cut_test_result_get_filename (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->filename;
}

guint
cut_test_result_get_line (CutTestResult *result)
{
    return CUT_TEST_RESULT_GET_PRIVATE(result)->line;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
