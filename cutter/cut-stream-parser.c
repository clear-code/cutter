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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-stream-parser.h"

#define CUT_STREAM_PARSER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_STREAM_PARSER, CutStreamParserPrivate))

typedef struct _CutStreamParserPrivate	CutStreamParserPrivate;
struct _CutStreamParserPrivate
{
    GMarkupParseContext *context;
    CutRunContext *run_context;
    CutTestResult *result;
    gchar *option_name;
};

enum
{
    PROP_0,
    PROP_RUN_CONTEXT,
};

enum
{
    RESULT,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE(CutStreamParser, cut_stream_parser, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void start_element_handler (GMarkupParseContext *context,
                                   const gchar         *element_name,
                                   const gchar        **attribute_names,
                                   const gchar        **attribute_values,
                                   gpointer             user_data,
                                   GError             **error);
static void end_element_handler   (GMarkupParseContext *context,
                                   const gchar         *element_name,
                                   gpointer             user_data,
                                   GError             **error);
static void text_handler          (GMarkupParseContext *context,
                                   const gchar         *text,
                                   gsize                text_len,
                                   gpointer             user_data,
                                   GError             **error);
static void error_handler         (GMarkupParseContext *context,
                                   GError              *error,
                                   gpointer             user_data);

static GMarkupParser markup_parser = {
    start_element_handler,
    end_element_handler,
    text_handler,
    NULL,
    error_handler,
};

static void
cut_stream_parser_class_init (CutStreamParserClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("run-context",
                               "run context",
                               "The run context of the stream parser",
                               CUT_TYPE_RUN_CONTEXT,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_RUN_CONTEXT, spec);

    signals[RESULT]
        = g_signal_new("result",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutStreamParserClass, result),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__OBJECT,
                       G_TYPE_NONE, 1, CUT_TYPE_TEST_RESULT);

    g_type_class_add_private(gobject_class, sizeof(CutStreamParserPrivate));
}

static void
cut_stream_parser_init (CutStreamParser *stream_parser)
{
    CutStreamParserPrivate *priv = CUT_STREAM_PARSER_GET_PRIVATE(stream_parser);

    priv->context = g_markup_parse_context_new(&markup_parser,
                                               G_MARKUP_TREAT_CDATA_AS_TEXT,
                                               stream_parser, NULL);
    priv->run_context = NULL;
    priv->result = NULL;
    priv->option_name = NULL;
}

static void
dispose (GObject *object)
{
    CutStreamParserPrivate *priv = CUT_STREAM_PARSER_GET_PRIVATE(object);

    if (priv->context) {
        g_markup_parse_context_free(priv->context);
        priv->context = NULL;
    }

    if (priv->run_context) {
        g_object_unref(priv->run_context);
        priv->run_context = NULL;
    }

    if (priv->result) {
        g_object_unref(priv->result);
        priv->result = NULL;
    }

    if (priv->option_name) {
        g_free(priv->option_name);
        priv->option_name = NULL;
    }

    G_OBJECT_CLASS(cut_stream_parser_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutStreamParserPrivate *priv = CUT_STREAM_PARSER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        if (priv->run_context) {
            g_object_unref(priv->run_context);
            priv->run_context = NULL;
        }
        priv->run_context = g_value_get_object(value);
        if (priv->run_context)
            g_object_ref(priv->run_context);
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
    CutStreamParserPrivate *priv = CUT_STREAM_PARSER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        g_value_set_object(value, priv->run_context);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutStreamParser *
cut_stream_parser_new (CutRunContext *run_context)
{
    return g_object_new(CUT_TYPE_STREAM_PARSER,
                        "run-context", run_context,
                        NULL);
}

gboolean
cut_stream_parser_parse (CutStreamParser *stream_parser,
                         const gchar *text, gsize text_len, GError **error)
{
    CutStreamParserPrivate *priv = CUT_STREAM_PARSER_GET_PRIVATE(stream_parser);

    if (text_len < 0)
        text_len = strlen(text);
    return g_markup_parse_context_parse(priv->context, text, text_len, error);
}

static void
set_parse_error (GMarkupParseContext *context,
                 GError             **error,
                 const gchar         *format, ...)
{
    gint line = 0, chr = 0;
    gchar *message, *user_message;
    va_list var_args;

    va_start(var_args, format);
    user_message = g_strdup_vprintf(format, var_args);
    va_end(var_args);

    g_markup_parse_context_get_position(context, &line, &chr);

    message = g_strdup_printf("%s\n at line %d char %d.",
                              user_message, line, chr);

    *error = g_error_new(G_MARKUP_ERROR,
                         G_MARKUP_ERROR_PARSE,
                         message);
    g_free(message);
    g_free(user_message);
}

static void
start_element_handler (GMarkupParseContext *context,
                       const gchar         *element_name,
                       const gchar        **attr_names,
                       const gchar        **attr_values,
                       gpointer             user_data,
                       GError             **error)
{
    CutStreamParser *parser = user_data;
    CutStreamParserPrivate *priv;

    priv = CUT_STREAM_PARSER_GET_PRIVATE(parser);

    if (g_ascii_strcasecmp("stream", element_name) == 0) {
        if (priv->run_context) {
            g_signal_emit_by_name(priv->run_context, "start-run");
        }
    } else if (g_ascii_strcasecmp("result", element_name) == 0) {
        priv->result = g_object_new(CUT_TYPE_TEST_RESULT, NULL);
    } else if (g_ascii_strcasecmp("test-case", element_name) == 0) {
        CutTestCase *test_case;
        test_case = cut_test_case_new(NULL,
                                      NULL, NULL,
                                      NULL, NULL,
                                      NULL, NULL);
        cut_test_result_set_test_case(priv->result, test_case);
        g_object_unref(test_case);
    } else if (g_ascii_strcasecmp("test", element_name) == 0) {
        CutTest *test;
        test = cut_test_new(NULL, NULL);
        cut_test_result_set_test(priv->result, test);
        g_object_unref(test);
    }
}

static void
end_element_handler (GMarkupParseContext *context,
                     const gchar         *element_name,
                     gpointer             user_data,
                     GError             **error)
{
    CutStreamParser *parser = user_data;
    CutStreamParserPrivate *priv;

    priv = CUT_STREAM_PARSER_GET_PRIVATE(parser);

    if (g_ascii_strcasecmp("result", element_name) == 0) {
        if (priv->result) {
            g_signal_emit_by_name(parser, "result", priv->result);
            g_object_unref(priv->result);
            priv->result = NULL;
        }
    }
}

static CutTestResultStatus
result_name_to_status (const gchar *name)
{
    if (g_ascii_strcasecmp(name, "success")  == 0)
        return CUT_TEST_RESULT_SUCCESS;
    else if (g_ascii_strcasecmp(name, "failure") == 0)
        return CUT_TEST_RESULT_FAILURE;
    else if (g_ascii_strcasecmp(name, "error") == 0)
         return CUT_TEST_RESULT_ERROR;
    else if (g_ascii_strcasecmp(name, "pending") == 0)
         return CUT_TEST_RESULT_PENDING;
    else if (g_ascii_strcasecmp(name, "notification") == 0)
        return CUT_TEST_RESULT_NOTIFICATION;

    return CUT_TEST_RESULT_INVALID;
}

static const gchar *
get_parent_element (GMarkupParseContext *context)
{
    const GSList *elements, *node;

    elements = g_markup_parse_context_get_element_stack(context);

    node = g_slist_next(elements);

    return node ? node->data : NULL;
}

static void
set_option_name (CutStreamParserPrivate *priv, const gchar *option_name)
{
    if (priv->option_name) {
        g_free(priv->option_name);
        priv->option_name = NULL;
    }

    if (option_name)
        priv->option_name = g_strdup(option_name);
}

static void
set_various_name (GMarkupParseContext *context,
                  CutStreamParserPrivate *priv,
                  const gchar *value,
                  GError **error)
{
    const gchar *parent;

    parent = get_parent_element(context);

    if (g_ascii_strcasecmp("test-case", parent) == 0) {
        cut_test_set_name(CUT_TEST(cut_test_result_get_test_case(priv->result)),
                          value);
    } else if (g_ascii_strcasecmp("test", parent) == 0) {
        cut_test_set_name(cut_test_result_get_test(priv->result), value);
    } else if (g_ascii_strcasecmp("option", parent) == 0) {
        set_option_name(priv, value);
    } else {
        set_parse_error(context, error, "Whose name is %s?", value);
    }
}

static void
set_option_value (GMarkupParseContext *context,
                  CutStreamParserPrivate *priv,
                  const gchar *value,
                  GError **error)
{
    const gchar *parent;

    parent = get_parent_element(context);

    if (g_ascii_strcasecmp("option", parent) == 0) {
        if (priv->option_name) {
            CutTest *test;
            test = cut_test_result_get_test(priv->result);
            cut_test_set_attribute(test, priv->option_name, value);
        } else {
            set_parse_error(context, error, "option name is not set.");
        }
    } else {
        set_parse_error(context, error,
                        "<value> element should be in <option>.");
    }
}

static gboolean
is_integer (const gchar *str)
{
    gint i = 0;

    if (!str)
        return FALSE;

    while (str[i]) {
        if (!g_ascii_isdigit(str[i]))
            return FALSE;
        i++;
    }
    return TRUE;
}

static void
set_line (GMarkupParseContext *context,
          CutStreamParserPrivate *priv,
          const gchar *text,
          GError **error)
{
    if (is_integer(text)) {
        cut_test_result_set_line(priv->result, atoi(text));
    } else {
        set_parse_error(context, error,
                        "%s does not seem be line number",
                        text);
    }
}

static void
set_function_name (GMarkupParseContext *context,
                   CutStreamParserPrivate *priv,
                   const gchar *name,
                   GError **error)
{
    if (g_str_has_suffix(name, "()")) {
        gchar *real_name;
        real_name = g_strndup(name, strlen(name) - 2);
        cut_test_result_set_function_name(priv->result, real_name);
        g_free(real_name);
    } else {
        set_parse_error(context, error,
                        "%s does not seem be function name", name);
    }
}

static void
text_handler (GMarkupParseContext *context,
              const gchar         *text,
              gsize                text_len,
              gpointer             user_data,
              GError             **error)
{
    CutStreamParser *parser = user_data;
    CutStreamParserPrivate *priv;
    const gchar *element;

    priv = CUT_STREAM_PARSER_GET_PRIVATE(parser);

    if (!priv->result)
        return;

    element = g_markup_parse_context_get_element(context);

    if (g_ascii_strcasecmp("name", element) == 0) {
        set_various_name(context, priv, text, error);
    } else if (g_ascii_strcasecmp("description", element) == 0) {
        cut_test_set_attribute(cut_test_result_get_test(priv->result),
                               "description", text);
    } else if (g_ascii_strcasecmp("value", element) == 0) {
        set_option_value(context, priv, text, error);
    } else if (g_ascii_strcasecmp("detail", element) == 0) {
        cut_test_result_set_message(priv->result, text);
    } else if (g_ascii_strcasecmp("file", element) == 0) {
        cut_test_result_set_filename(priv->result, text);
    } else if (g_ascii_strcasecmp("line", element) == 0) {
        set_line(context, priv, text, error);
    } else if (g_ascii_strcasecmp("info", element) == 0) {
        set_function_name(context, priv, text, error);
    } else if (g_ascii_strcasecmp("status", element) == 0) {
        CutTestResultStatus status;
        status = result_name_to_status(text);
        if (status != CUT_TEST_RESULT_INVALID)
            cut_test_result_set_status(priv->result, status);
    } else if (g_ascii_strcasecmp("elapsed", element) == 0) {
        cut_test_result_set_elapsed(priv->result, g_ascii_strtod(text, NULL));
    }
}

static void
error_handler (GMarkupParseContext *context,
               GError              *error,
               gpointer             user_data)
{
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
