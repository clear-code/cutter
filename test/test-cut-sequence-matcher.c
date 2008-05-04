/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <cutter/cut-gassertions.h>
#include <cutter/cut-sequence-matcher.h>

void test_to_indexes_for_string_sequence(void);
void test_to_indexes_for_char_sequence(void);
void test_longest_match(void);

static CutSequenceMatcher *matcher;
static GList *expected_indexes;

void
setup (void)
{
    matcher = NULL;
    expected_indexes = NULL;
}

void
teardown (void)
{
    if (matcher)
        g_object_unref(matcher);
    g_list_free(expected_indexes);
}

static GSequence *
string_sequence_new (gchar **strings)
{
    GSequence *sequence;

    sequence = g_sequence_new(g_free);

    for (; *strings; strings++) {
        g_sequence_append(sequence, g_strdup(*strings));
    }

    return sequence;
}

static CutSequenceMatcher *
string_sequence_matcher_new (GSequence *from, GSequence *to)
{
    return cut_sequence_matcher_new(from, to, NULL, NULL,
                                    g_str_hash, g_str_equal);
}

static CutSequenceMatcher *
create_string_sequence_matcher (gchar **from_strings, gchar **to_strings)
{
    return string_sequence_matcher_new(string_sequence_new(from_strings),
                                       string_sequence_new(to_strings));
}

static GSequence *
char_sequence_new (gchar *string)
{
    GSequence *sequence;

    sequence = g_sequence_new(NULL);

    for (; *string != '\0'; string++) {
        g_sequence_append(sequence, GINT_TO_POINTER(*string));
    }

    return sequence;
}

static guint
int_hash (gconstpointer v)
{
    gint integer;
    integer = GPOINTER_TO_INT(v);
    return g_int_hash(&integer);
}

static gboolean
int_equal (gconstpointer v1, gconstpointer v2)
{
    gint integer1, integer2;
    integer1 = GPOINTER_TO_INT(v1);
    integer2 = GPOINTER_TO_INT(v2);
    return g_int_equal(&integer1, &integer2);
}

static CutSequenceMatcher *
char_sequence_matcher_new (GSequence *from, GSequence *to)
{
    return cut_sequence_matcher_new(from, to, NULL, NULL, int_hash, int_equal);
}

static CutSequenceMatcher *
create_char_sequence_matcher (gchar *from_string, gchar *to_string)
{
    return char_sequence_matcher_new(char_sequence_new(from_string),
                                       char_sequence_new(to_string));
}


#define cut_assert_to_index(expected, matcher, to_content) do           \
{                                                                       \
    const GList *indexes;                                               \
    indexes = cut_sequence_matcher_get_to_index((matcher),              \
                                                (to_content));          \
    cut_assert_equal_g_list_int(expected, indexes);                     \
} while (0)

void
test_to_indexes_for_string_sequence (void)
{
    gchar *from[] = {"", NULL};
    gchar *to[] = {"abc def", "abc", "abc def", NULL};

    matcher = create_string_sequence_matcher(from, to);

    expected_indexes = g_list_append(expected_indexes, GINT_TO_POINTER(0));
    expected_indexes = g_list_append(expected_indexes, GINT_TO_POINTER(2));
    cut_assert_to_index(expected_indexes, matcher, "abc def");
    g_list_free(expected_indexes);

    expected_indexes = g_list_append(NULL, GINT_TO_POINTER(1));
    cut_assert_to_index(expected_indexes, matcher, "abc");
}

void
test_to_indexes_for_char_sequence (void)
{
    matcher = create_char_sequence_matcher("", "abcad");

    expected_indexes = g_list_append(expected_indexes, GINT_TO_POINTER(0));
    expected_indexes = g_list_append(expected_indexes, GINT_TO_POINTER(3));
    cut_assert_to_index(expected_indexes, matcher, GINT_TO_POINTER('a'));
    g_list_free(expected_indexes);

    expected_indexes = g_list_append(NULL, GINT_TO_POINTER(1));
    cut_assert_to_index(expected_indexes, matcher, GINT_TO_POINTER('b'));
    g_list_free(expected_indexes);

    expected_indexes = g_list_append(NULL, GINT_TO_POINTER(2));
    cut_assert_to_index(expected_indexes, matcher, GINT_TO_POINTER('c'));
    g_list_free(expected_indexes);

    expected_indexes = g_list_append(NULL, GINT_TO_POINTER(4));
    cut_assert_to_index(expected_indexes, matcher, GINT_TO_POINTER('d'));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
