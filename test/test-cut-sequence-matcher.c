/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <cutter/cut-gassertions.h>
#include <cutter/cut-sequence-matcher.h>

void test_to_indexes_for_string_sequence(void);
void test_to_indexes_for_char_sequence(void);
void test_get_longest_match_for_string_sequence(void);
void test_get_longest_match_for_char_sequence(void);
void test_get_matches_for_string_sequence(void);
void test_get_matches_for_char_sequence(void);

static CutSequenceMatcher *matcher;
static GList *expected_indexes;
static CutSequenceMatchInfo *actual_info;
static GList *expected_matches;

static void
free_matches (GList *matches)
{
    g_list_foreach(matches, (GFunc)g_free, NULL);
    g_list_free(matches);
}

void
setup (void)
{
    matcher = NULL;
    expected_indexes = NULL;
    actual_info = NULL;
    expected_matches = NULL;
}

void
teardown (void)
{
    if (matcher)
        g_object_unref(matcher);

    g_list_free(expected_indexes);

    g_free(actual_info);

    free_matches(expected_matches);
}

static gboolean
equal_match (const CutSequenceMatchInfo *info1,
             const CutSequenceMatchInfo *info2)
{
    return (info1->begin == info2->begin &&
            info1->end == info2->end &&
            info1->size == info2->size);
}

static gboolean
equal_match_with_data (gconstpointer info1, gconstpointer info2,
                       gpointer user_data)
{
    return equal_match(info1, info2);
}

static gboolean
equal_matches (const GList *matches1, const GList *matches2)
{
    return cut_list_equal(matches1, matches2, equal_match_with_data, NULL);
}

static gchar *
_inspect_match (CutSequenceMatchInfo *info)
{
    return g_strdup_printf("<begin: %d, end: %d, size: %d>",
                           info->begin,
                           info->end,
                           info->size);
}

static const gchar *
inspect_match (CutSequenceMatchInfo *info)
{
    return cut_take_string(_inspect_match(info));
}

static void
inspect_match_func (GString *string, gconstpointer data, gpointer user_data)
{
    g_string_append(string, _inspect_match((CutSequenceMatchInfo *)data));
}

static const gchar *
inspect_matches (const GList *matches)
{
    return cut_take_string(cut_list_inspect(matches, inspect_match_func, NULL));
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

    matcher = cut_sequence_matcher_string_new(from, to);

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
    matcher = cut_sequence_matcher_char_new("", "abcad");

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
    CutSequenceMatchInfo info;                                          \
                                                                        \
    info.begin = (expected_begin);                                      \
    info.end = (expected_end);                                          \
    info.size = (expected_size);                                        \
    matcher = (sequence_matcher);                                       \
    actual_info = cut_sequence_matcher_get_longest_match(matcher,       \
                                                         from_begin,    \
                                                         from_end,      \
                                                         to_begin,      \
                                                         to_end);       \
    if (equal_match(&info, actual_info)) {                              \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("cut_sequence_matcher_"           \
                                      "get_longest_match("              \
                                      "%s, %s, %s, %s, %s)\n"           \
                                      " ==\n"                           \
                                      "<begin: %s, end: %s, size: %s>"  \
                                      "\n"                              \
                                      "expected: %s\n"                  \
                                      " but was: %s",                   \
                                      sequence_matcher_inspect,         \
                                      #from_begin,                      \
                                      #from_end,                        \
                                      #to_begin,                        \
                                      #to_end,                          \
                                      #expected_begin,                  \
                                      #expected_end,                    \
                                      #expected_size,                   \
                                      inspect_match(&info),             \
                                      inspect_match(actual_info)));     \
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
    const gchar **_from, **_to;                                         \
    CutSequenceMatcher *matcher;                                        \
    const gchar *inspected_matcher;                                     \
                                                                        \
    _from = (from);                                                     \
    _to = (to);                                                         \
    matcher = cut_sequence_matcher_string_new(_from, _to);              \
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
test_get_longest_match_for_string_sequence (void)
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
                             cut_sequence_matcher_char_new(from, to),   \
                             cut_take_printf("[%s, %s]", #from, #to),   \
                             from_begin,from_end, to_begin, to_end)

void
test_get_longest_match_for_char_sequence (void)
{
    cut_assert_longest_match_char(0, 1, 3, "bcd", "abcdxyz", 0, 2, 0, 7);
    cut_assert_longest_match_char(1, 2, 2, "bcd", "abcdxyz", 1, 2, 0, 6);
    cut_assert_longest_match_char(0, 0, 0, "ab", "c", 0, 1, 0, 0);
    cut_assert_longest_match_char(1, 0, 2, "qabxcd", "abycdf", 0, 5, 0, 5);
    cut_assert_longest_match_char(4, 3, 2, "qabxcd", "abycdf", 3, 5, 2, 5);

    cut_assert_longest_match_char(0, 0, 1, "efg", "eg", 0, 2, 0, 1);
    cut_assert_longest_match_char(2, 1, 1, "efg", "eg", 1, 2, 1, 1);
}

static GList *
append_match_info (GList *list, gint begin, gint end, gint size)
{
    CutSequenceMatchInfo *info;

    info = g_new(CutSequenceMatchInfo, 1);
    info->begin = begin;
    info->end = end;
    info->size = size;

    return g_list_append(list, info);
}

#define cut_assert_matches(expected_matches,                            \
                           sequence_matcher,                            \
                           sequence_matcher_inspect) do                 \
{                                                                       \
    GList *_expected_matches;                                           \
    const GList *actual_matches;                                        \
                                                                        \
    _expected_matches = (expected_matches);                             \
    matcher = (sequence_matcher);                                       \
    actual_matches = cut_sequence_matcher_get_matches(matcher);         \
    if (equal_matches(_expected_matches, actual_matches)) {             \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
                                                                        \
        inspected_expected = inspect_matches(_expected_matches);        \
        inspected_actual = inspect_matches(actual_matches);             \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("cut_sequence_matcher_"           \
                                      "get_matches(%s)\n"               \
                                      "expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      sequence_matcher_inspect,         \
                                      inspected_expected,               \
                                      inspected_actual));               \
    }                                                                   \
                                                                        \
    g_object_unref(matcher);                                            \
    matcher = NULL;                                                     \
} while (0)

#define cut_assert_matches_string(expected_matches, from, to) do        \
{                                                                       \
    const gchar **_from, **_to;                                         \
    _from = (from);                                                     \
    _to = (to);                                                         \
                                                                        \
    cut_assert_matches(expected_matches,                                \
                       cut_sequence_matcher_string_new(_from, _to),     \
                       cut_take_printf("[%s, %s]",                      \
                                       cut_inspect_string_array(_from), \
                                       cut_inspect_string_array(_to))); \
} while (0)

void
test_get_matches_for_string_sequence (void)
{
    const gchar *abxcd[] = {"a", "b", "x", "c", "d", NULL};
    const gchar *abcd[] = {"a", "b", "c", "d", NULL};
    const gchar *qabxcd[] = {"q", "a", "b", "x", "c", "d", NULL};
    const gchar *abycdf[] = {"a", "b", "y", "c", "d", "f", NULL};
    const gchar *efg[] = {"e", "f", "g", NULL};
    const gchar *eg[] = {"e", "g", NULL};

    expected_matches = append_match_info(NULL, 0, 0, 2);
    expected_matches = append_match_info(expected_matches, 3, 2, 2);
    cut_assert_matches_string(expected_matches, abxcd, abcd);
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 1, 0, 2);
    expected_matches = append_match_info(expected_matches, 4, 3, 2);
    cut_assert_matches_string(expected_matches, qabxcd, abycdf);
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 0, 0, 1);
    expected_matches = append_match_info(expected_matches, 2, 1, 1);
    cut_assert_matches_string(expected_matches, efg, eg);
}

#define cut_assert_matches_char(expected_matches, from, to)             \
    cut_assert_matches(expected_matches,                                \
                       cut_sequence_matcher_char_new(from, to),         \
                       cut_take_printf("[%s, %s]", #from, #to))

void
test_get_matches_for_char_sequence (void)
{
    expected_matches = append_match_info(NULL, 0, 0, 2);
    expected_matches = append_match_info(expected_matches, 3, 2, 2);
    cut_assert_matches_char(expected_matches, "abxcd", "abcd");
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 1, 0, 2);
    expected_matches = append_match_info(expected_matches, 4, 3, 2);
    cut_assert_matches_char(expected_matches, "qabxcd", "abycdf");
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 0, 0, 1);
    expected_matches = append_match_info(expected_matches, 2, 1, 1);
    cut_assert_matches_char(expected_matches, "efg", "eg");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
