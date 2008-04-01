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
    STATE_TEST,
    STATE_TEST_NAME,
    STATE_DESCRIPTION,
    STATE_OPTION,
    STATE_OPTION_NAME,
    STATE_OPTION_VALUE,
    STATE_BACKTRACE,
    STATE_FILE,
    STATE_LINE,
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
start_element_handler (GMarkupParseContext *context,
                       const gchar         *element_name,
                       const gchar        **attr_names,
                       const gchar        **attr_values,
                       gpointer             user_data,
                       GError             **error)
{
    ParseData *data = (ParseData *)user_data;

    if (!strcmp("result", element_name)) {
        data->result = g_object_new(CUT_TYPE_TEST_RESULT, NULL);
        push_state(data, STATE_RESULT);
    }

    if (!strcmp("test_case", element_name)) {
        CutTestCase *test_case;
        test_case = cut_test_case_new(NULL,
                                      NULL, NULL,
                                      NULL, NULL,
                                      NULL, NULL);
        cut_test_result_set_test_case(data->result,
                                      test_case);
        g_object_unref(test_case); 
        push_state(data, STATE_TEST_CASE);
    }

    if (!strcmp("test", element_name)) {
        CutTest *test;
        test = cut_test_new(NULL, NULL);
        cut_test_result_set_test(data->result, test);
        g_object_unref(test); 
        push_state(data, STATE_TEST);
    }

    if (!strcmp("name", element_name)) {
        if (get_current_state(data) == STATE_OPTION)
            push_state(data, STATE_OPTION_NAME);
        else
            push_state(data, STATE_TEST_NAME);
    }

    if (!strcmp("description", element_name)) {
        push_state(data, STATE_DESCRIPTION);
    }

    if (!strcmp("option", element_name)) {
        push_state(data, STATE_OPTION);
    }

    if (!strcmp("value", element_name)) {
        push_state(data, STATE_OPTION_VALUE);
    }

    if (!strcmp("backtrace", element_name)) {
        push_state(data, STATE_BACKTRACE);
    }

    if (!strcmp("file", element_name)) {
        push_state(data, STATE_FILE);
    }

    if (!strcmp("line", element_name)) {
        push_state(data, STATE_LINE);
    }

    if (!strcmp("status", element_name)) {
        push_state(data, STATE_STATUS);
    }

    if (!strcmp("elapsed", element_name)) {
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

    if (!strcmp("result", element_name)) {
        if (pop_state(data) != STATE_RESULT) {
            *error = g_error_new (G_MARKUP_ERROR,
                                  G_MARKUP_ERROR_INVALID_CONTENT,
                                  "result element is not closed");
            return;  
        }
    }

    pop_state(data);
}

static CutTestResultStatus
result_name_to_status (const gchar *name)
{
    if (!strcmp(name, "success"))
        return CUT_TEST_RESULT_SUCCESS;
    if (!strcmp(name, "failure"))
        return CUT_TEST_RESULT_FAILURE;
    if (!strcmp(name, "error"))
        return CUT_TEST_RESULT_ERROR;
    if (!strcmp(name, "pending"))
        return CUT_TEST_RESULT_PENDING;
    if (!strcmp(name, "notification"))
        return CUT_TEST_RESULT_NOTIFICATION;

    return -1;
}

static void
text_handler (GMarkupParseContext *context,
              const gchar         *text,
              gsize                text_len,
              gpointer             user_data,
              GError             **error)
{
    ParseData *data = (ParseData *)user_data;

    switch(get_current_state(data)) {
      case STATE_TEST_NAME:
        g_object_set(cut_test_result_get_test(data->result),
                     "name", text, NULL);
        break;
      case STATE_FILE:
        cut_test_result_set_filename(data->result, text);
        break;
      case STATE_LINE:
        cut_test_result_set_line(data->result, atoi(text));
        break;
      case STATE_STATUS:
        cut_test_result_set_status(data->result, result_name_to_status(text));
        break;
      case STATE_ELAPSED:
        cut_test_result_set_elapsed(data->result, g_ascii_strtod(text, NULL));
        break;
      default:
        break;
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
