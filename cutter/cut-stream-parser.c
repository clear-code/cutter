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
    IN_TOP_LEVEL_RESULT,

    IN_STREAM,
    IN_STREAM_SUCCESS,

    IN_TEST_SUITE,
    IN_TEST_CASE,
    IN_TEST,

    IN_TEST_NAME,
    IN_TEST_DESCRIPTION,
    IN_TEST_OPTION,

    IN_TEST_OPTION_NAME,
    IN_TEST_OPTION_VALUE,

    IN_TEST_CONTEXT,
    IN_TEST_CONTEXT_FAILED,

    IN_READY_TEST_SUITE,
    IN_READY_TEST_SUITE_N_TEST_CASES,
    IN_READY_TEST_SUITE_N_TESTS,

    IN_START_TEST_SUITE,

    IN_READY_TEST_CASE,
    IN_READY_TEST_CASE_N_TESTS,

    IN_START_TEST_CASE,

    IN_START_TEST,

    IN_TEST_RESULT,

    IN_RESULT,
    IN_RESULT_STATUS,
    IN_RESULT_DETAIL,
    IN_RESULT_BACKTRACE,
    IN_RESULT_BACKTRACE_ENTRY,
    IN_RESULT_BACKTRACE_ENTRY_FILE,
    IN_RESULT_BACKTRACE_ENTRY_LINE,
    IN_RESULT_BACKTRACE_ENTRY_INFO,
    IN_RESULT_ELAPSED,

    IN_COMPLETE_TEST,

    IN_TEST_CASE_RESULT,

    IN_COMPLETE_TEST_CASE,

    IN_COMPLETE_TEST_SUITE,

    IN_CRASHED,
    IN_CRASHED_BACKTRACE
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

typedef struct _StartTest StartTest;
struct _StartTest
{
    CutTest *test;
    CutTestContext *test_context;
};

typedef struct _TestResult TestResult;
struct _TestResult
{
    CutTest *test;
    CutTestContext *test_context;
    CutTestResult *result;
};

typedef struct _CompleteTest CompleteTest;
struct _CompleteTest
{
    CutTest *test;
    CutTestContext *test_context;
};

typedef struct _TestCaseResult TestCaseResult;
struct _TestCaseResult
{
    CutTestCase *test_case;
    CutTestResult *result;
};

typedef struct _CutStreamParserPrivate	CutStreamParserPrivate;
struct _CutStreamParserPrivate
{
    GMarkupParseContext *context;
    CutRunContext *run_context;

    GQueue *states;
    GQueue *tests;
    GQueue *test_cases;
    GQueue *test_suites;
    GQueue *test_contexts;

    ReadyTestSuite *ready_test_suite;
    ReadyTestCase *ready_test_case;
    StartTest *start_test;
    TestResult *test_result;
    CompleteTest *complete_test;
    TestCaseResult *test_case_result;

    CutTestResult *result;
    gchar *option_name;
    gchar *option_value;
    gboolean success;
    gchar *crashed_backtrace;
};

#define PUSH_STATE(priv, state)                                 \
    (g_queue_push_head((priv)->states, GINT_TO_POINTER(state)))
#define POP_STATE(priv)                                 \
    (GPOINTER_TO_INT(g_queue_pop_head((priv)->states)))
#define DROP_STATE(priv)                        \
    (g_queue_pop_head((priv)->states))
#define PEEK_STATE(priv)                                        \
    (GPOINTER_TO_INT(g_queue_peek_head((priv)->states)))
#define PEEK_NTH_STATE(priv, n)                                 \
    (GPOINTER_TO_INT(g_queue_peek_nth((priv)->states, n)))

#define PUSH_TEST(priv, test)                                   \
    (g_queue_push_head((priv)->tests, g_object_ref(test)))
#define POP_TEST(priv)                          \
    (g_queue_pop_head((priv)->tests))
#define DROP_TEST(priv)                         \
    (g_object_unref(POP_TEST(priv)))
#define PEEK_TEST(priv)                         \
    (g_queue_peek_head((priv)->tests))

#define PUSH_TEST_CASE(priv, test_case)                                 \
    (g_queue_push_head((priv)->test_cases, g_object_ref(test_case)))
#define POP_TEST_CASE(priv)                     \
    (g_queue_pop_head((priv)->test_cases))
#define DROP_TEST_CASE(priv)                    \
    (g_object_unref(POP_TEST_CASE(priv)))
#define PEEK_TEST_CASE(priv)                    \
    (g_queue_peek_head((priv)->test_cases))

#define PUSH_TEST_SUITE(priv, test_suite)                               \
    (g_queue_push_head((priv)->test_suites, g_object_ref(test_suite)))
#define POP_TEST_SUITE(priv)                    \
    (g_queue_pop_head((priv)->test_suites))
#define DROP_TEST_SUITE(priv)                   \
    (g_object_unref(POP_TEST_SUITE(priv)))
#define PEEK_TEST_SUITE(priv)                   \
    (g_queue_peek_head((priv)->test_suites))

#define PUSH_TEST_CONTEXT(priv, test_context)                           \
    (g_queue_push_head((priv)->test_contexts, g_object_ref(test_context)))
#define POP_TEST_CONTEXT(priv)                  \
    (g_queue_pop_head((priv)->test_contexts))
#define DROP_TEST_CONTEXT(priv)                 \
    (g_object_unref(POP_TEST_CONTEXT(priv)))
#define PEEK_TEST_CONTEXT(priv)                 \
    (g_queue_peek_head((priv)->test_contexts))

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

    priv->states = g_queue_new();
    PUSH_STATE(priv, IN_TOP_LEVEL);
    priv->tests = g_queue_new();
    priv->test_cases = g_queue_new();
    priv->test_suites = g_queue_new();
    priv->test_contexts = g_queue_new();

    priv->ready_test_suite = NULL;
    priv->ready_test_case = NULL;
    priv->start_test = NULL;
    priv->complete_test = NULL;
    priv->test_result = NULL;
    priv->test_case_result = NULL;
    priv->crashed_backtrace = NULL;

    priv->result = NULL;
    priv->option_name = NULL;
    priv->option_value = NULL;
    priv->success = TRUE;
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

static StartTest *
start_test_new (void)
{
    return g_slice_new0(StartTest);
}

static void
start_test_free (StartTest *start_test)
{
    if (start_test->test)
        g_object_unref(start_test->test);
    if (start_test->test_context)
        g_object_unref(start_test->test_context);
    g_slice_free(StartTest, start_test);
}

static TestResult *
test_result_new (void)
{
    return g_slice_new0(TestResult);
}

static void
test_result_free (TestResult *test_result)
{
    if (test_result->test)
        g_object_unref(test_result->test);
    if (test_result->test_context)
        g_object_unref(test_result->test_context);
    if (test_result->result)
        g_object_unref(test_result->result);
    g_slice_free(TestResult, test_result);
}

static CompleteTest *
complete_test_new (void)
{
    return g_slice_new0(CompleteTest);
}

static void
complete_test_free (CompleteTest *complete_test)
{
    if (complete_test->test)
        g_object_unref(complete_test->test);
    if (complete_test->test_context)
        g_object_unref(complete_test->test_context);
    g_slice_free(CompleteTest, complete_test);
}

static TestCaseResult *
test_case_result_new (void)
{
    return g_slice_new0(TestCaseResult);
}

static void
test_case_result_free (TestCaseResult *test_case_result)
{
    if (test_case_result->test_case)
        g_object_unref(test_case_result->test_case);
    if (test_case_result->result)
        g_object_unref(test_case_result->result);
    g_slice_free(TestCaseResult, test_case_result);
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
        CutRunContext *run_context;

        run_context = priv->run_context;
        priv->run_context = NULL;
        g_object_unref(run_context);
    }

    if (priv->states) {
        g_queue_free(priv->states);
        priv->states = NULL;
    }

    if (priv->tests) {
        g_queue_foreach(priv->tests, (GFunc)g_object_unref, NULL);
        g_queue_free(priv->tests);
        priv->tests = NULL;
    }

    if (priv->test_cases) {
        g_queue_foreach(priv->test_cases, (GFunc)g_object_unref, NULL);
        g_queue_free(priv->test_cases);
        priv->test_cases = NULL;
    }

    if (priv->test_suites) {
        g_queue_foreach(priv->test_suites, (GFunc)g_object_unref, NULL);
        g_queue_free(priv->test_suites);
        priv->test_suites = NULL;
    }

    if (priv->test_contexts) {
        g_queue_foreach(priv->test_contexts, (GFunc)g_object_unref, NULL);
        g_queue_free(priv->test_contexts);
        priv->test_contexts = NULL;
    }

    if (priv->ready_test_suite) {
        ready_test_suite_free(priv->ready_test_suite);
        priv->ready_test_suite = NULL;
    }

    if (priv->ready_test_case) {
        ready_test_case_free(priv->ready_test_case);
        priv->ready_test_case = NULL;
    }

    if (priv->start_test) {
        start_test_free(priv->start_test);
        priv->start_test = NULL;
    }

    if (priv->test_result) {
        test_result_free(priv->test_result);
        priv->test_result = NULL;
    }

    if (priv->complete_test) {
        complete_test_free(priv->complete_test);
        priv->complete_test = NULL;
    }

    if (priv->test_case_result) {
        test_case_result_free(priv->test_case_result);
        priv->test_case_result = NULL;
    }

    if (priv->result) {
        g_object_unref(priv->result);
        priv->result = NULL;
    }

    if (priv->option_name) {
        g_free(priv->option_name);
        priv->option_name = NULL;
    }

    if (priv->option_value) {
        g_free(priv->option_value);
        priv->option_value = NULL;
    }

    if (priv->crashed_backtrace) {
        g_free(priv->crashed_backtrace);
        priv->crashed_backtrace = NULL;
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

CutStreamParser *
cut_test_result_parser_new (void)
{
    CutStreamParser *parser;
    CutStreamParserPrivate *priv;

    parser = cut_stream_parser_new(NULL);
    priv = CUT_STREAM_PARSER_GET_PRIVATE(parser);

    DROP_STATE(priv);
    PUSH_STATE(priv, IN_TOP_LEVEL_RESULT);

    return parser;
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

    message = g_strdup_printf("Error on line %d char %d: %s",
                              line, chr, user_message);

    *error = g_error_new(G_MARKUP_ERROR,
                         G_MARKUP_ERROR_PARSE,
                         message);
    g_free(message);
    g_free(user_message);
}

static gchar *
element_path (GMarkupParseContext *context)
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

    return g_string_free(string, FALSE);
}

static void
invalid_element (GMarkupParseContext *context, GError **error)
{
    gchar *path;

    path = element_path(context);
    set_parse_error(context, error, "invalid element: %s", path);
    g_free(path);
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
        if (g_str_equal("stream", element_name)) {
            PUSH_STATE(priv, IN_STREAM);
            if (priv->run_context) {
                g_signal_emit_by_name(priv->run_context, "start-run");
            }
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_STREAM:
        if (g_str_equal("test-result", element_name)) {
            PUSH_STATE(priv, IN_TEST_RESULT);
            priv->test_result = test_result_new();
        } else if (g_str_equal("ready-test-suite", element_name)) {
            PUSH_STATE(priv, IN_READY_TEST_SUITE);
            priv->ready_test_suite = ready_test_suite_new();
        } else if (g_str_equal("start-test-suite", element_name)) {
            PUSH_STATE(priv, IN_START_TEST_SUITE);
            PUSH_TEST_SUITE(priv, cut_test_suite_new());
        } else if (g_str_equal("ready-test-case", element_name)) {
            PUSH_STATE(priv, IN_READY_TEST_CASE);
            priv->ready_test_case = ready_test_case_new();;
        } else if (g_str_equal("start-test-case", element_name)) {
            PUSH_STATE(priv, IN_START_TEST_CASE);
            PUSH_TEST_CASE(priv, cut_test_case_new_empty());
        } else if (g_str_equal("start-test", element_name)) {
            PUSH_STATE(priv, IN_START_TEST);
            priv->start_test = start_test_new();
        } else if (g_str_equal("complete-test", element_name)) {
            PUSH_STATE(priv, IN_COMPLETE_TEST);
            priv->complete_test = complete_test_new();
        } else if (g_str_equal("test-case-result", element_name)) {
            PUSH_STATE(priv, IN_TEST_CASE_RESULT);
            priv->test_case_result = test_case_result_new();
        } else if (g_str_equal("complete-test-case", element_name)) {
            PUSH_STATE(priv, IN_COMPLETE_TEST_CASE);
        } else if (g_str_equal("complete-test-suite", element_name)) {
            PUSH_STATE(priv, IN_COMPLETE_TEST_SUITE);
        } else if (g_str_equal("success", element_name)) {
            PUSH_STATE(priv, IN_STREAM_SUCCESS);
        } else if (g_str_equal("crashed", element_name)) {
            PUSH_STATE(priv, IN_CRASHED);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_READY_TEST_SUITE:
        if (g_str_equal("test-suite", element_name)) {
            PUSH_STATE(priv, IN_TEST_SUITE);
            priv->ready_test_suite->test_suite = cut_test_suite_new();
            PUSH_TEST_SUITE(priv, priv->ready_test_suite->test_suite);
        } else if (g_str_equal("n-test-cases", element_name)) {
            PUSH_STATE(priv, IN_READY_TEST_SUITE_N_TEST_CASES);
        } else if (g_str_equal("n-tests", element_name)) {
            PUSH_STATE(priv, IN_READY_TEST_SUITE_N_TESTS);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_START_TEST_SUITE:
        if (g_str_equal("test-suite", element_name)) {
            PUSH_STATE(priv, IN_TEST_SUITE);
            PUSH_TEST_SUITE(priv, cut_test_suite_new());
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_READY_TEST_CASE:
        if (g_str_equal("test-case", element_name)) {
            PUSH_STATE(priv, IN_TEST_CASE);
            priv->ready_test_case->test_case = cut_test_case_new_empty();
            PUSH_TEST_CASE(priv, priv->ready_test_case->test_case);
        } else if (g_str_equal("n-tests", element_name)) {
            PUSH_STATE(priv, IN_READY_TEST_CASE_N_TESTS);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_START_TEST_CASE:
        if (g_str_equal("test-case", element_name)) {
            PUSH_STATE(priv, IN_TEST_CASE);
            PUSH_TEST_CASE(priv, cut_test_case_new_empty());
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_START_TEST:
        if (g_str_equal("test", element_name)) {
            PUSH_STATE(priv, IN_TEST);
            priv->start_test->test = cut_test_new_empty();
            PUSH_TEST(priv, priv->start_test->test);
        } else if (g_str_equal("test-context", element_name)) {
            PUSH_STATE(priv, IN_TEST_CONTEXT);
            priv->start_test->test_context = cut_test_context_new_empty();
            PUSH_TEST_CONTEXT(priv,  priv->start_test->test_context);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TEST_RESULT:
        if (g_str_equal("test", element_name)) {
            PUSH_STATE(priv, IN_TEST);
            priv->test_result->test = cut_test_new_empty();
            PUSH_TEST(priv, priv->test_result->test);
        } else if (g_str_equal("test-context", element_name)) {
            PUSH_STATE(priv, IN_TEST_CONTEXT);
            priv->test_result->test_context = cut_test_context_new_empty();
            PUSH_TEST_CONTEXT(priv, priv->test_result->test_context);
        } else if (g_str_equal("result", element_name)) {
            PUSH_STATE(priv, IN_RESULT);
            priv->test_result->result = cut_test_result_new_empty();
            priv->result = priv->test_result->result;
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TOP_LEVEL_RESULT:
        if (g_str_equal("result", element_name)) {
            PUSH_STATE(priv, IN_RESULT);
            priv->result = cut_test_result_new_empty();
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_RESULT:
        if (g_str_equal("test-case", element_name)) {
            CutTestCase *test_case;

            PUSH_STATE(priv, IN_TEST_CASE);
            test_case = cut_test_case_new_empty();
            cut_test_result_set_test_case(priv->result, test_case);
            PUSH_TEST_CASE(priv, test_case);
            g_object_unref(test_case);
        } else if (g_str_equal("test", element_name)) {
            CutTest *test;

            PUSH_STATE(priv, IN_TEST);
            test = cut_test_new_empty();
            cut_test_result_set_test(priv->result, test);
            PUSH_TEST(priv, test);
            g_object_unref(test);
        } else if (g_str_equal("status", element_name)) {
            PUSH_STATE(priv, IN_RESULT_STATUS);
        } else if (g_str_equal("detail", element_name)) {
            PUSH_STATE(priv, IN_RESULT_DETAIL);
        } else if (g_str_equal("backtrace", element_name)) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE);
        } else if (g_str_equal("elapsed", element_name)) {
            PUSH_STATE(priv, IN_RESULT_ELAPSED);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_COMPLETE_TEST:
        if (g_str_equal("test", element_name)) {
            PUSH_STATE(priv, IN_TEST);
            priv->complete_test->test = cut_test_new_empty();
            PUSH_TEST(priv, priv->complete_test->test);
        } else if (g_str_equal("test-context", element_name)) {
            PUSH_STATE(priv, IN_TEST_CONTEXT);
            priv->complete_test->test_context = cut_test_context_new_empty();
            PUSH_TEST_CONTEXT(priv, priv->complete_test->test_context);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TEST_CASE_RESULT:
        if (g_str_equal("test-case", element_name)) {
            PUSH_STATE(priv, IN_TEST_CASE);
            priv->test_case_result->test_case = cut_test_case_new_empty();
            PUSH_TEST_CASE(priv, priv->test_case_result->test_case);
        } else if (g_str_equal("result", element_name)) {
            PUSH_STATE(priv, IN_RESULT);
            priv->test_case_result->result = cut_test_result_new_empty();
            priv->result = priv->test_case_result->result;
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_COMPLETE_TEST_CASE:
        if (g_str_equal("test-case", element_name)) {
            PUSH_STATE(priv, IN_TEST_CASE);
            PUSH_TEST_CASE(priv, cut_test_case_new_empty());
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_COMPLETE_TEST_SUITE:
        if (g_str_equal("test-suite", element_name)) {
            PUSH_STATE(priv, IN_TEST_SUITE);
            PUSH_TEST_SUITE(priv, cut_test_suite_new());
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_CRASHED:
        if (g_str_equal("backtrace", element_name)) {
            PUSH_STATE(priv, IN_CRASHED_BACKTRACE);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TEST_SUITE:
      case IN_TEST_CASE:
      case IN_TEST:
        if (g_str_equal("name", element_name)) {
            PUSH_STATE(priv, IN_TEST_NAME);
        } else if (g_str_equal("description", element_name)) {
            PUSH_STATE(priv, IN_TEST_DESCRIPTION);
        } else if (g_str_equal("option", element_name)) {
            PUSH_STATE(priv, IN_TEST_OPTION);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TEST_OPTION:
        if (g_str_equal("name", element_name)) {
            PUSH_STATE(priv, IN_TEST_OPTION_NAME);
        } else if (g_str_equal("value", element_name)) {
            PUSH_STATE(priv, IN_TEST_OPTION_VALUE);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_RESULT_BACKTRACE:
        if (g_str_equal("entry", element_name)) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE_ENTRY);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_RESULT_BACKTRACE_ENTRY:
        if (g_str_equal("file", element_name)) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE_ENTRY_FILE);
        } else if (g_str_equal("line", element_name)) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE_ENTRY_LINE);
        } else if (g_str_equal("info", element_name)) {
            PUSH_STATE(priv, IN_RESULT_BACKTRACE_ENTRY_INFO);
        } else {
            invalid_element(context, error);
        }
        break;
      case IN_TEST_CONTEXT:
        if (g_str_equal("test-suite", element_name)) {
            CutTestSuite *test_suite;

            PUSH_STATE(priv, IN_TEST_SUITE);
            test_suite = cut_test_suite_new();
            cut_test_context_set_test_suite(PEEK_TEST_CONTEXT(priv), test_suite);
            PUSH_TEST_SUITE(priv, test_suite);
            g_object_unref(test_suite);
        } else if (g_str_equal("test-case", element_name)) {
            CutTestCase *test_case;

            PUSH_STATE(priv, IN_TEST_CASE);
            test_case = cut_test_case_new_empty();
            cut_test_context_set_test_case(PEEK_TEST_CONTEXT(priv), test_case);
            PUSH_TEST_CASE(priv, test_case);
            g_object_unref(test_case);
        } else if (g_str_equal("test", element_name)) {
            CutTest *test;

            PUSH_STATE(priv, IN_TEST);
            test = cut_test_new_empty();
            cut_test_context_set_test(PEEK_TEST_CONTEXT(priv), test);
            PUSH_TEST(priv, test);
            g_object_unref(test);
        } else if (g_str_equal("failed", element_name)) {
            PUSH_STATE(priv, IN_TEST_CONTEXT_FAILED);
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
      case IN_TOP_LEVEL_RESULT:
        if (priv->result) {
            g_object_unref(priv->result);
            priv->result = NULL;
        }
        break;
      case IN_STREAM:
        if (priv->run_context)
            g_signal_emit_by_name(priv->run_context,
                                  "complete-run", priv->success);
        break;
      case IN_RESULT:
        if (priv->result) {
            g_signal_emit_by_name(parser, "result", priv->result);
        }
        break;
      case IN_TEST_OPTION:
        {
            gchar *path = NULL;

            if (!priv->option_name) {
                path = element_path(context);
                set_parse_error(context, error,
                                "option name is not set: %s", path);
            } else if (!priv->option_value) {
                path = element_path(context);
                set_parse_error(context, error,
                                "option value is not set: %s", path);
            } else {
                cut_test_set_attribute(PEEK_TEST(priv),
                                       priv->option_name,
                                       priv->option_value);
            }

            if (path)
                g_free(path);

            if (priv->option_name) {
                g_free(priv->option_name);
                priv->option_name = NULL;
            }

            if (priv->option_value) {
                g_free(priv->option_value);
                priv->option_value = NULL;
            }
        }
        break;
      case IN_READY_TEST_SUITE:
        if (priv->ready_test_suite) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context, "ready-test-suite",
                                      priv->ready_test_suite->test_suite,
                                      priv->ready_test_suite->n_test_cases,
                                      priv->ready_test_suite->n_tests);
            if (priv->ready_test_suite->test_suite)
                DROP_TEST_SUITE(priv);
            ready_test_suite_free(priv->ready_test_suite);
            priv->ready_test_suite = NULL;
        }
        break;
      case IN_START_TEST_SUITE:
        {
            CutTestSuite *test_suite;

            test_suite = POP_TEST_SUITE(priv);
            if (test_suite) {
                if (priv->run_context)
                    g_signal_emit_by_name(priv->run_context, "start-test-suite",
                                          test_suite);
                g_object_unref(test_suite);
            }
        }
        break;
      case IN_READY_TEST_CASE:
        if (priv->ready_test_case) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context, "ready-test-case",
                                      priv->ready_test_case->test_case,
                                      priv->ready_test_case->n_tests);
            if (priv->ready_test_case->test_case)
                DROP_TEST_CASE(priv);
            ready_test_case_free(priv->ready_test_case);
            priv->ready_test_case = NULL;
        }
        break;
      case IN_START_TEST_CASE:
        {
            CutTestCase *test_case;

            test_case = POP_TEST_CASE(priv);
            if (test_case) {
                if (priv->run_context)
                    g_signal_emit_by_name(priv->run_context, "start-test-case",
                                          test_case);
                g_object_unref(test_case);
            }
        }
        break;
      case IN_START_TEST:
        if (priv->start_test) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context, "start-test",
                                      priv->start_test->test,
                                      priv->start_test->test_context);
            if (priv->start_test->test)
                DROP_TEST(priv);
            if (priv->start_test->test_context)
                DROP_TEST_CONTEXT(priv);
            start_test_free(priv->start_test);
            priv->start_test = NULL;
        }
        break;
      case IN_TEST_RESULT:
        if (priv->test_result) {
            if (priv->run_context) {
                CutTestResult *result;
                CutTestResultStatus status;
                const gchar *signal_name;
                gchar *full_signal_name;

                result = priv->test_result->result;
                status = cut_test_result_get_status(result);
                signal_name = cut_test_result_status_to_signal_name(status);
                full_signal_name = g_strdup_printf("%s-test", signal_name);
                g_signal_emit_by_name(priv->run_context, full_signal_name,
                                      priv->test_result->test,
                                      priv->test_result->test_context,
                                      result);
                g_free(full_signal_name);
            }
            if (priv->test_result->test)
                DROP_TEST(priv);
            if (priv->test_result->test_context)
                DROP_TEST_CONTEXT(priv);
            test_result_free(priv->test_result);
            priv->test_result = NULL;
            priv->result = NULL;
        }
        break;
      case IN_COMPLETE_TEST:
        if (priv->complete_test) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context, "complete-test",
                                      priv->complete_test->test,
                                      priv->complete_test->test_context);
            if (priv->complete_test->test)
                DROP_TEST(priv);
            if (priv->complete_test->test_context)
                DROP_TEST_CONTEXT(priv);
            complete_test_free(priv->complete_test);
            priv->complete_test = NULL;
        }
        break;
      case IN_TEST_CASE_RESULT:
        if (priv->test_case_result) {
            if (priv->run_context) {
                CutTestResult *result;
                CutTestResultStatus status;
                const gchar *signal_name;
                gchar *full_signal_name;

                result = priv->test_case_result->result;
                status = cut_test_result_get_status(result);
                signal_name = cut_test_result_status_to_signal_name(status);
                full_signal_name = g_strdup_printf("%s-test-case", signal_name);
                g_signal_emit_by_name(priv->run_context, full_signal_name,
                                      priv->test_case_result->test_case,
                                      priv->test_case_result->result);
                g_free(full_signal_name);
            }
            if (priv->test_case_result->test_case)
                DROP_TEST_CASE(priv);
            test_case_result_free(priv->test_case_result);
            priv->test_case_result = NULL;
            priv->result = NULL;
        }
        break;
      case IN_COMPLETE_TEST_CASE:
        {
            CutTestCase *test_case;

            test_case = POP_TEST_CASE(priv);
            if (test_case) {
                if (priv->run_context)
                    g_signal_emit_by_name(priv->run_context,
                                          "complete-test-case", test_case);
                g_object_unref(test_case);
            }
        }
        break;
      case IN_COMPLETE_TEST_SUITE:
        {
            CutTestSuite *test_suite;

            test_suite = POP_TEST_SUITE(priv);
            if (test_suite) {
                if (priv->run_context)
                    g_signal_emit_by_name(priv->run_context,
                                          "complete-test-suite", test_suite);
                g_object_unref(test_suite);
            }
        }
        break;
      case IN_CRASHED_BACKTRACE:
        if (priv->crashed_backtrace) {
            if (priv->run_context)
                g_signal_emit_by_name(priv->run_context,
                                      "crashed", priv->crashed_backtrace);
            g_free(priv->crashed_backtrace);
            priv->crashed_backtrace = NULL;
        }
        break;
      default:
        break;
    }
}

static CutTestResultStatus
result_name_to_status (const gchar *name)
{
    if (g_str_equal(name, "success"))
        return CUT_TEST_RESULT_SUCCESS;
    else if (g_str_equal(name, "failure"))
        return CUT_TEST_RESULT_FAILURE;
    else if (g_str_equal(name, "error"))
         return CUT_TEST_RESULT_ERROR;
    else if (g_str_equal(name, "pending"))
         return CUT_TEST_RESULT_PENDING;
    else if (g_str_equal(name, "notification"))
        return CUT_TEST_RESULT_NOTIFICATION;
    else if (g_str_equal(name, "notification"))
        return CUT_TEST_RESULT_NOTIFICATION;
    else if (g_str_equal(name, "omission"))
        return CUT_TEST_RESULT_OMISSION;

    return CUT_TEST_RESULT_INVALID;
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
        set_parse_error(context, error, "invalid line number: %s", text);
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
        set_parse_error(context, error, "invalid function name: %s", name);
    }
}

static CutTest *
target_test_object (CutStreamParserPrivate *priv, ParseState parent_state)
{
    CutTest *target = NULL;

    switch (parent_state) {
      case IN_TEST_SUITE:
        target = CUT_TEST(PEEK_TEST_SUITE(priv));
        break;
      case IN_TEST_CASE:
        target = CUT_TEST(PEEK_TEST_CASE(priv));
        break;
      case IN_TEST:
        target = PEEK_TEST(priv);
        break;
      default:
        break;
    }

    return target;
}

static gboolean
string_to_boolean (const gchar *string)
{
    return g_str_equal("TRUE", string);
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
        if (priv->option_name) {
            set_parse_error(context, error, "multiple option name: %s", text);
        } else {
            priv->option_name = g_strdup(text);
        }
        break;
      case IN_TEST_OPTION_VALUE:
        if (priv->option_value) {
            set_parse_error(context, error, "multiple option value: %s", text);
        } else {
            priv->option_value = g_strdup(text);
        }
        break;
      case IN_RESULT_STATUS:
        {
            CutTestResultStatus result_status;

            result_status = result_name_to_status(text);
            if (result_status == CUT_TEST_RESULT_INVALID) {
                set_parse_error(context, error, "invalid status: %s", text);
            } else {
                cut_test_result_set_status(priv->result, result_status);
            }
        }
        break;
      case IN_RESULT_DETAIL:
        cut_test_result_set_message(priv->result, text);
        break;
      case IN_RESULT_BACKTRACE_ENTRY_FILE:
        cut_test_result_set_filename(priv->result, text);
        break;
      case IN_RESULT_BACKTRACE_ENTRY_LINE:
        set_line(context, priv, text, error);
        break;
      case IN_RESULT_BACKTRACE_ENTRY_INFO:
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
      case IN_TEST_CONTEXT_FAILED:
        cut_test_context_set_failed(PEEK_TEST_CONTEXT(priv),
                                    string_to_boolean(text));
        break;
      case IN_STREAM_SUCCESS:
        priv->success = string_to_boolean(text);
      case IN_CRASHED_BACKTRACE:
        priv->crashed_backtrace = g_strdup(text);
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
