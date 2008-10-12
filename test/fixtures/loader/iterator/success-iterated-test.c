/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>

#include <cutter.h>

void data_translate (void);
void test_translate (const void *data);

static const char*
translate (int input)
{
   switch(input) {
   case 1:
       return "first";
   case 111:
       return "a hundred eleven";
   default:
       return "unsupported";
   }
}

typedef struct _TranslateTestData
{
    char *translated;
    int input;
} TranslateTestData;

static TranslateTestData *
translate_test_data_new (char *translated, int input)
{
    TranslateTestData *data;

    data = malloc(sizeof(TranslateTestData));
    data->translated = strdup(translated);
    data->input = input;

    return data;
}

static void
translate_test_data_free (TranslateTestData *data)
{
    free(data->translated);
    free(data);
}

void
data_translate(void)
{
    cut_add_data("simple data",
                 translate_test_data_new("first", 1),
                 translate_test_data_free,
                 "complex data",
                 translate_test_data_new("a hundred eleven", 111),
                 translate_test_data_free);
}

void
test_translate(const void *data)
{
     const TranslateTestData *test_data = data;

     cut_assert_equal_string(test_data->translated,
                             translate(test_data->input));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
