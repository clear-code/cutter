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
void test_get_blocks_for_string_sequence(void);
void test_get_blocks_for_char_sequence(void);
void test_get_operations_for_string_sequence(void);
void test_get_operations_for_char_sequence(void);

static CutSequenceMatcher *matcher;
static GList *expected_indexes;
static CutSequenceMatchInfo *actual_info;
static GList *expected_matches;
static GList *expected_operations;

static void
free_matches (GList *matches)
{
    g_list_foreach(matches, (GFunc)cut_sequence_match_info_free, NULL);
    g_list_free(matches);
}

static void
free_operations (GList *operations)
{
    g_list_foreach(operations, (GFunc)cut_sequence_match_operation_free, NULL);
    g_list_free(operations);
}

void
setup (void)
{
    matcher = NULL;
    expected_indexes = NULL;
    actual_info = NULL;
    expected_matches = NULL;
    expected_operations = NULL;
}

void
teardown (void)
{
    if (matcher)
        g_object_unref(matcher);

    g_list_free(expected_indexes);

    g_free(actual_info);

    free_matches(expected_matches);

    free_operations(expected_operations);
}

static const gchar *
inspect_string_matcher (const gchar **from, const gchar **to)
{
    return cut_take_printf("[%s, %s]",
                           cut_inspect_string_array(from),
                           cut_inspect_string_array(to));
}

#define inspect_char_matcher(from, to)          \
    cut_take_printf("[%s, %s]", #from, #to)

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
                                                                        \
    _from = (from);                                                     \
    _to = (to);                                                         \
    matcher = cut_sequence_matcher_string_new(_from, _to);              \
    cut_assert_longest_match(expected_begin, expected_end,              \
                             expected_size,                             \
                             matcher,                                   \
                             inspect_string_matcher(_from, _to),        \
                             from_begin, from_end, to_begin, to_end);   \
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
                             inspect_char_matcher(from, to),            \
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
    return g_list_append(list, cut_sequence_match_info_new(begin, end, size));
}

#define cut_assert_equal_matches(expected, actual, ...) do              \
{                                                                       \
    const GList *_expected, *_actual;                                   \
                                                                        \
    _expected = (expected);                                             \
    _actual = (actual);                                                 \
    if (equal_matches(_expected, _actual)) {                            \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
                                                                        \
        inspected_expected = inspect_matches(_expected);                \
        inspected_actual = inspect_matches(_actual);                    \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      inspected_expected,               \
                                      inspected_actual),                \
                      ## __VA_ARGS__);                                  \
    }                                                                   \
} while (0)

#define cut_assert_matches(expected_matches,                            \
                           sequence_matcher,                            \
                           sequence_matcher_inspect) do                 \
{                                                                       \
    matcher = (sequence_matcher);                                       \
    cut_assert_equal_matches(expected_matches,                          \
                             cut_sequence_matcher_get_matches(matcher), \
                             "cut_sequence_matcher_get_matches(%s)",    \
                             sequence_matcher_inspect);                 \
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
                       inspect_string_matcher(_from, _to));             \
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
                       inspect_char_matcher(from, to))

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

#define cut_assert_blocks(expected_matches,                             \
                          sequence_matcher,                             \
                          sequence_matcher_inspect) do                  \
{                                                                       \
    matcher = (sequence_matcher);                                       \
    cut_assert_equal_matches(expected_matches,                          \
                             cut_sequence_matcher_get_blocks(matcher),  \
                             "cut_sequence_matcher_get_blocks(%s)",     \
                             sequence_matcher_inspect);                 \
    g_object_unref(matcher);                                            \
    matcher = NULL;                                                     \
} while (0)

#define cut_assert_blocks_string(expected_matches, from, to) do         \
{                                                                       \
    const gchar **_from, **_to;                                         \
    _from = (from);                                                     \
    _to = (to);                                                         \
                                                                        \
    cut_assert_blocks(expected_matches,                                 \
                      cut_sequence_matcher_string_new(_from, _to),      \
                      inspect_string_matcher(_from, _to));              \
} while (0)

void
test_get_blocks_for_string_sequence (void)
{
    const gchar *abxcd[] = {"a", "b", "x", "c", "d", NULL};
    const gchar *abcd[] = {"a", "b", "c", "d", NULL};
    const gchar *qabxcd[] = {"q", "a", "b", "x", "c", "d", NULL};
    const gchar *abycdf[] = {"a", "b", "y", "c", "d", "f", NULL};
    const gchar *efg[] = {"e", "f", "g", NULL};
    const gchar *eg[] = {"e", "g", NULL};

    expected_matches = append_match_info(NULL, 0, 0, 2);
    expected_matches = append_match_info(expected_matches, 3, 2, 2);
    expected_matches = append_match_info(expected_matches, 5, 4, 0);
    cut_assert_blocks_string(expected_matches, abxcd, abcd);
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 1, 0, 2);
    expected_matches = append_match_info(expected_matches, 4, 3, 2);
    expected_matches = append_match_info(expected_matches, 6, 6, 0);
    cut_assert_blocks_string(expected_matches, qabxcd, abycdf);
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 0, 0, 1);
    expected_matches = append_match_info(expected_matches, 2, 1, 1);
    expected_matches = append_match_info(expected_matches, 3, 2, 0);
    cut_assert_blocks_string(expected_matches, efg, eg);
}

#define cut_assert_blocks_char(expected_matches, from, to)              \
    cut_assert_blocks(expected_matches,                                 \
                      cut_sequence_matcher_char_new(from, to),          \
                      inspect_char_matcher(from, to))

void
test_get_blocks_for_char_sequence (void)
{
    expected_matches = append_match_info(NULL, 0, 0, 2);
    expected_matches = append_match_info(expected_matches, 3, 2, 2);
    expected_matches = append_match_info(expected_matches, 5, 4, 0);
    cut_assert_blocks_char(expected_matches, "abxcd", "abcd");
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 1, 0, 2);
    expected_matches = append_match_info(expected_matches, 4, 3, 2);
    expected_matches = append_match_info(expected_matches, 6, 6, 0);
    cut_assert_blocks_char(expected_matches, "qabxcd", "abycdf");
    free_matches(expected_matches);

    expected_matches = append_match_info(NULL, 0, 0, 1);
    expected_matches = append_match_info(expected_matches, 2, 1, 1);
    expected_matches = append_match_info(expected_matches, 3, 2, 0);
    cut_assert_blocks_char(expected_matches, "efg", "eg");
}

static gboolean
equal_operation_with_data (gconstpointer data1, gconstpointer data2,
                           gpointer user_data)
{
    const CutSequenceMatchOperation *operation1, *operation2;

    operation1 = data1;
    operation2 = data2;
    return (operation1->type == operation2->type &&
            operation1->from_begin == operation2->from_begin &&
            operation1->from_end == operation2->from_end &&
            operation1->to_begin == operation2->to_begin &&
            operation1->to_end == operation2->to_end);
}

static gboolean
equal_operations (const GList *operations1, const GList *operations2)
{
    return cut_list_equal(operations1, operations2,
                          equal_operation_with_data, NULL);
}

static const gchar *
operation_type_to_string (CutSequenceMatchOperationType type)
{
    switch (type) {
      case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
        return "EQUAL";
      case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
        return "INSERT";
      case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
        return "DELETE";
      case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
        return "REPLACE";
    }

    return "UNKNOWN";
}

static void
inspect_operation_func (GString *string, gconstpointer data, gpointer user_data)
{
    const CutSequenceMatchOperation *operation = data;

    g_string_append_printf(string,
                           "<type: %s, from_begin: %d, from_end: %d, "
                           "to_begin: %d, to_end: %d>",
                           operation_type_to_string(operation->type),
                           operation->from_begin,
                           operation->from_end,
                           operation->to_begin,
                           operation->to_end);
}

static const gchar *
inspect_operations (const GList *operations)
{
    return cut_take_string(cut_list_inspect(operations,
                                            inspect_operation_func, NULL));
}

#define cut_assert_operations(expected,                                 \
                              sequence_matcher,                         \
                              sequence_matcher_inspect) do              \
{                                                                       \
    const GList *_expected, *_actual;                                   \
                                                                        \
    _expected = (expected);                                             \
    matcher = (sequence_matcher);                                       \
    _actual = cut_sequence_matcher_get_operations(matcher);             \
                                                                        \
    if (equal_operations(_expected, _actual)) {                         \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
                                                                        \
        inspected_expected = inspect_operations(_expected);             \
        inspected_actual = inspect_operations(_actual);                 \
        cut_test_fail(FAILURE,                                          \
                      cut_take_printf("expected: <%s>\n"                \
                                      " but was: <%s>",                 \
                                      inspected_expected,               \
                                      inspected_actual),                \
                      "cut_sequence_matcher_get_operations(%s)",        \
                      sequence_matcher_inspect);                        \
    }                                                                   \
    g_object_unref(matcher);                                            \
    matcher = NULL;                                                     \
} while (0)

#define cut_assert_operations_string(expected_operations, from, to) do  \
{                                                                       \
    const gchar **_from, **_to;                                         \
    _from = (from);                                                     \
    _to = (to);                                                         \
                                                                        \
    cut_assert_operations(expected_operations,                          \
                          cut_sequence_matcher_string_new(_from, _to),  \
                          inspect_string_matcher(_from, _to));          \
} while (0)

static GList *
_append_operation (GList *list, CutSequenceMatchOperationType type,
                   gint from_begin, gint from_end, gint to_begin, gint to_end)
{
    return g_list_append(list,
                         cut_sequence_match_operation_new(type,
                                                          from_begin,
                                                          from_end,
                                                          to_begin,
                                                          to_end));
}

#define append_operation(list, type, from_begin, from_end,              \
                         to_begin, to_end)                              \
    _append_operation(list, CUT_SEQUENCE_MATCH_OPERATION_ ## type,      \
                      from_begin, from_end, to_begin, to_end)

void
test_get_operations_for_string_sequence (void)
{
    const gchar *empty[] = {NULL};
    const gchar *qabxcd[] = {"q", "a", "b", "x", "c", "d", NULL};
    const gchar *abycdf[] = {"a", "b", "y", "c", "d", "f", NULL};
    const gchar *summary_1010[] = {
        "1", " ", "t", "e", "s", "t", "s", ",", " ",
        "0", " ", "a", "s", "s", "e", "r", "t", "i", "o", "n", "s", ",", " ",
        "1", " ", "f", "a", "i", "l", "u", "r", "e", "s", ",", " ",
        "0", " ", "p", "e", "n", "d", "i", "n", "g", "s",
        NULL};
    const gchar *summary_1001[] = {
        "1", " ", "t", "e", "s", "t", "s", ",", " ",
        "0", " ", "a", "s", "s", "e", "r", "t", "i", "o", "n", "s", ",", " ",
        "0", " ", "f", "a", "i", "l", "u", "r", "e", "s", ",", " ",
        "1", " ", "p", "e", "n", "d", "i", "n", "g", "s",
        NULL};

    cut_assert_operations_string(NULL, empty, empty);

    expected_operations = append_operation(NULL, DELETE, 0, 1, 0, 0);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           1, 3, 0, 2);
    expected_operations = append_operation(expected_operations, REPLACE,
                                           3, 4, 2, 3);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           4, 6, 3, 5);
    expected_operations = append_operation(expected_operations, INSERT,
                                           6, 6, 5, 6);
    cut_assert_operations_string(expected_operations, qabxcd, abycdf);
    free_operations(expected_operations);

    expected_operations = append_operation(NULL, EQUAL, 0, 23, 0, 23);
    expected_operations = append_operation(expected_operations, REPLACE,
                                           23, 24, 23, 24);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           24, 35, 24, 35);
    expected_operations = append_operation(expected_operations, REPLACE,
                                           35, 36, 35, 36);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           36, 45, 36, 45);
    cut_assert_operations_string(expected_operations,
                                 summary_1010, summary_1001);
}

#define cut_assert_operations_char(expected_operations, from, to)       \
    cut_assert_operations(expected_operations,                          \
                          cut_sequence_matcher_char_new(from, to),      \
                          inspect_char_matcher(from, to))

void
test_get_operations_for_char_sequence (void)
{
    cut_assert_operations_char(NULL, "", "");

    expected_operations = append_operation(NULL, DELETE, 0, 1, 0, 0);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           1, 3, 0, 2);
    expected_operations = append_operation(expected_operations, REPLACE,
                                           3, 4, 2, 3);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           4, 6, 3, 5);
    expected_operations = append_operation(expected_operations, INSERT,
                                           6, 6, 5, 6);
    cut_assert_operations_char(expected_operations, "qabxcd", "abycdf");
    free_operations(expected_operations);

    expected_operations = append_operation(NULL, EQUAL, 0, 23, 0, 23);
    expected_operations = append_operation(expected_operations, REPLACE,
                                           23, 24, 23, 24);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           24, 35, 24, 35);
    expected_operations = append_operation(expected_operations, REPLACE,
                                           35, 36, 35, 36);
    expected_operations = append_operation(expected_operations, EQUAL,
                                           36, 45, 36, 45);
    cut_assert_operations_char(expected_operations,
                               "1 tests, 0 assertions, 1 failures, 0 pendings",
                               "1 tests, 0 assertions, 0 failures, 1 pendings");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
