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

typedef enum {
    IN_TOP_LEVEL,

    IN_STREAM,

    IN_TEST_SUITE,
    IN_TEST_CASE,
    IN_TEST,

    IN_TEST_NAME,
    IN_TEST_DESCRIPTION,
    IN_TEST_OPTION,

    IN_TEST_OPTION_NAME,
    IN_TEST_OPTION_VALUE,

    IN_READY_TEST_SUITE,
    IN_READY_TEST_SUITE_N_TEST_CASES,
    IN_READY_TEST_SUITE_N_TESTS,

    IN_START_TEST_SUITE,

    IN_READY_TEST_CASE,
    IN_READY_TEST_CASE_N_TESTS,

    IN_RESULT,
    IN_RESULT_STATUS,
    IN_RESULT_DETAIL,
    IN_RESULT_BACKTRACE,
    IN_RESULT_BACKTRACE_FILE,
    IN_RESULT_BACKTRACE_LINE,
    IN_RESULT_BACKTRACE_INFO,
    IN_RESULT_ELAPSED,

    IN_SUCCESS
} ParseState;

#define CUT_STREAM_PARSER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_STREAM_PARSER, CutStreamParserPrivate))

typedef struct _ReadyTestSuite ReadyTestSuite;
struct _ReadyTestSuite
{
    CutTestSuite *test_suite;
    gint n_test_cases;
    gint n_tests;
};

typedef struct _ReadyTestCase ReadyTestCase;
struct _ReadyTestCase
{
    CutTestCase *test_case;
    gint n_tests;
};

typedef struct _CutStreamParserPrivate	CutStreamParserPrivate;
struct _CutStreamParserPrivate
{
    GMarkupParseContext *context;
    CutRunContext *run_context;
    GQueue *states;
    CutTestResult *result;
    gchar *option_name;
    gboolean success;
    ReadyTestSuite *ready_test_suite;
    ReadyTestCase *ready_test_case;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTest *test;
};

#define PUSH_STATE(priv, state)                                 \
    (g_queue_push_head((priv)->states, GINT_TO_POINTER(state)))
#define POP_STATE(priv)                                 \
    (GPOINTER_TO_INT(g_queue_pop_head((priv)->states)))
#define PEEK_STATE(priv)                                        \
    (GPOINTER_TO_INT(g_queue_peek_head((priv)->states)))
#define PEEK_NTH_STATE(priv, n)                                 \
    (GPOINTER_TO_INT(g_queue_peek_nth((priv)->states, n)))

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
    priv->states = g_queue_new();
    PUSH_STATE(priv, IN_TOP_LEVEL);
    priv->run_context = NULL;
    priv->result = NULL;
    priv->option_name = NULL;
    priv->success = TRUE;
    priv->ready_test_suite = NULL;
    priv->ready_test_case = NULL;
    priv->test_suite = NULL;
    priv->test_case = NULL;
    priv->test = NULL;
}

static ReadyTestSuite *
ready_test_suite_new (void)
{
    return g_slice_new0(ReadyTestSuite);
}

static void
ready_test_suite_free (ReadyTestSuite *ready_test_suite)
{
    if (ready_test_suite->test_suite)
        g_object_unref(ready_test_suite->test_suite);
    g_slice_free(ReadyTestSuite, ready_test_suite);
}

static ReadyTestCase *
ready_test_case_new (void)
{
    return g_slice_new0(ReadyTestCase);
}

static void
ready_test_case_free (ReadyTestCase *ready_test_case)
{
    if (ready_test_case->test_case)
        g_object_unref(ready_test_case->test_case);
    g_slice_free(ReadyTestCase, ready_test_case);
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

    if (priv->states) {
        g_queue_free(priv->states);
        priv->states = NULL;
    }

    if (priv->result) {
        g_object_unref(priv->result);
        priv->result = NULL;
    }

    if (priv->option_name) {
        g_free(priv->option_name);
        priv->option_name = NULL;
    }

    if (priv->ready_test_suite) {
        ready_test_suite_free(priv->ready_test_suite);
        priv->ready_test_suite = NULL;
    }

    if (priv->ready_test_case) {
        ready_test_case_free(priv->ready_test_case);
        priv->ready_test_case = NULL;
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

    message = g_strdup_printf("%s at line %d char %d.",
                              user_message, line, chr);

    *error = g_error_new(G_MARKUP_ERROR,
                         G_MARKUP_ERROR_PARSE,
                         message);
    g_free(message);
    g_free(user_message);
}

#define invalid_element(context, error)                         \
    _invalid_element(context, error, __FILE__, __LINE__)

static void
_invalid_element (GMarkupParseContext *context, GError **error,
                  const gchar *file, gint line)
{
    GString *string;
    const GSList *node;

    string = g_string_new(NULL);
    for (node = g_markup_parse_context_get_element_stack(context);
         node;
         node = g_slist_next(node)) {
        g_string_prepend(string, node->data);
        g_string_prepend(string, "/");
    }

    set_parse_error(context, error,
                    "invalid element: %s: %s:%d:", string->str, file, line);
    g_string_free(string, TRUE);
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
start_element_handler (GMarkupParseContext *context,
                       const gchar         *element_name,
                       const gchar        **attr_names,
                       const gchar        **attr_values,
                       gpointer             user_data,
                       GError             **error)
{
    CutStreamParser *parser = user_data;
    CutStreamParserPrivate *priv;
    ParseState state;

    priv = CUT_STREAM_PARSER_GET_PRIVATE(parser);

    state = PEEK_STATE(priv);
    switch (state) {
      case IN_TOP_LEVEL:
        if (g_ascii_strcasecmp("stream", element_name) == 0) {
            PUSH_STATE(priv, IN_STREAM);
            if (priv->run_context) {
                g_signal_emit_by_name(priv->run_context, "start-run");
            }
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_STREAM:
        if (g_ascii_strcasecmp("result", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT);
            priv->result = g_object_new(CUT_TYPE_TEST_RESULT, NULL);
        } else if (g_ascii_strcasecmp("ready-test-suite", element_name) == 0) {
            PUSH_STATE(priv, IN_READY_TEST_SUITE);
            priv->ready_test_suite = ready_test_suite_new();
        } else if (g_ascii_strcasecmp("start-test-suite", element_name) == 0) {
            PUSH_STATE(priv, IN_START_TEST_SUITE);
            priv->test_suite = cut_test_suite_new();
        } else if (g_ascii_strcasecmp("ready-test-case", element_name) == 0) {
            PUSH_STATE(priv, IN_READY_TEST_CASE);
            priv->ready_test_case = ready_test_case_new();;
        } else if (g_ascii_strcasecmp("success", element_name) == 0) {
            PUSH_STATE(priv, IN_SUCCESS);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_READY_TEST_SUITE:
        if (g_ascii_strcasecmp("test-suite", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_SUITE);
            priv->ready_test_suite->test_suite = cut_test_suite_new();
            priv->test_suite = priv->ready_test_suite->test_suite;
        } else if (g_ascii_strcasecmp("n-test-cases", element_name) == 0) {
            PUSH_STATE(priv, IN_READY_TEST_SUITE_N_TEST_CASES);
        } else if (g_ascii_strcasecmp("n-tests", element_name) == 0) {
            PUSH_STATE(priv, IN_READY_TEST_SUITE_N_TESTS);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_START_TEST_SUITE:
        if (g_ascii_strcasecmp("test-suite", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_SUITE);
            priv->test_suite = cut_test_suite_new();
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_READY_TEST_CASE:
        if (g_ascii_strcasecmp("test-case", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_CASE);
            priv->ready_test_case->test_case = cut_test_case_new_empty();
            priv->test_case = priv->ready_test_case->test_case;
        } else if (g_ascii_strcasecmp("n-tests", element_name) == 0) {
            PUSH_STATE(priv, IN_READY_TEST_CASE_N_TESTS);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_RESULT:
        if (g_ascii_strcasecmp("test-case", element_name) == 0) {
            CutTestCase *test_case;

            PUSH_STATE(priv, IN_TEST_CASE);
            test_case = cut_test_case_new_empty();
            cut_test_result_set_test_case(priv->result, test_case);
            priv->test_case = test_case;
            g_object_unref(test_case);
        } else if (g_ascii_strcasecmp("test", element_name) == 0) {
            CutTest *test;

            PUSH_STATE(priv, IN_TEST);
            test = cut_test_new(NULL, NULL);
            cut_test_result_set_test(priv->result, test);
            priv->test = test;
            g_object_unref(test);
        } else if (g_ascii_strcasecmp("status", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT_STATUS);
        } else if (g_ascii_strcasecmp("detail", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT_DETAIL);
        } else if (g_ascii_strcasecmp("backtrace", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE);
        } else if (g_ascii_strcasecmp("elapsed", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT_ELAPSED);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TEST_SUITE:
      case IN_TEST_CASE:
      case IN_TEST:
        if (g_ascii_strcasecmp("name", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_NAME);
        } else if (g_ascii_strcasecmp("description", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_DESCRIPTION);
        } else if (g_ascii_strcasecmp("option", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_OPTION);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TEST_OPTION:
        if (g_ascii_strcasecmp("name", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_OPTION_NAME);
        } else if (g_ascii_strcasecmp("value", element_name) == 0) {
            PUSH_STATE(priv, IN_TEST_OPTION_VALUE);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_RESULT_BACKTRACE:
        if (g_ascii_strcasecmp("file", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE_FILE);
        } else if (g_ascii_strcasecmp("line", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE_LINE);
        } else if (g_ascii_strcasecmp("info", element_name) == 0) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE_INFO);
        } else {
            invalid_element(context, error);
        }
        break;
      default:
        g_print("%d\n", state);
        invalid_element(context, error);
        break;
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
    const gchar *parent_name;
    ParseState state;

    priv = CUT_STREAM_PARSER_GET_PRIVATE(parser);

    parent_name = get_parent_element(context);
    state = POP_STATE(priv);
    switch (state) {
      case IN_STREAM:
        if (priv->run_context)
            g_signal_emit_by_name(priv->run_context,
                                  "complete-run", priv->success);
        break;
      case IN_RESULT:
        if (priv->result) {
            g_signal_emit_by_name(parser, "result", priv->result);
            g_object_unref(priv->result);
            priv->result = NULL;
        }
        break;
      case IN_READY_TEST_SUITE:
        if (priv->ready_test_suite) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context, "ready-test-suite",
                                      priv->ready_test_suite->test_suite,
                                      priv->ready_test_suite->n_test_cases,
                                      priv->ready_test_suite->n_tests);
            ready_test_suite_free(priv->ready_test_suite);
            priv->ready_test_suite = NULL;
        }
        break;
      case IN_START_TEST_SUITE:
        if (priv->test_suite) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context, "start-test-suite",
                                      priv->test_suite);
            g_object_unref(priv->test_suite);
            priv->test_suite = NULL;
        }
        break;
      case IN_READY_TEST_CASE:
        if (priv->ready_test_case) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context, "ready-test-case",
                                      priv->ready_test_case->test_case,
                                      priv->ready_test_case->n_tests);
            ready_test_case_free(priv->ready_test_case);
            priv->ready_test_case = NULL;
        }
        break;
      default:
        break;
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

static CutTest *
target_test_object (CutStreamParserPrivate *priv, ParseState parent_state)
{
    CutTest *target = NULL;

    switch (parent_state) {
      case IN_TEST_SUITE:
        target = CUT_TEST(priv->test_suite);
        break;
      case IN_TEST_CASE:
        target = CUT_TEST(priv->test_case);
        break;
      case IN_TEST:
        target = priv->test;
        break;
      default:
        break;
    }

    return target;
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
    ParseState state, parent_state;

    priv = CUT_STREAM_PARSER_GET_PRIVATE(parser);
    element = g_markup_parse_context_get_element(context);

    state = PEEK_STATE(priv);
    parent_state = PEEK_NTH_STATE(priv, 1);
    switch (state) {
      case IN_TEST_NAME:
        {
            CutTest *target;

            target = target_test_object(priv, parent_state);
            if (target) {
                cut_test_set_name(target, text);
            } else {
                set_parse_error(context, error,
                                "unknown test object: %s", element);
            }
        }
        break;
      case IN_TEST_DESCRIPTION:
        {
            CutTest *target;

            target = target_test_object(priv, parent_state);
            if (target) {
                cut_test_set_attribute(target, "description", text);
            } else {
                set_parse_error(context, error,
                                "unknown test object: %s", element);
            }
        }
        break;
      case IN_TEST_OPTION_NAME:
        set_option_name(priv, text);
        break;
      case IN_TEST_OPTION_VALUE:
        set_option_value(context, priv, text, error);
        break;
      case IN_RESULT_STATUS:
        {
            CutTestResultStatus result_status;
            result_status = result_name_to_status(text);
            if (result_status != CUT_TEST_RESULT_INVALID) {
                cut_test_result_set_status(priv->result, result_status);
            }
        }
        break;
      case IN_RESULT_DETAIL:
        cut_test_result_set_message(priv->result, text);
        break;
      case IN_RESULT_BACKTRACE_FILE:
        cut_test_result_set_filename(priv->result, text);
        break;
      case IN_RESULT_BACKTRACE_LINE:
        set_line(context, priv, text, error);
        break;
      case IN_RESULT_BACKTRACE_INFO:
        set_function_name(context, priv, text, error);
        break;
      case IN_RESULT_ELAPSED:
        cut_test_result_set_elapsed(priv->result,
                                    g_ascii_strtod(text, NULL));
        break;
      case IN_READY_TEST_SUITE_N_TEST_CASES:
        priv->ready_test_suite->n_test_cases = atoi(text);
        break;
      case IN_READY_TEST_SUITE_N_TESTS:
        priv->ready_test_suite->n_tests = atoi(text);
        break;
      case IN_READY_TEST_CASE_N_TESTS:
        priv->ready_test_case->n_tests = atoi(text);
        break;
      case IN_SUCCESS:
        priv->success = g_ascii_strcasecmp("TRUE", text) == 0;
      default:
        break;
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
