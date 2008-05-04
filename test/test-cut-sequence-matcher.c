/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <cutter/cut-gassertions.h>
#include <cutter/cut-sequence-matcher.h>

void test_to_indexes_for_string_sequence(void);
void test_to_indexes_for_char_sequence(void);
void test_longest_match_for_string_sequence(void);
void test_longest_match_for_char_sequence(void);

static CutSequenceMatcher *matcher;
static GList *expected_indexes;
static CutSequenceMatchInfo *actual_info;

void
setup (void)
{
    matcher = NULL;
    expected_indexes = NULL;
    actual_info = NULL;
}

void
teardown (void)
{
    if (matcher)
        g_object_unref(matcher);
    g_list_free(expected_indexes);
    g_free(actual_info);
}

static GSequence *
string_sequence_new (const gchar **strings)
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
create_string_sequence_matcher (const gchar **from_strings,
                                const gchar **to_strings)
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
    const gchar *from[] = {"", NULL};
    const gchar *to[] = {"abc def", "abc", "abc def", NULL};

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

#define cut_assert_longest_match(expected_begin,                        \
                                 expected_end,                          \
                                 expected_size,                         \
                                 sequence_matcher,                      \
                                 sequence_matcher_inspect,              \
                                 from_begin, from_end,                  \
                                 to_begin, to_end) do                   \
{                                                                       \
    gint _expected_begin = (expected_begin);                            \
    gint _expected_end = (expected_end);                                \
    gint _expected_size = (expected_size);                              \
                                                                        \
    matcher = (sequence_matcher);                                       \
    actual_info = cut_sequence_matcher_longest_match(matcher,           \
                                                     from_begin,        \
                                                     from_end,          \
                                                     to_begin,          \
                                                     to_end);           \
    if (_expected_begin == actual_info->begin &&                        \
        _expected_end == actual_info->end &&                            \
        _expected_size == actual_info->size) {                          \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("cut_sequence_matcher_longest_match(" \
                                      "%s, %s, %s, %s, %s)\n"           \
                                      " ==\n"                           \
                                      "<begin: %s, end: %s, size: %s>"  \
                                      "\n"                              \
                                      "expected: "                      \
                                      "<begin: %d, end: %d, size: %d>"  \
                                      "\n"                              \
                                      " but was: "                      \
                                      "<begin: %d, end: %d, size: %d>", \
                                      sequence_matcher_inspect,         \
                                      #from_begin,                      \
                                      #from_end,                        \
                                      #to_begin,                        \
                                      #to_end,                          \
                                      #expected_begin,                  \
                                      #expected_end,                    \
                                      #expected_size,                   \
                                      _expected_begin,                  \
                                      _expected_end,                    \
                                      _expected_size,                   \
                                      actual_info->begin,               \
                                      actual_info->end,                 \
                                      actual_info->size));              \
    }                                                                   \
                                                                        \
    g_object_unref(matcher);                                            \
    matcher = NULL;                                                     \
    g_free(actual_info);                                                \
    actual_info = NULL;                                                 \
} while (0)

#define cut_assert_longest_match_string(expected_begin,                 \
                                        expected_end,                   \
                                        expected_size,                  \
                                        from, to,                       \
                                        from_begin, from_end,           \
                                        to_begin, to_end) do            \
{                                                                       \
    const gchar **_from = (from);                                       \
    const gchar **_to = (to);                                           \
    CutSequenceMatcher *matcher;                                        \
    const gchar *inspected_matcher;                                     \
                                                                        \
    matcher = create_string_sequence_matcher(_from, _to);               \
    inspected_matcher =                                                 \
        cut_take_printf("[%s, %s]",                                     \
                        cut_inspect_string_array(_from),                \
                        cut_inspect_string_array(_to));                 \
    cut_assert_longest_match(expected_begin, expected_end,              \
                             expected_size,                             \
                             matcher, inspected_matcher,                \
                             from_begin,from_end, to_begin, to_end);    \
} while (0)

void
test_longest_match_for_string_sequence (void)
{
    const gchar *bcd[] = {"b", "c", "d", NULL};
    const gchar *abcdxyz[] = {"a", "b", "c", "d", "x", "y", "z", NULL};
    const gchar *ab[] = {"a", "b", NULL};
    const gchar *c[] = {"c", NULL};
    const gchar *qabxcd[] = {"q", "a", "b", "x", "c", "d", NULL};
    const gchar *abycdf[] = {"a", "b", "y", "c", "d", "f", NULL};

    cut_assert_longest_match_string(0, 1, 3, bcd, abcdxyz, 0, 2, 0, 7);
    cut_assert_longest_match_string(1, 2, 2, bcd, abcdxyz, 1, 2, 0, 6);
    cut_assert_longest_match_string(0, 0, 0, ab, c, 0, 1, 0, 0);
    cut_assert_longest_match_string(1, 0, 2, qabxcd, abycdf, 0, 5, 0, 5);
    cut_assert_longest_match_string(4, 3, 2, qabxcd, abycdf, 3, 5, 2, 5);
}

#define cut_assert_longest_match_char(expected_begin,                   \
                                      expected_end,                     \
                                      expected_size,                    \
                                      from, to,                         \
                                      from_begin, from_end,             \
                                      to_begin, to_end)                 \
    cut_assert_longest_match(expected_begin, expected_end,              \
                             expected_size,                             \
                             create_char_sequence_matcher(from, to),    \
                             cut_take_printf("[%s, %s]", #from, #to),   \
                             from_begin,from_end, to_begin, to_end)

void
test_longest_match_for_char_sequence (void)
{
    cut_assert_longest_match_char(0, 1, 3, "bcd", "abcdxyz", 0, 2, 0, 7);
    cut_assert_longest_match_char(1, 2, 2, "bcd", "abcdxyz", 1, 2, 0, 6);
    cut_assert_longest_match_char(0, 0, 0, "ab", "c", 0, 1, 0, 0);
    cut_assert_longest_match_char(1, 0, 2, "qabxcd", "abycdf", 0, 5, 0, 5);
    cut_assert_longest_match_char(4, 3, 2, "qabxcd", "abycdf", 3, 5, 2, 5);

    cut_assert_longest_match_char(0, 0, 1, "efg", "eg", 0, 2, 0, 1);
    cut_assert_longest_match_char(2, 1, 1, "efg", "eg", 1, 2, 1, 1);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
