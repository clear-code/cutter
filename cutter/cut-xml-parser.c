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

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-xml-parser.h"

typedef enum {
    STATE_NONE,
    STATE_RESULT,
    STATE_TEST_CASE,
    STATE_TEST_CASE_NAME,
    STATE_TEST,
    STATE_TEST_NAME,
    STATE_DESCRIPTION,
    STATE_OPTION,
    STATE_OPTION_NAME,
    STATE_OPTION_VALUE,
    STATE_BACKTRACE,
    STATE_FILE,
    STATE_LINE,
    STATE_INFO,
    STATE_STATUS,
    STATE_ELAPSED,
} CutXMLState;

typedef struct _ParseData
{
    GQueue *states;
    CutTestResult *result;

} ParseData;


static CutXMLState
get_current_state (ParseData *data)
{
    return GPOINTER_TO_INT(g_queue_peek_head(data->states));
}

static void
push_state (ParseData *data, CutXMLState state)
{
    g_queue_push_head(data->states, GINT_TO_POINTER(state));
}

static CutXMLState
pop_state (ParseData *data)
{
    return GPOINTER_TO_INT(g_queue_pop_head(data->states));
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
                         G_MARKUP_ERROR_INVALID_CONTENT,
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
    ParseData *data = (ParseData *)user_data;

    if (!g_ascii_strcasecmp("result", element_name)) {
        data->result = g_object_new(CUT_TYPE_TEST_RESULT, NULL);
        push_state(data, STATE_RESULT);
    } else if (!g_ascii_strcasecmp("test-case", element_name)) {
        CutTestCase *test_case;
        test_case = cut_test_case_new(NULL,
                                      NULL, NULL,
                                      NULL, NULL,
                                      NULL, NULL);
        cut_test_result_set_test_case(data->result,
                                      test_case);
        g_object_unref(test_case); 
        push_state(data, STATE_TEST_CASE);
    } else if (!g_ascii_strcasecmp("test", element_name)) {
        CutTest *test;
        test = cut_test_new(NULL, NULL);
        cut_test_result_set_test(data->result, test);
        g_object_unref(test); 
        push_state(data, STATE_TEST);
    } else if (!g_ascii_strcasecmp("name", element_name)) {
        switch (get_current_state(data)) {
          case STATE_OPTION:
            push_state(data, STATE_OPTION_NAME);
            break;
          case STATE_TEST:
            push_state(data, STATE_TEST_NAME);
            break;
          case STATE_TEST_CASE:
            push_state(data, STATE_TEST_CASE_NAME);
            break;
          default:
            set_parse_error(context, error, 
                            "<name> element should be in <option> or <test-case> or <test>.");
            break;
        }
    } else if (!g_ascii_strcasecmp("description", element_name)) {
        push_state(data, STATE_DESCRIPTION);
    } else if (!g_ascii_strcasecmp("option", element_name)) {
        push_state(data, STATE_OPTION);
    } else if (!g_ascii_strcasecmp("value", element_name)) {
        push_state(data, STATE_OPTION_VALUE);
    } else if (!g_ascii_strcasecmp("backtrace", element_name)) {
        push_state(data, STATE_BACKTRACE);
    } else if (!g_ascii_strcasecmp("file", element_name)) {
        push_state(data, STATE_FILE);
    } else if (!g_ascii_strcasecmp("line", element_name)) {
        push_state(data, STATE_LINE);
    } else if (!g_ascii_strcasecmp("info", element_name)) {
        push_state(data, STATE_INFO);
    } else if (!g_ascii_strcasecmp("status", element_name)) {
        push_state(data, STATE_STATUS);
    } else if (!g_ascii_strcasecmp("elapsed", element_name)) {
        push_state(data, STATE_ELAPSED);
    }
}

static void
end_element_handler (GMarkupParseContext *context,
                     const gchar         *element_name,
                     gpointer             user_data,
                     GError             **error)
{
    ParseData *data = (ParseData *)user_data;

    if (!g_ascii_strcasecmp("result", element_name)) {
        if (pop_state(data) != STATE_RESULT) {
            set_parse_error(context, error,
                            "<result> is not closed");
            return;  
        }
    }

    pop_state(data);
}

static CutTestResultStatus
result_name_to_status (const gchar *name)
{
    if (!g_ascii_strcasecmp(name, "success"))
        return CUT_TEST_RESULT_SUCCESS;
    else if (!g_ascii_strcasecmp(name, "failure"))
        return CUT_TEST_RESULT_FAILURE;
    else if (!g_ascii_strcasecmp(name, "error"))
         return CUT_TEST_RESULT_ERROR;
    else if (!g_ascii_strcasecmp(name, "pending"))
         return CUT_TEST_RESULT_PENDING;
    else if (!g_ascii_strcasecmp(name, "notification"))
        return CUT_TEST_RESULT_NOTIFICATION;

    return -1;
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
text_handler (GMarkupParseContext *context,
              const gchar         *text,
              gsize                text_len,
              gpointer             user_data,
              GError             **error)
{
    ParseData *data = (ParseData *)user_data;
    const gchar *element, *parent;

    element = g_markup_parse_context_get_element(context);
    parent = get_parent_element(context);
    
    if (!g_ascii_strcasecmp("name", element)) {
        if (!g_ascii_strcasecmp("test-case", parent)) {
            cut_test_set_name(CUT_TEST(cut_test_result_get_test_case(data->result)),
                              text);
        } else if (!g_ascii_strcasecmp("test", parent)) {
            cut_test_set_name(cut_test_result_get_test(data->result), text);
        } else if (!g_ascii_strcasecmp("option", parent)) {
        }
    } else if (!g_ascii_strcasecmp("description", element)) {
        push_state(data, STATE_DESCRIPTION);
    } else if (!g_ascii_strcasecmp("value", element)) {
        if (!g_ascii_strcasecmp("option", parent)) {
        } else {
            set_parse_error(context, error, 
                            "<value> element should be in <option>.");
        }
    } else if (!g_ascii_strcasecmp("detail", element)) {
        cut_test_result_set_system_message(data->result, text);
    } else if (!g_ascii_strcasecmp("file", element)) {
        cut_test_result_set_filename(data->result, text);
    } else if (!g_ascii_strcasecmp("line", element)) {
        cut_test_result_set_line(data->result, atoi(text));
    } else if (!g_ascii_strcasecmp("info", element)) {
        cut_test_result_set_message(data->result, text);
    } else if (!g_ascii_strcasecmp("status", element)) {
        cut_test_result_set_status(data->result, result_name_to_status(text));
    } else if (!g_ascii_strcasecmp("elapsed", element)) {
        cut_test_result_set_elapsed(data->result, g_ascii_strtod(text, NULL));
    }
}

static void
passthrough_handler (GMarkupParseContext *context,
                     const gchar         *text,
                     gsize                text_len,
                     gpointer             user_data,
                     GError             **error)
{
}

static void
error_handler (GMarkupParseContext *context,
               GError              *error,
               gpointer             user_data)
{
}

static GMarkupParser parser = {
    start_element_handler,
    end_element_handler,
    text_handler,
    passthrough_handler,
    error_handler,
};

static void
init_parse_data (ParseData *data)
{
    data->result = NULL;
    data->states = g_queue_new();
}

static void
free_parse_data (ParseData *data)
{
    g_queue_free(data->states);
}

CutTestResult *
cut_xml_parse_test_result_xml (const gchar *xml, gssize len)
{
    GMarkupParseContext *parse_context;
    ParseData data;
    GError *error = NULL;

    init_parse_data(&data);

    parse_context = g_markup_parse_context_new(&parser, 
                                               G_MARKUP_TREAT_CDATA_AS_TEXT,
                                               &data, NULL);
    if (!g_markup_parse_context_parse(parse_context, xml, len, &error)) {
        g_error_free(error);
    }

    g_markup_parse_context_free(parse_context);
    free_parse_data(&data);

    return data.result;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
