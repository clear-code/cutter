/* -*- c-file-style: "gnu" -*- */
/*
 * Copyright (C) 2005 - 2006, Marco Barisione <marco@barisione.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <cutter.h>
#include <cut-utils.h>

#include <string.h>
#include <locale.h>
#include <glib.h>

/* U+20AC EURO SIGN (symbol, currency) */
#define EURO "\xe2\x82\xac"
/* U+00E0 LATIN SMALL LETTER A WITH GRAVE (letter, lowercase) */
#define AGRAVE "\xc3\xa0"
/* U+00C0 LATIN CAPITAL LETTER A WITH GRAVE (letter, uppercase) */
#define AGRAVE_UPPER "\xc3\x80"
/* U+00E8 LATIN SMALL LETTER E WITH GRAVE (letter, lowercase) */
#define EGRAVE "\xc3\xa8"
/* U+00F2 LATIN SMALL LETTER O WITH GRAVE (letter, lowercase) */
#define OGRAVE "\xc3\xb2"
/* U+014B LATIN SMALL LETTER ENG (letter, lowercase) */
#define ENG "\xc5\x8b"
/* U+0127 LATIN SMALL LETTER H WITH STROKE (letter, lowercase) */
#define HSTROKE "\xc4\xa7"
/* U+0634 ARABIC LETTER SHEEN (letter, other) */
#define SHEEN "\xd8\xb4"
/* U+1374 ETHIOPIC NUMBER THIRTY (number, other) */
#define ETH30 "\xe1\x8d\xb4"

/* A random value use to mark untouched integer variables. */
#define UNTOUCHED -559038737

void test_regex_new (void);
void test_regex_new_fail (void);
void test_match_simple (void);
void test_match (void);
void test_mismatch (void);
void test_match_count (void);
void test_get_string_number (void);
void test_escape (void);
void test_check_replacement (void);
void test_replace (void);
void test_partial_match (void);
void test_replace_lit (void);
void test_expand (void);
void test_match_next (void);
void test_sub_pattern (void);
void test_named_sub_pattern (void);
void test_fetch_all (void);
void test_split_simple (void);
void test_split (void);

static GRegex *regex;
static GMatchInfo *match_info;

void
setup (void)
{
  regex = NULL;
  match_info = NULL;
}

void
teardown (void)
{
  if (match_info)
    g_match_info_free (match_info);
  if (regex)
    g_regex_unref (regex);
  match_info = NULL;
  regex = NULL;
}


#define PASS passed++
#define FAIL \
  G_STMT_START \
    { \
      failed++; \
      if (abort_on_fail) \
	goto end; \
    } \
  G_STMT_END

/* A replacement for strcmp that doesn't crash with null pointers. */
static gboolean
streq (const gchar *s1, const gchar *s2)
{
  if (s1 == NULL && s2 == NULL)
    return TRUE;
  else if (s1 == NULL)
    return FALSE;
  else if (s2 == NULL)
    return FALSE;
  else
    return strcmp (s1, s2) == 0;
}

#define TEST_REGEX_NEW(pattern, compile_opts, match_opts) \
  regex = g_regex_new (pattern, compile_opts, match_opts, NULL); \
  cut_assert (regex, "failed (pattern: \"%s\", compile: %d, match %d)", \
	      pattern, compile_opts, match_opts); \
  cut_assert (streq (g_regex_get_pattern (regex), pattern), \
              "failed (pattern: \"%s\")", pattern); \
  g_regex_unref (regex); \
  regex = NULL;

void
test_regex_new (void)
{
  TEST_REGEX_NEW ("", 0, 0);
  TEST_REGEX_NEW ("", 0, 0);
  TEST_REGEX_NEW (".*", 0, 0);
  TEST_REGEX_NEW (".*", G_REGEX_OPTIMIZE, 0);
  TEST_REGEX_NEW (".*", G_REGEX_MULTILINE, 0);
  TEST_REGEX_NEW (".*", G_REGEX_DOTALL, 0);
  TEST_REGEX_NEW (".*", G_REGEX_DOTALL, G_REGEX_MATCH_NOTBOL);
  TEST_REGEX_NEW ("(123\\d*)[a-zA-Z]+(?P<hello>.*)", 0, 0);
  TEST_REGEX_NEW ("(123\\d*)[a-zA-Z]+(?P<hello>.*)", G_REGEX_CASELESS, 0);
  TEST_REGEX_NEW ("(123\\d*)[a-zA-Z]+(?P<hello>.*)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0);
  TEST_REGEX_NEW ("(?P<A>x)|(?P<A>y)", G_REGEX_DUPNAMES, 0);
  TEST_REGEX_NEW ("(?P<A>x)|(?P<A>y)", G_REGEX_DUPNAMES | G_REGEX_OPTIMIZE, 0);
  /* This gives "internal error: code overflow" with pcre 6.0 */
  TEST_REGEX_NEW ("(?i)(?-i)", 0, 0);
}

#define TEST_REGEX_NEW_FAIL(pattern, compile_opts) \
  regex = g_regex_new (pattern, compile_opts, 0, NULL); \
  cut_assert_null (regex, "failed (pattern: \"%s\", compile: %d)", \
	           pattern, compile_opts);

void
test_regex_new_fail (void)
{
  TEST_REGEX_NEW_FAIL ("(", 0);
  TEST_REGEX_NEW_FAIL (")", 0);
  TEST_REGEX_NEW_FAIL ("[", 0);
  TEST_REGEX_NEW_FAIL ("*", 0);
  TEST_REGEX_NEW_FAIL ("?", 0);
  TEST_REGEX_NEW_FAIL ("(?P<A>x)|(?P<A>y)", 0);
}

void
test_match_simple (void)
{
  cut_assert (!g_regex_match_simple ("a", "", 0, 0));
  cut_assert (g_regex_match_simple ("a", "a", 0, 0));
  cut_assert (g_regex_match_simple ("a", "ba", 0, 0));
  cut_assert (!g_regex_match_simple ("^a", "ba", 0, 0));
  cut_assert (!g_regex_match_simple ("a", "ba", G_REGEX_ANCHORED, 0));
  cut_assert (!g_regex_match_simple ("a", "ba", 0, G_REGEX_MATCH_ANCHORED));
  cut_assert (g_regex_match_simple ("a", "ab", G_REGEX_ANCHORED, 0));
  cut_assert (g_regex_match_simple ("a", "ab", 0, G_REGEX_MATCH_ANCHORED));
  cut_assert (g_regex_match_simple ("a", "a", G_REGEX_CASELESS, 0));
  cut_assert (g_regex_match_simple ("a", "A", G_REGEX_CASELESS, 0));
  /* These are needed to test extended properties. */
  cut_assert (g_regex_match_simple (AGRAVE, AGRAVE, G_REGEX_CASELESS, 0));
  cut_assert (g_regex_match_simple (AGRAVE, AGRAVE_UPPER, G_REGEX_CASELESS, 0));
  cut_assert (g_regex_match_simple ("\\p{L}", "a", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{L}", "1", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{L}", AGRAVE, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{L}", AGRAVE_UPPER, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{L}", SHEEN, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{L}", ETH30, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Ll}", "a", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Ll}", AGRAVE, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ll}", AGRAVE_UPPER, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ll}", ETH30, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Sc}", AGRAVE, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Sc}", EURO, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Sc}", ETH30, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{N}", "a", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{N}", "1", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{N}", AGRAVE, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{N}", AGRAVE_UPPER, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{N}", SHEEN, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{N}", ETH30, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Nd}", "a", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Nd}", "1", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Nd}", AGRAVE, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Nd}", AGRAVE_UPPER, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Nd}", SHEEN, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Nd}", ETH30, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Common}", SHEEN, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Common}", "a", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Common}", AGRAVE, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Common}", AGRAVE_UPPER, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Common}", ETH30, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Common}", "%", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Common}", "1", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Arabic}", SHEEN, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Arabic}", "a", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Arabic}", AGRAVE, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Arabic}", AGRAVE_UPPER, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Arabic}", ETH30, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Arabic}", "%", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Arabic}", "1", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Latin}", SHEEN, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Latin}", "a", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Latin}", AGRAVE, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Latin}", AGRAVE_UPPER, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Latin}", ETH30, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Latin}", "%", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Latin}", "1", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ethiopic}", SHEEN, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ethiopic}", "a", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ethiopic}", AGRAVE, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ethiopic}", AGRAVE_UPPER, 0, 0));
  cut_assert (g_regex_match_simple ("\\p{Ethiopic}", ETH30, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ethiopic}", "%", 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{Ethiopic}", "1", 0, 0));
  cut_assert (g_regex_match_simple ("\\p{L}(?<=\\p{Arabic})", SHEEN, 0, 0));
  cut_assert (!g_regex_match_simple ("\\p{L}(?<=\\p{Latin})", SHEEN, 0, 0));
  /* Invalid patterns. */
  cut_assert (!g_regex_match_simple ("\\", "a", 0, 0));
  cut_assert (!g_regex_match_simple ("[", "", 0, 0));
}

static void
cut_assert_match (const gchar *pattern, GRegexCompileFlags compile_options,
                  GRegexMatchFlags pattern_match_options, const gchar *string,
                  gssize length, gint start_position,
                  GRegexMatchFlags match_options)
{
  regex = g_regex_new (pattern, compile_options, pattern_match_options, NULL);
  cut_assert (regex,
              "failed (pattern: \"%s\", compile: %d, match %d)",
	      pattern, compile_options, pattern_match_options);
  cut_assert (g_regex_match_full (regex, string, length, start_position,
                                  match_options, NULL, NULL));
  if (length == -1 && start_position == 0)
    {
      cut_assert (g_regex_match (regex, string, match_options, NULL),
	          "failed (pattern: \"%s\", string: \"%s\")",
                  pattern, string);
    }
  g_regex_unref (regex);
  regex = NULL;
}

void
test_match (void)
{
  cut_assert_match ("a", 0, 0, "a", -1, 0, 0);
  cut_assert_match ("a", G_REGEX_CASELESS, 0, "A", -1, 0, 0);
  cut_assert_match ("a", 0, 0, "bab", -1, 0, 0);
  cut_assert_match ("a", 0, G_REGEX_ANCHORED, "a", -1, 0, 0);
  cut_assert_match ("a|b", 0, 0, "a", -1, 0, 0);
  cut_assert_match ("^.$", 0, 0, EURO, -1, 0, 0);
  cut_assert_match ("^.{3}$", G_REGEX_RAW, 0, EURO, -1, 0, 0);
  cut_assert_match (AGRAVE, G_REGEX_CASELESS, 0, AGRAVE_UPPER, -1, 0, 0);
  cut_assert_match ("a", 0, 0, "a", -1, 0, G_REGEX_ANCHORED);

  /* New lines handling. */
  cut_assert_match ("^a\\Rb$", 0, 0, "a\r\nb", -1, 0, 0);
  cut_assert_match ("^a\\Rb$", 0, 0, "a\nb", -1, 0, 0);
  cut_assert_match ("^a\\Rb$", 0, 0, "a\rb", -1, 0, 0);
  cut_assert_match ("^a\\R\\Rb$", 0, 0, "a\n\rb", -1, 0, 0);
  cut_assert_match ("^a\\r\\nb$", 0, 0, "a\r\nb", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE, 0, "a\nb\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE, 0, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE, 0, "a\rb\rc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_LF, 0, "a\nb\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CRLF, 0, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, "a\rb\rc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_LF, "a\nb\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CRLF, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CR, "a\rb\rc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_ANY, "a\nb\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_ANY, "a\rb\rc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_ANY, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_LF, "a\nb\nc", -1, 0, 0);
  cut_assert_match ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_CRLF, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match ("a#\nb", G_REGEX_EXTENDED | G_REGEX_NEWLINE_CR, 0, "a", -1, 0, 0);
}

#define TEST_MISMATCH(pattern, compile_opts, match_opts, string, \
		   string_len, start_position, match_opts2) { \
  regex = g_regex_new (pattern, compile_opts, match_opts, NULL); \
  cut_assert (regex, "failed (pattern: \"%s\", compile: %d, match %d)", \
	      pattern, compile_opts, match_opts); \
  cut_assert (!g_regex_match_full (regex, string, string_len, \
              start_position, match_opts2, NULL, NULL)); \
  if (string_len == -1 && start_position == 0) \
    { \
      cut_assert (!g_regex_match (regex, string, match_opts2, NULL), \
	          "failed (pattern: \"%s\", string: \"%s\")", \
		   pattern, string); \
    } \
  g_regex_unref (regex); \
  regex = NULL; \
}
void
test_mismatch (void)
{
  TEST_MISMATCH("a", 0, 0, "A", -1, 0, 0);
  TEST_MISMATCH("a", 0, 0, "ab", -1, 1, 0);
  TEST_MISMATCH("a", 0, 0, "ba", 1, 0, 0);
  TEST_MISMATCH("a", 0, 0, "b", -1, 0, 0);
  TEST_MISMATCH("a", 0, G_REGEX_ANCHORED, "ab", -1, 1, 0);
  TEST_MISMATCH("a", 0, G_REGEX_ANCHORED, "ba", 1, 0, 0);
  TEST_MISMATCH("a", 0, G_REGEX_ANCHORED, "bab", -1, 0, 0);
  TEST_MISMATCH("a", 0, G_REGEX_ANCHORED, "b", -1, 0, 0);
  TEST_MISMATCH("a", 0, 0, "ab", -1, 1, G_REGEX_ANCHORED);
  TEST_MISMATCH("a", 0, 0, "ba", 1, 0, G_REGEX_ANCHORED);
  TEST_MISMATCH("a", 0, 0, "bab", -1, 0, G_REGEX_ANCHORED);
  TEST_MISMATCH("a", 0, 0, "b", -1, 0, G_REGEX_ANCHORED);
  TEST_MISMATCH("\\d", 0, 0, EURO, -1, 0, 0);
  TEST_MISMATCH("^.{3}$", 0, 0, EURO, -1, 0, 0);
  TEST_MISMATCH("^.$", G_REGEX_RAW, 0, EURO, -1, 0, 0);

  /* New lines handling. */
  TEST_MISMATCH("^a\\Rb$", 0, 0, "a\n\rb", -1, 0, 0);
  TEST_MISMATCH("^a\\nb$", 0, 0, "a\r\nb", -1, 0, 0);

  TEST_MISMATCH("^b$", 0, 0, "a\nb\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, "a\nb\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CRLF, 0, "a\nb\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, "a\r\nb\r\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_LF, 0, "a\r\nb\r\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_LF, 0, "a\rb\rc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CRLF, 0, "a\rb\rc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CR, "a\nb\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CRLF, "a\nb\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CR, "a\r\nb\r\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_LF, "a\r\nb\r\nc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_LF, "a\rb\rc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CRLF, "a\rb\rc", -1, 0, 0);

  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_LF, "a\rb\rc", -1, 0, 0);
  TEST_MISMATCH("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_CRLF, "a\rb\rc", -1, 0, 0);

  TEST_MISMATCH("a#\nb", G_REGEX_EXTENDED, 0, "a", -1, 0, 0);
  TEST_MISMATCH("a#\r\nb", G_REGEX_EXTENDED, 0, "a", -1, 0, 0);
  TEST_MISMATCH("a#\rb", G_REGEX_EXTENDED, 0, "a", -1, 0, 0);
  TEST_MISMATCH("a#\nb", G_REGEX_EXTENDED, G_REGEX_MATCH_NEWLINE_CR, "a", -1, 0, 0);
}
static void
cut_assert_match_count (const gchar     *pattern,
			const gchar     *string,
			gint             start_position,
                  	GRegexMatchFlags match_opts,
			gint             expected_count)
{
  regex = g_regex_new (pattern, 0, 0, NULL);
  g_regex_match_full (regex, string, -1, start_position,
		      match_opts, &match_info, NULL);
  cut_assert_equal_int (expected_count, g_match_info_get_match_count (match_info));
  g_match_info_free (match_info);
  g_regex_unref (regex);
  match_info = NULL;
  regex = NULL;
}

void
test_match_count (void)
{
  /* TEST_MATCH_COUNT(pattern, string, start_position, match_opts, expected_count) */
  cut_assert_match_count ("a", "", 0, 0, 0);
  cut_assert_match_count ("a", "a", 0, 0, 1);
  cut_assert_match_count ("a", "a", 1, 0, 0);
  cut_assert_match_count ("(.)", "a", 0, 0, 2);
  cut_assert_match_count ("(.)", EURO, 0, 0, 2);
  cut_assert_match_count ("(?:.)", "a", 0, 0, 1);
  cut_assert_match_count ("(?P<A>.)", "a", 0, 0, 2);
  cut_assert_match_count ("a$", "a", 0, G_REGEX_MATCH_NOTEOL, 0);
  cut_assert_match_count ("(a)?(b)", "b", 0, 0, 3);
  cut_assert_match_count ("(a)?(b)", "ab", 0, 0, 3);
}

static void
cut_assert_get_string_number (const gchar *pattern,
			      const gchar *name,
			      gint         expected_num)
{
  regex = g_regex_new (pattern, 0, 0, NULL);
  cut_assert_equal_int (expected_num, g_regex_get_string_number (regex, name));
  g_regex_unref (regex);
  regex = NULL;
}

void
test_get_string_number (void)
{
  cut_assert_get_string_number ("", "A", -1);
  cut_assert_get_string_number ("(?P<A>.)", "A", 1);
  cut_assert_get_string_number ("(?P<A>.)", "B", -1);
  cut_assert_get_string_number ("(?P<A>.)(?P<B>a)", "A", 1);
  cut_assert_get_string_number ("(?P<A>.)(?P<B>a)", "B", 2);
  cut_assert_get_string_number ("(?P<A>.)(?P<B>a)", "C", -1);
  cut_assert_get_string_number ("(?P<A>.)(.)(?P<B>a)", "A", 1);
  cut_assert_get_string_number ("(?P<A>.)(.)(?P<B>a)", "B", 3);
  cut_assert_get_string_number ("(?P<A>.)(.)(?P<B>a)", "C", -1);
  cut_assert_get_string_number ("(?:a)(?P<A>.)", "A", 1);
  cut_assert_get_string_number ("(?:a)(?P<A>.)", "B", -1);
}

static void
cut_assert_escape (const gchar *string,
		   gint         length,
		   const gchar *expected)
{
  gchar *escaped;
  escaped = g_regex_escape_string (string, length);
  cut_assert_equal_string (expected, escaped);
  g_free (escaped);
}

void
test_escape (void)
{
  cut_assert_escape ("hello world", -1, "hello world");
  cut_assert_escape ("hello world", 5, "hello");
  cut_assert_escape ("hello.world", -1, "hello\\.world");
  cut_assert_escape ("a(b\\b.$", -1, "a\\(b\\\\b\\.\\$");
  cut_assert_escape ("hello\0world", -1, "hello");
  cut_assert_escape ("hello\0world", 11, "hello\\0world");
  cut_assert_escape (EURO "*" ENG, -1, EURO "\\*" ENG);
  cut_assert_escape ("a$", -1, "a\\$");
  cut_assert_escape ("$a", -1, "\\$a");
  cut_assert_escape ("a$a", -1, "a\\$a");
  cut_assert_escape ("$a$", -1, "\\$a\\$");
  cut_assert_escape ("$a$", 0, "");
  cut_assert_escape ("$a$", 1, "\\$");
  cut_assert_escape ("$a$", 2, "\\$a");
  cut_assert_escape ("$a$", 3, "\\$a\\$");
  cut_assert_escape ("$a$", 4, "\\$a\\$\\0");
  cut_assert_escape ("|()[]{}^$*+?.", -1, "\\|\\(\\)\\[\\]\\{\\}\\^\\$\\*\\+\\?\\.");
  cut_assert_escape ("a|a(a)a[a]a{a}a^a$a*a+a?a.a", -1,
	      	     "a\\|a\\(a\\)a\\[a\\]a\\{a\\}a\\^a\\$a\\*a\\+a\\?a\\.a");
}

#define TEST_REPLACE(pattern, string, start_position, replacement, expected) { \
  gchar *res; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  res = g_regex_replace (regex, string, -1, start_position, replacement, 0, NULL); \
  cut_assert_equal_string (expected, res); \
  g_free (res); \
  g_regex_unref (regex); \
  regex = NULL; \
}

#define TEST_REPLACE_NULL(pattern, string, start_position, replacement) { \
  gchar *res; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  res = g_regex_replace (regex, string, -1, start_position, replacement, 0, NULL); \
  cut_assert_null (res); \
  g_regex_unref (regex); \
  regex = NULL; \
}

void
test_replace (void)
{
  /* TEST_REPLACE(pattern, string, start_position, replacement, expected) */
  TEST_REPLACE("a", "ababa", 0, "A", "AbAbA");
  TEST_REPLACE("a", "ababa", 1, "A", "abAbA");
  TEST_REPLACE("a", "ababa", 2, "A", "abAbA");
  TEST_REPLACE("a", "ababa", 3, "A", "ababA");
  TEST_REPLACE("a", "ababa", 4, "A", "ababA");
  TEST_REPLACE("a", "ababa", 5, "A", "ababa");
  TEST_REPLACE("a", "ababa", 6, "A", "ababa");
  TEST_REPLACE("a", "abababa", 2, "A", "abAbAbA");
  TEST_REPLACE("a", "abab", 0, "A", "AbAb");
  TEST_REPLACE("a", "baba", 0, "A", "bAbA");
  TEST_REPLACE("a", "bab", 0, "A", "bAb");
  TEST_REPLACE("$^", "abc", 0, "X", "abc");
  TEST_REPLACE("(.)a", "ciao", 0, "a\\1", "caio");
  TEST_REPLACE("a.", "abc", 0, "\\0\\0", "ababc");
  TEST_REPLACE("a", "asd", 0, "\\0101", "Asd");
  TEST_REPLACE("(a).\\1", "aba cda", 0, "\\1\\n", "a\n cda");
  TEST_REPLACE("a" AGRAVE "a", "a" AGRAVE "a", 0, "x", "x");
  TEST_REPLACE("a" AGRAVE "a", "a" AGRAVE "a", 0, OGRAVE, OGRAVE);
  TEST_REPLACE("[^-]", "-" EURO "-x-" HSTROKE, 0, "a", "-a-a-a");
  TEST_REPLACE("[^-]", "-" EURO "-" HSTROKE, 0, "a\\g<0>a", "-a" EURO "a-a" HSTROKE "a");
  TEST_REPLACE("-", "-" EURO "-" HSTROKE, 0, "", EURO HSTROKE);
  TEST_REPLACE(".*", "hello", 0, "\\U\\0\\E", "HELLO");
  TEST_REPLACE(".*", "hello", 0, "\\u\\0", "Hello");
  TEST_REPLACE("\\S+", "hello world", 0, "\\U-\\0-", "-HELLO- -WORLD-");
  TEST_REPLACE_NULL(".", "a", 0, "\\A");
  TEST_REPLACE_NULL(".", "a", 0, "\\g");
}

#define TEST_PARTIAL_MATCH(pattern, string) { \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match (regex, string, G_REGEX_MATCH_PARTIAL, &match_info); \
  cut_assert (g_match_info_is_partial_match (match_info)); \
  cut_assert (!g_match_info_fetch_pos (match_info, 0, NULL, NULL)); \
  cut_assert (!g_match_info_fetch_pos (match_info, 1, NULL, NULL)); \
  g_match_info_free (match_info); \
  g_regex_unref (regex); \
  regex = NULL; \
  match_info = NULL; \
}

#define TEST_PARTIAL_MISMATCH(pattern, string) { \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match (regex, string, G_REGEX_MATCH_PARTIAL, &match_info); \
  cut_assert (!g_match_info_is_partial_match (match_info)); \
  g_match_info_free (match_info); \
  g_regex_unref (regex); \
  regex = NULL; \
  match_info = NULL; \
}

void
test_partial_match (void)
{
  /* TEST_PARTIAL(pattern, string, expected) */
  TEST_PARTIAL_MATCH("^ab", "a");
  TEST_PARTIAL_MATCH("ab", "xa");
  TEST_PARTIAL_MATCH("(a)+b", "aa");
  TEST_PARTIAL_MATCH("a?b", "a");
  TEST_PARTIAL_MISMATCH("^ab", "xa");
  TEST_PARTIAL_MISMATCH("ab", "ab"); /* normal match. */
  TEST_PARTIAL_MISMATCH("a+b", "aa"); /* PCRE_ERROR_BAD_PARTIAL */
}

#define TEST_CHECK_REPLACEMENT(string_to_expand, expected_refs) \
  cut_assert (g_regex_check_replacement (string_to_expand, &has_refs, NULL)); \
  cut_assert (expected_refs == has_refs, \
              "failed (got has_references \"%s\", expected \"%s\")", \
	       has_refs ? "TRUE" : "FALSE", \
	       expected_refs ? "TRUE" : "FALSE"); \

void
test_check_replacement (void)
{
  gboolean has_refs;
  /* TEST_CHECK_REPLACEMENT(string_to_expand, expected, expected_refs) */
  TEST_CHECK_REPLACEMENT("", FALSE);
  TEST_CHECK_REPLACEMENT("a", FALSE);
  TEST_CHECK_REPLACEMENT("\\t\\n\\v\\r\\f\\a\\b\\\\\\x{61}", FALSE);
  TEST_CHECK_REPLACEMENT("\\0", TRUE);
  TEST_CHECK_REPLACEMENT("\\n\\2", TRUE);
  TEST_CHECK_REPLACEMENT("\\g<foo>", TRUE);

  /* Invalid strings */
  cut_assert (!g_regex_check_replacement ("\\Q", NULL, NULL));
  cut_assert (!g_regex_check_replacement ("x\\Ay", NULL, NULL));
}

#define TEST_REPLACE_LIT(pattern, string, start_position, replacement, expected) { \
  gchar *res; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  res = g_regex_replace_literal (regex, string, -1, start_position, \
				 replacement, 0, NULL); \
  cut_assert_equal_string (expected, res); \
  g_free (res); \
  g_regex_unref (regex); \
  regex = NULL; \
}

void
test_replace_lit (void)
{
  /* TEST_REPLACE_LIT(pattern, string, start_position, replacement, expected) */
  TEST_REPLACE_LIT("a", "ababa", 0, "A", "AbAbA");
  TEST_REPLACE_LIT("a", "ababa", 1, "A", "abAbA");
  TEST_REPLACE_LIT("a", "ababa", 2, "A", "abAbA");
  TEST_REPLACE_LIT("a", "ababa", 3, "A", "ababA");
  TEST_REPLACE_LIT("a", "ababa", 4, "A", "ababA");
  TEST_REPLACE_LIT("a", "ababa", 5, "A", "ababa");
  TEST_REPLACE_LIT("a", "ababa", 6, "A", "ababa");
  TEST_REPLACE_LIT("a", "abababa", 2, "A", "abAbAbA");
  TEST_REPLACE_LIT("a", "abcadaa", 0, "A", "AbcAdAA");
  TEST_REPLACE_LIT("$^", "abc", 0, "X", "abc");
  TEST_REPLACE_LIT("(.)a", "ciao", 0, "a\\1", "ca\\1o");
  TEST_REPLACE_LIT("a.", "abc", 0, "\\0\\0\\n", "\\0\\0\\nc");
  TEST_REPLACE_LIT("a" AGRAVE "a", "a" AGRAVE "a", 0, "x", "x");
  TEST_REPLACE_LIT("a" AGRAVE "a", "a" AGRAVE "a", 0, OGRAVE, OGRAVE);
  TEST_REPLACE_LIT(AGRAVE, "-" AGRAVE "-" HSTROKE, 0, "a" ENG "a", "-a" ENG "a-" HSTROKE);
  TEST_REPLACE_LIT("[^-]", "-" EURO "-" AGRAVE "-" HSTROKE, 0, "a", "-a-a-a");
  TEST_REPLACE_LIT("[^-]", "-" EURO "-" AGRAVE, 0, "a\\g<0>a", "-a\\g<0>a-a\\g<0>a");
  TEST_REPLACE_LIT("-", "-" EURO "-" AGRAVE "-" HSTROKE, 0, "", EURO AGRAVE HSTROKE);
}

#define TEST_EXPAND(pattern, string, string_to_expand, raw, expected) { \
  gchar *res; \
  regex = g_regex_new (pattern, raw ? G_REGEX_RAW : 0, 0, NULL); \
  g_regex_match (regex, string, 0, &match_info); \
  res = g_match_info_expand_references (match_info, string_to_expand, NULL); \
  if (!expected) \
    cut_assert_null (res); \
  else \
    cut_assert_equal_string (expected, res); \
  if (res) \
    g_free (res); \
  if (match_info) \
    g_match_info_free (match_info); \
  if (regex) \
    g_regex_unref (regex); \
  match_info = NULL; \
  regex = NULL; \
}

#define TEST_EXPAND_RETURN_NULL(string_to_expand) \
  cut_assert_null (g_match_info_expand_references (NULL, string_to_expand, NULL));

#define TEST_EXPAND_NULL(string_to_expand, expected) {\
  gchar *res; \
  res = g_match_info_expand_references (NULL, string_to_expand, NULL); \
  cut_assert_equal_string (expected, res); \
  if (res) \
    g_free (res); \
}

void
test_expand (void)
{
  /* TEST_EXPAND(pattern, string, string_to_expand, raw, expected) */
  TEST_EXPAND("a", "a", "", FALSE, "");
  TEST_EXPAND("a", "a", "\\0", FALSE, "a");
  TEST_EXPAND("a", "a", "\\1", FALSE, "");
  TEST_EXPAND("(a)", "ab", "\\1", FALSE, "a");
  TEST_EXPAND("(a)", "a", "\\1", FALSE, "a");
  TEST_EXPAND("(a)", "a", "\\g<1>", FALSE, "a");
  TEST_EXPAND("a", "a", "\\0130", FALSE, "X");
  TEST_EXPAND("a", "a", "\\\\\\0", FALSE, "\\a");
  TEST_EXPAND("a(?P<G>.)c", "xabcy", "X\\g<G>X", FALSE, "XbX");
  TEST_EXPAND("(.)(?P<1>.)", "ab", "\\1", FALSE, "a");
  TEST_EXPAND("(.)(?P<1>.)", "ab", "\\g<1>", FALSE, "a");
  TEST_EXPAND(".", EURO, "\\0", FALSE, EURO);
  TEST_EXPAND("(.)", EURO, "\\1", FALSE, EURO);
  TEST_EXPAND("(?P<G>.)", EURO, "\\g<G>", FALSE, EURO);
  TEST_EXPAND(".", "a", EURO, FALSE, EURO);
  TEST_EXPAND(".", "a", EURO "\\0", FALSE, EURO "a");
  TEST_EXPAND(".", "", "\\Lab\\Ec", FALSE, "abc");
  TEST_EXPAND(".", "", "\\LaB\\EC", FALSE, "abC");
  TEST_EXPAND(".", "", "\\Uab\\Ec", FALSE, "ABc");
  TEST_EXPAND(".", "", "a\\ubc", FALSE, "aBc");
  TEST_EXPAND(".", "", "a\\lbc", FALSE, "abc");
  TEST_EXPAND(".", "", "A\\uBC", FALSE, "ABC");
  TEST_EXPAND(".", "", "A\\lBC", FALSE, "AbC");
  TEST_EXPAND(".", "", "A\\l\\\\BC", FALSE, "A\\BC");
  TEST_EXPAND(".", "", "\\L" AGRAVE "\\E", FALSE, AGRAVE);
  TEST_EXPAND(".", "", "\\U" AGRAVE "\\E", FALSE, AGRAVE_UPPER);
  TEST_EXPAND(".", "", "\\u" AGRAVE "a", FALSE, AGRAVE_UPPER "a");
  TEST_EXPAND(".", "ab", "x\\U\\0y\\Ez", FALSE, "xAYz");
  TEST_EXPAND(".(.)", "AB", "x\\L\\1y\\Ez", FALSE, "xbyz");
  TEST_EXPAND(".", "ab", "x\\u\\0y\\Ez", FALSE, "xAyz");
  TEST_EXPAND(".(.)", "AB", "x\\l\\1y\\Ez", FALSE, "xbyz");
  TEST_EXPAND(".(.)", "a" AGRAVE_UPPER, "x\\l\\1y", FALSE, "x" AGRAVE "y");
  TEST_EXPAND("a", "bab", "\\x{61}", FALSE, "a");
  TEST_EXPAND("a", "bab", "\\x61", FALSE, "a");
  TEST_EXPAND("a", "bab", "\\x5a", FALSE, "Z");
  TEST_EXPAND("a", "bab", "\\0\\x5A", FALSE, "aZ");
  TEST_EXPAND("a", "bab", "\\1\\x{5A}", FALSE, "Z");
  TEST_EXPAND("a", "bab", "\\x{00E0}", FALSE, AGRAVE);
  TEST_EXPAND("", "bab", "\\x{0634}", FALSE, SHEEN);
  TEST_EXPAND("", "bab", "\\x{634}", FALSE, SHEEN);
  TEST_EXPAND("", "", "\\t", FALSE, "\t");
  TEST_EXPAND("", "", "\\v", FALSE, "\v");
  TEST_EXPAND("", "", "\\r", FALSE, "\r");
  TEST_EXPAND("", "", "\\n", FALSE, "\n");
  TEST_EXPAND("", "", "\\f", FALSE, "\f");
  TEST_EXPAND("", "", "\\a", FALSE, "\a");
  TEST_EXPAND("", "", "\\b", FALSE, "\b");
  TEST_EXPAND("a(.)", "abc", "\\0\\b\\1", FALSE, "ab\bb");
  TEST_EXPAND("a(.)", "abc", "\\0141", FALSE, "a");
  TEST_EXPAND("a(.)", "abc", "\\078", FALSE, "\a8");
  TEST_EXPAND("a(.)", "abc", "\\077", FALSE, "?");
  TEST_EXPAND("a(.)", "abc", "\\0778", FALSE, "?8");
  TEST_EXPAND("a(.)", "a" AGRAVE "b", "\\1", FALSE, AGRAVE);
  TEST_EXPAND("a(.)", "a" AGRAVE "b", "\\1", TRUE, "\xc3");
  TEST_EXPAND("a(.)", "a" AGRAVE "b", "\\0", TRUE, "a\xc3");

  /* Invalid strings. */
  TEST_EXPAND("", "", "\\Q", FALSE, NULL);
  TEST_EXPAND("", "", "x\\Ay", FALSE, NULL);
  TEST_EXPAND("", "", "\\g<", FALSE, NULL);
  TEST_EXPAND("", "", "\\g<>", FALSE, NULL);
  TEST_EXPAND("", "", "\\g<1a>", FALSE, NULL);
  TEST_EXPAND("", "", "\\g<a$>", FALSE, NULL);
  TEST_EXPAND("", "", "\\", FALSE, NULL);
  TEST_EXPAND("a", "a", "\\x{61", FALSE, NULL);
  TEST_EXPAND("a", "a", "\\x6X", FALSE, NULL);
  /* Pattern-less. */
  TEST_EXPAND_NULL("", "");
  TEST_EXPAND_NULL("\\n", "\n");
  /* Invalid strings */
  TEST_EXPAND_RETURN_NULL("\\Q");
  TEST_EXPAND_RETURN_NULL("x\\Ay");
}

struct _Match
{
  gchar *string;
  gint start, end;
};
typedef struct _Match Match;

static void
free_match (gpointer data, gpointer user_data)
{
  Match *match = data;
  if (match == NULL)
    return;
  g_free (match->string);
  g_free (match);
}

static GSList *
collect_expected_matches (gint start_position, ...)
{
  va_list args;
  GSList *expected = NULL;

  /* The va_list is a NULL-terminated sequence of: expected matched string,
   * expected start and expected end. */
  va_start (args, start_position);
  while (TRUE)
   {
      Match *match;
      const gchar *expected_string = va_arg (args, const gchar *);
      if (expected_string == NULL)
        break;
      match = g_new0 (Match, 1);
      match->string = g_strdup (expected_string);
      match->start = va_arg (args, gint);
      match->end = va_arg (args, gint);
      expected = g_slist_prepend (expected, match);
    }
  expected = g_slist_reverse (expected);
  va_end (args);

  return expected;
}

static GSList *
collect_actual_matches (GRegex **regex, const gchar *pattern,
                        const gchar *string, gssize string_len,
                        gint start_position, GMatchInfo **match_info)
{
  GSList *actual = NULL;

  *regex = g_regex_new (pattern, 0, 0, NULL);
  g_regex_match_full (*regex, string, string_len,
		      start_position, 0, match_info, NULL);

  while (g_match_info_matches (*match_info))
    {
      Match *match = g_new0 (Match, 1);
      match->string = g_match_info_fetch (*match_info, 0);
      match->start = UNTOUCHED;
      match->end = UNTOUCHED;
      g_match_info_fetch_pos (*match_info, 0, &match->start, &match->end);
      actual = g_slist_prepend (actual, match);
      g_match_info_next (*match_info, NULL);
    }

  return actual;
}

#define TEST_MATCH_NEXT(pattern, _string, string_len, start_position, ...) do \
{                                                                       \
  GRegex *regex;                                                        \
  GMatchInfo *match_info;                                               \
  GSList *actual = NULL, *expected = NULL;                              \
  GSList *actual_node, *expected_node;                                  \
                                                                        \
  expected = collect_expected_matches (start_position, ## __VA_ARGS__); \
  actual = collect_actual_matches (&regex, pattern,                     \
                                   _string, string_len,                 \
                                   start_position, &match_info);        \
  cut_assert (regex == g_match_info_get_regex (match_info));            \
  cut_assert_equal_string (_string,                                     \
                           g_match_info_get_string (match_info));       \
  g_match_info_free (match_info);                                       \
  actual = g_slist_reverse (actual);                                    \
                                                                        \
  cut_assert_equal_int(g_slist_length (expected),                       \
                       g_slist_length (actual));                        \
                                                                        \
  expected_node = expected;                                             \
  actual_node = actual;                                                 \
  while (expected_node)                                                 \
    {                                                                   \
      Match *exp = expected_node->data;                                 \
      Match *act = actual_node->data;                                   \
                                                                        \
      if (exp->string)                                                  \
        cut_assert_equal_string(exp->string, act->string);              \
      else                                                              \
        cut_assert_null(act->string);                                   \
                                                                        \
      expected_node = g_slist_next (expected_node);                     \
      actual_node = g_slist_next (actual_node);                         \
    }                                                                   \
                                                                        \
  g_regex_unref (regex);                                                \
  g_slist_foreach (expected, free_match, NULL);                         \
  g_slist_free (expected);                                              \
  g_slist_foreach (actual, free_match, NULL);                           \
  g_slist_free (actual);                                                \
} while (0)

void
test_match_next (void)
{
  /* TEST_MATCH_NEXT#(pattern, string, string_len, start_position, ...) */
  TEST_MATCH_NEXT("a", "x", -1, 0, NULL);
  TEST_MATCH_NEXT("a", "ax", -1, 1, NULL);
  TEST_MATCH_NEXT("a", "xa", 1, 0, NULL);
  TEST_MATCH_NEXT("a", "axa", 1, 2, NULL);
  TEST_MATCH_NEXT("a", "a", -1, 0, "a", 0, 1, NULL);
  TEST_MATCH_NEXT("a", "xax", -1, 0, "a", 1, 2, NULL);
  TEST_MATCH_NEXT(EURO, ENG EURO, -1, 0, EURO, 2, 5, NULL);
  TEST_MATCH_NEXT("a*", "", -1, 0, "", 0, 0, NULL);
  TEST_MATCH_NEXT("a*", "aa", -1, 0, "aa", 0, 2, "", 2, 2, NULL);
  TEST_MATCH_NEXT(EURO "*", EURO EURO, -1, 0, EURO EURO, 0, 6, "", 6, 6, NULL);
  TEST_MATCH_NEXT("a", "axa", -1, 0, "a", 0, 1, "a", 2, 3, NULL);
  TEST_MATCH_NEXT("a+", "aaxa", -1, 0, "aa", 0, 2, "a", 3, 4, NULL);
  TEST_MATCH_NEXT("a", "aa", -1, 0, "a", 0, 1, "a", 1, 2, NULL);
  TEST_MATCH_NEXT("a", "ababa", -1, 2, "a", 2, 3, "a", 4, 5, NULL);
  TEST_MATCH_NEXT(EURO "+", EURO "-" EURO, -1, 0, EURO, 0, 3, EURO, 4, 7, NULL);
  TEST_MATCH_NEXT("", "ab", -1, 0, "", 0, 0, "", 1, 1, "", 2, 2, NULL);
  TEST_MATCH_NEXT("", AGRAVE "b", -1, 0, "", 0, 0, "", 2, 2, "", 3, 3, NULL);
  TEST_MATCH_NEXT("a", "aaxa", -1, 0, "a", 0, 1, "a", 1, 2, "a", 3, 4, NULL);
  TEST_MATCH_NEXT("a", "aa" OGRAVE "a", -1, 0, "a", 0, 1, "a", 1, 2, "a", 4, 5, NULL);
  TEST_MATCH_NEXT("a*", "aax", -1, 0, "aa", 0, 2, "", 2, 2, "", 3, 3, NULL);
  TEST_MATCH_NEXT("a*", "aaxa", -1, 0, "aa", 0, 2, "", 2, 2, "a", 3, 4, "", 4, 4, NULL);

}

#define TEST_SUB_PATTERN(pattern, string, start_position, sub_n, expected_sub, \
			 expected_start, expected_end) { \
  gchar *sub_expr; \
  gint start = UNTOUCHED, end = UNTOUCHED; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match_full (regex, string, -1, start_position, 0, &match_info, NULL); \
  sub_expr = g_match_info_fetch (match_info, sub_n); \
  cut_assert_equal_string (expected_sub, sub_expr); \
  g_free (sub_expr); \
  g_match_info_fetch_pos (match_info, sub_n, &start, &end); \
  cut_assert (start == expected_start && end == expected_end, \
      "failed (got [%d, %d], expected [%d, %d])", \
	       start, end, expected_start, expected_end); \
  g_regex_unref (regex); \
  g_match_info_free (match_info); \
  regex = NULL; \
  match_info = NULL; \
}

#define TEST_SUB_PATTERN_NULL(pattern, string, start_position, sub_n) { \
  gchar *sub_expr; \
  gint start = UNTOUCHED, end = UNTOUCHED; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match_full (regex, string, -1, start_position, 0, &match_info, NULL); \
  sub_expr = g_match_info_fetch (match_info, sub_n); \
  cut_assert_null (sub_expr); \
  g_match_info_fetch_pos (match_info, sub_n, &start, &end); \
  cut_assert (start == UNTOUCHED && end == UNTOUCHED); \
  g_regex_unref (regex); \
  g_match_info_free (match_info); \
  regex = NULL; \
  match_info = NULL; \
}

void
test_sub_pattern (void)
{
  /* TEST_SUB_PATTERN(pattern, string, start_position, sub_n, expected_sub,
   * 		      expected_start, expected_end) */
  TEST_SUB_PATTERN("a", "a", 0, 0, "a", 0, 1);
  TEST_SUB_PATTERN("a(.)", "ab", 0, 1, "b", 1, 2);
  TEST_SUB_PATTERN("a(.)", "a" EURO, 0, 1, EURO, 1, 4);
  TEST_SUB_PATTERN("(?:.*)(a)(.)", "xxa" ENG, 0, 2, ENG, 3, 5);
  TEST_SUB_PATTERN("(" HSTROKE ")", "a" HSTROKE ENG, 0, 1, HSTROKE, 1, 3);
  TEST_SUB_PATTERN("(a)?(b)", "b", 0, 0, "b", 0, 1);
  TEST_SUB_PATTERN("(a)?(b)", "b", 0, 1, "", -1, -1);
  TEST_SUB_PATTERN("(a)?(b)", "b", 0, 2, "b", 0, 1);

  TEST_SUB_PATTERN_NULL("a", "a", 0, 1);
  TEST_SUB_PATTERN_NULL("a", "a", 0, 1);
}

#define TEST_NAMED_SUB_PATTERN(pattern, string, start_position, sub_name, \
			       expected_sub, expected_start, expected_end) { \
  gint start = UNTOUCHED, end = UNTOUCHED; \
  gchar *sub_expr; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match_full (regex, string, -1, start_position, 0, &match_info, NULL); \
  sub_expr = g_match_info_fetch_named (match_info, sub_name); \
  cut_assert_equal_string (expected_sub, sub_expr); \
  g_free (sub_expr); \
  g_match_info_fetch_named_pos (match_info, sub_name, &start, &end); \
  cut_assert (start == expected_start && end == expected_end, \
      "failed (got [%d, %d], expected [%d, %d])", \
	       start, end, expected_start, expected_end); \
  g_regex_unref (regex); \
  g_match_info_free (match_info); \
  regex = NULL; \
  match_info = NULL; \
}

#define TEST_NAMED_SUB_PATTERN_NULL(pattern, string, start_position, sub_name, \
			            expected_start, expected_end) { \
  gint start = UNTOUCHED, end = UNTOUCHED; \
  gchar *sub_expr; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match_full (regex, string, -1, start_position, 0, &match_info, NULL); \
  sub_expr = g_match_info_fetch_named (match_info, sub_name); \
  cut_assert_null (sub_expr); \
  g_match_info_fetch_named_pos (match_info, sub_name, &start, &end); \
  cut_assert (start == expected_start && end == expected_end, \
      "failed (got [%d, %d], expected [%d, %d])", \
	       start, end, expected_start, expected_end); \
  g_regex_unref (regex); \
  g_match_info_free (match_info); \
  regex = NULL; \
  match_info = NULL; \
}

#define TEST_NAMED_SUB_PATTERN_DUPNAMES(pattern, string, start_position, sub_name, \
					expected_sub, expected_start, expected_end) { \
  gint start = UNTOUCHED, end = UNTOUCHED; \
  gchar *sub_expr; \
  regex = g_regex_new (pattern, G_REGEX_DUPNAMES, 0, NULL); \
  g_regex_match_full (regex, string, -1, start_position, 0, &match_info, NULL); \
  sub_expr = g_match_info_fetch_named (match_info, sub_name); \
  cut_assert_equal_string (expected_sub, sub_expr); \
  g_free (sub_expr); \
  g_match_info_fetch_named_pos (match_info, sub_name, &start, &end); \
  cut_assert (start == expected_start && end == expected_end, \
      "failed (got [%d, %d], expected [%d, %d])", \
	       start, end, expected_start, expected_end); \
  g_regex_unref (regex); \
  g_match_info_free (match_info); \
  regex = NULL; \
  match_info = NULL; \
}

void
test_named_sub_pattern (void)
{
  /* TEST_NAMED_SUB_PATTERN(pattern, string, start_position, sub_name,
   * 			    expected_sub, expected_start, expected_end) */
  TEST_NAMED_SUB_PATTERN("a(?P<A>.)(?P<B>.)?", "ab", 0, "A", "b", 1, 2);
  TEST_NAMED_SUB_PATTERN("a(?P<A>.)(?P<B>.)?", "aab", 1, "A", "b", 2, 3);
  TEST_NAMED_SUB_PATTERN("a(?P<A>.)(?P<B>.)?", EURO "ab", 0, "A", "b", 4, 5);
  TEST_NAMED_SUB_PATTERN("a(?P<A>.)(?P<B>.)?", "a" EGRAVE "x", 0, "A", EGRAVE, 1, 3);
  TEST_NAMED_SUB_PATTERN("a(?P<A>.)(?P<B>.)?", "a" EGRAVE "x", 0, "B", "x", 3, 4);
  TEST_NAMED_SUB_PATTERN("(?P<A>a)?(?P<B>b)", "b", 0, "A", "", -1, -1);
  TEST_NAMED_SUB_PATTERN("(?P<A>a)?(?P<B>b)", "b", 0, "B", "b", 0, 1);

  TEST_NAMED_SUB_PATTERN_NULL("a(?P<A>.)(?P<B>.)?", EURO "ab", 0, "B", UNTOUCHED, UNTOUCHED);
  TEST_NAMED_SUB_PATTERN_NULL("a(?P<A>.)(?P<B>.)?", EURO "ab", 0, "C", UNTOUCHED, UNTOUCHED);

  /* TEST_NAMED_SUB_PATTERN_DUPNAMES(pattern, string, start_position, sub_name,
   *				     expected_sub, expected_start, expected_end) */
  TEST_NAMED_SUB_PATTERN_DUPNAMES("(?P<N>a)|(?P<N>b)", "ab", 0, "N", "a", 0, 1);
  TEST_NAMED_SUB_PATTERN_DUPNAMES("(?P<N>aa)|(?P<N>a)", "aa", 0, "N", "aa", 0, 2);
  TEST_NAMED_SUB_PATTERN_DUPNAMES("(?P<N>aa)(?P<N>a)", "aaa", 0, "N", "aa", 0, 2);
  TEST_NAMED_SUB_PATTERN_DUPNAMES("(?P<N>x)|(?P<N>a)", "a", 0, "N", "a", 0, 1);
  TEST_NAMED_SUB_PATTERN_DUPNAMES("(?P<N>x)y|(?P<N>a)b", "ab", 0, "N", "a", 0, 1);

  /* DUPNAMES option inside the pattern */
  TEST_NAMED_SUB_PATTERN("(?J)(?P<N>a)|(?P<N>b)", "ab", 0, "N", "a", 0, 1);
  TEST_NAMED_SUB_PATTERN("(?J)(?P<N>aa)|(?P<N>a)", "aa", 0, "N", "aa", 0, 2);
  TEST_NAMED_SUB_PATTERN("(?J)(?P<N>aa)(?P<N>a)", "aaa", 0, "N", "aa", 0, 2);
  TEST_NAMED_SUB_PATTERN("(?J)(?P<N>x)|(?P<N>a)", "a", 0, "N", "a", 0, 1);
  TEST_NAMED_SUB_PATTERN("(?J)(?P<N>x)y|(?P<N>a)b", "ab", 0, "N", "a", 0, 1);
}

#define TEST_FETCH_ALL(pattern, string, expected) { \
  gchar **matches; \
  gint match_count; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match (regex, string, 0, &match_info); \
  matches = g_match_info_fetch_all (match_info); \
  if (matches) \
    match_count = g_strv_length (matches); \
  else \
    match_count = 0; \
  cut_assert_equal_int (g_strv_length (expected), match_count); \
  cut_assert_equal_string_array (expected, matches); \
  g_match_info_free (match_info); \
  g_regex_unref (regex); \
  g_strfreev (matches); \
  match_info = NULL; \
  regex = NULL; \
}

#define TEST_FETCH_ALL_NULL(pattern, string) { \
  gchar **matches; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  g_regex_match (regex, string, 0, &match_info); \
  matches = g_match_info_fetch_all (match_info); \
  cut_assert_null (matches); \
  g_match_info_free (match_info); \
  g_regex_unref (regex); \
  match_info = NULL; \
  regex = NULL; \
}

void
test_fetch_all (void)
{
  gchar *expected2[] = {"a", NULL};
  gchar *expected3[] = {"aa", NULL};
  gchar *expected4[] = {"a", "a", NULL};
  gchar *expected5[] = {"ab", "b", NULL};
  gchar *expected6[] = {"a" HSTROKE, HSTROKE, NULL};
  gchar *expected7[] = {"xyaz", "a", "z", NULL};
  gchar *expected8[] = {"xa", "x", "a", NULL};
  gchar *expected9[] = {ENG "a", ENG, "a", NULL};
  gchar *expected10[] = {"b", "", "b", NULL};
  gchar *expected11[] = {"ab", "a", "b", NULL};

  TEST_FETCH_ALL_NULL("a", "");
  TEST_FETCH_ALL_NULL("a", "b");
  TEST_FETCH_ALL("a", "a", expected2);
  TEST_FETCH_ALL("a+", "aa", expected3);
  TEST_FETCH_ALL("(?:a)", "a", expected2);
  TEST_FETCH_ALL("(a)", "a", expected4);
  TEST_FETCH_ALL("a(.)", "ab", expected5);
  TEST_FETCH_ALL("a(.)", "a" HSTROKE, expected6);
  TEST_FETCH_ALL("(?:.*)(a)(.)", "xyazk", expected7);
  TEST_FETCH_ALL("(?P<A>.)(a)", "xa", expected8);
  TEST_FETCH_ALL("(?P<A>.)(a)", ENG "a", expected9);
  TEST_FETCH_ALL("(a)?(b)", "b", expected10);
  TEST_FETCH_ALL("(a)?(b)", "ab", expected11);
}

#define TEST_SPLIT_SIMPLE(pattern, string, expected) { \
  gchar **tokens; \
  gint token_count; \
  tokens = g_regex_split_simple (pattern, string, 0, 0); \
  if (tokens) \
    token_count = g_strv_length (tokens); \
  else \
    token_count = 0; \
  cut_assert_equal_int (g_strv_length (expected), token_count); \
  cut_assert_equal_string_array (expected, tokens); \
  g_strfreev (tokens); \
}

#define TEST_SPLIT_SIMPLE_NULL(pattern, string) { \
  gchar **tokens; \
  tokens = g_regex_split_simple (pattern, string, 0, 0); \
  cut_assert_null (tokens); \
}

void
test_split_simple (void)
{
  gchar *expected1[] = {NULL};
  gchar *expected2[] = {"a", NULL};
  gchar *expected3[] = {"a", "b", NULL};
  gchar *expected4[] = {"a", "b", "c", NULL};
  gchar *expected5[] = {"a", ",", "b", NULL};
  gchar *expected6[] = {"x", "y", NULL};
  gchar *expected7[] = {"x", "b", "y", NULL};

  TEST_SPLIT_SIMPLE("", "", expected1);
  TEST_SPLIT_SIMPLE("a", "", expected1);
  TEST_SPLIT_SIMPLE(",", "a", expected2);
  TEST_SPLIT_SIMPLE("(,)\\s*", "a", expected2);
  TEST_SPLIT_SIMPLE(",", "a,b", expected3);
  TEST_SPLIT_SIMPLE(",", "a,b,c", expected4);
  TEST_SPLIT_SIMPLE(",\\s*", "a,b,c", expected4);
  TEST_SPLIT_SIMPLE(",\\s*", "a, b, c", expected4);
  TEST_SPLIT_SIMPLE("(,)\\s*", "a,b", expected5);
  TEST_SPLIT_SIMPLE("(,)\\s*", "a, b", expected5);
  /* Not matched sub-strings. */
  TEST_SPLIT_SIMPLE("a|(b)", "xay", expected6);
  TEST_SPLIT_SIMPLE("a|(b)", "xby", expected7);
  /* Empty matches. */
  TEST_SPLIT_SIMPLE("", "abc", expected4);
  TEST_SPLIT_SIMPLE(" *", "ab c", expected4);
  /* Invalid patterns. */
  TEST_SPLIT_SIMPLE_NULL("\\", "");
  TEST_SPLIT_SIMPLE_NULL("[", "");
}

#define TEST_SPLIT(pattern, string, start_position, max_tokens, expected) { \
  gchar **tokens; \
  gint token_count; \
  regex = g_regex_new (pattern, 0, 0, NULL); \
  tokens = g_regex_split_full (regex, string, -1, start_position, \
			       0, max_tokens, NULL); \
  if (tokens) \
    token_count = g_strv_length (tokens); \
  else \
    token_count = 0; \
  cut_assert_equal_int (g_strv_length (expected), token_count); \
  cut_assert_equal_string_array (expected, tokens); \
  g_strfreev (tokens); \
\
  if (start_position == 0 && max_tokens <= 0) {\
    tokens = g_regex_split (regex, string, 0); \
    if (tokens) \
      token_count = g_strv_length (tokens); \
    else \
      token_count = 0; \
    cut_assert_equal_int (g_strv_length (expected), token_count); \
    cut_assert_equal_string_array (expected, tokens); \
    g_strfreev (tokens); \
  } \
  g_regex_unref (regex); \
  regex = NULL; \
}

void
test_split (void)
{
  gchar *expected1[] = {NULL};
  gchar *expected2[] = {"a", NULL};
  gchar *expected3[] = {"a,b", NULL};
  gchar *expected4[] = {"b", NULL};
  gchar *expected5[] = {"a", "b", NULL};
  gchar *expected6[] = {"a", "b,c", NULL};
  gchar *expected7[] = {"", "b", NULL};
  gchar *expected8[] = {"a", "", NULL};
  gchar *expected9[] = {"a", "b", "c", NULL};
  gchar *expected10[] = {"a", ",", "b", NULL};
  gchar *expected11[] = {"x", "y", NULL};
  gchar *expected12[] = {"x", "b", "y", NULL};
  gchar *expected13[] = {"b", "c", NULL};
  gchar *expected14[] = {"ab c", NULL};
  gchar *expected15[] = {"a", "b c", NULL};

  TEST_SPLIT("", "", 0, 0, expected1);
  TEST_SPLIT("a", "", 0, 0, expected1);
  TEST_SPLIT("a", "", 0, 1, expected1);
  TEST_SPLIT("a", "", 0, 2, expected1);
  TEST_SPLIT("a", "a", 1, 0, expected1);

  TEST_SPLIT(",", "a", 0, 0, expected2);
  TEST_SPLIT(",", "a,b", 0, 1, expected3);
  TEST_SPLIT("(,)\\s*", "a", 0, 0, expected2);
  TEST_SPLIT(",", "a,b", 2, 0, expected4);
  TEST_SPLIT(",", "a,b", 0, 0, expected5);
  TEST_SPLIT(",", "a,b,c", 0, 2, expected6);
  TEST_SPLIT(",", "a,b", 1, 0, expected7);
  TEST_SPLIT(",", "a,", 0, 0, expected8);
  TEST_SPLIT(",", "a,b,c", 0, 0, expected9);
  TEST_SPLIT(",\\s*", "a,b,c", 0, 0, expected9);
  TEST_SPLIT(",\\s*", "a, b, c", 0, 0, expected9);
  TEST_SPLIT("(,)\\s*", "a,b", 0, 0, expected10);
  TEST_SPLIT("(,)\\s*", "a, b", 0, 0, expected10);
  /* Not matched sub-strings. */
  TEST_SPLIT("a|(b)", "xay", 0, 0, expected11);
  TEST_SPLIT("a|(b)", "xby", 0, -1, expected12);
  /* Empty matches. */
  TEST_SPLIT(" *", "ab c", 1, 0, expected13);
  TEST_SPLIT("", "abc", 0, 0, expected9);
  TEST_SPLIT(" *", "ab c", 0, 0, expected9);
  TEST_SPLIT(" *", "ab c", 0, 1, expected14);
  TEST_SPLIT(" *", "ab c", 0, 2, expected15);
  TEST_SPLIT(" *", "ab c", 0, 3, expected9);
  TEST_SPLIT(" *", "ab c", 0, 4, expected9);
}

static gboolean
test_match_all_full (const gchar *pattern,
		     const gchar *string,
		     gssize       string_len,
		     gint         start_position,
		     ...)
{
  GRegex *regex;
  GMatchInfo *match_info;
  va_list args;
  GSList *expected = NULL;
  GSList *l_exp;
  gboolean match_ok;
  gboolean ret = TRUE;
  gint match_count;
  gint i;
  
  /* The va_list is a NULL-terminated sequence of: extected matched string,
   * expected start and expected end. */
  va_start (args, start_position);
  while (TRUE)
   {
      Match *match;
      const gchar *expected_string = va_arg (args, const gchar *);
      if (expected_string == NULL)
        break;
      match = g_new0 (Match, 1);
      match->string = g_strdup (expected_string);
      match->start = va_arg (args, gint);
      match->end = va_arg (args, gint);
      expected = g_slist_prepend (expected, match);
    }
  expected = g_slist_reverse (expected);
  va_end (args);

  regex = g_regex_new (pattern, 0, 0, NULL);
  match_ok = g_regex_match_all_full (regex, string, string_len, start_position,
				     0, &match_info, NULL);

  if (match_ok && g_slist_length (expected) == 0)
    {
      g_print ("failed\n");
      ret = FALSE;
      goto exit;
    }
  if (!match_ok && g_slist_length (expected) != 0)
    {
      g_print ("failed\n");
      ret = FALSE;
      goto exit;
    }

  match_count = g_match_info_get_match_count (match_info);
  if (match_count != g_slist_length (expected))
    {
      g_print ("failed \t(got %d %s, expected %d)\n", match_count,
	       match_count == 1 ? "match" : "matches", 
	       g_slist_length (expected));
      ret = FALSE;
      goto exit;
    }

  l_exp = expected;
  for (i = 0; i < match_count; i++)
    {
      gint start, end;
      gchar *matched_string;
      Match *exp = l_exp->data;

      matched_string = g_match_info_fetch (match_info, i);
      g_match_info_fetch_pos (match_info, i, &start, &end);

      if (!streq(exp->string, matched_string))
	{
	  g_print ("failed \t(got \"%s\", expected \"%s\")\n",
		   matched_string, exp->string);
          g_free (matched_string);
	  ret = FALSE;
	  goto exit;
	}
      g_free (matched_string);

      if (exp->start != start || exp->end != end)
	{
	  g_print ("failed \t(got [%d, %d], expected [%d, %d])\n",
		   start, end, exp->start, exp->end);
	  ret = FALSE;
	  goto exit;
	}

      l_exp = g_slist_next (l_exp);
    }

exit:
  g_match_info_free (match_info);
  g_regex_unref (regex);
  g_slist_foreach (expected, free_match, NULL);
  g_slist_free (expected);

  return ret;
}

#define TEST_MATCH_ALL(pattern, string, string_len, start_position, expected) { \
  total++; \
  if (test_match_all_full (pattern, string, string_len, start_position, NULL)) \
    PASS; \
  else \
    FAIL; \
  if (string_len == -1 && start_position == 0) \
  { \
    total++; \
    if (test_match_all (pattern, string, NULL)) \
      PASS; \
    else \
      FAIL; \
  } \
}

static gboolean
test_match_all (const gchar *pattern,
		const gchar *string,
                ...)
{
  GRegex *regex;
  GMatchInfo *match_info;
  va_list args;
  GSList *expected = NULL;
  GSList *l_exp;
  gboolean match_ok;
  gboolean ret = TRUE;
  gint match_count;
  gint i;
  
  /* The va_list is a NULL-terminated sequence of: extected matched string,
   * expected start and expected end. */
  va_start (args, string);
  while (TRUE)
   {
      Match *match;
      const gchar *expected_string = va_arg (args, const gchar *);
      if (expected_string == NULL)
        break;
      match = g_new0 (Match, 1);
      match->string = g_strdup (expected_string);
      match->start = va_arg (args, gint);
      match->end = va_arg (args, gint);
      expected = g_slist_prepend (expected, match);
    }
  expected = g_slist_reverse (expected);
  va_end (args);

  regex = g_regex_new (pattern, 0, 0, NULL);
  match_ok = g_regex_match_all (regex, string, 0, &match_info);

  if (match_ok && g_slist_length (expected) == 0)
    {
      g_print ("failed\n");
      ret = FALSE;
      goto exit;
    }
  if (!match_ok && g_slist_length (expected) != 0)
    {
      g_print ("failed\n");
      ret = FALSE;
      goto exit;
    }

  match_count = g_match_info_get_match_count (match_info);
  if (match_count != g_slist_length (expected))
    {
      g_print ("failed \t(got %d %s, expected %d)\n", match_count,
	       match_count == 1 ? "match" : "matches", 
	       g_slist_length (expected));
      ret = FALSE;
      goto exit;
    }

  l_exp = expected;
  for (i = 0; i < match_count; i++)
    {
      gint start, end;
      gchar *matched_string;
      Match *exp = l_exp->data;

      matched_string = g_match_info_fetch (match_info, i);
      g_match_info_fetch_pos (match_info, i, &start, &end);

      if (!streq(exp->string, matched_string))
	{
	  g_print ("failed \t(got \"%s\", expected \"%s\")\n",
		   matched_string, exp->string);
          g_free (matched_string);
	  ret = FALSE;
	  goto exit;
	}
      g_free (matched_string);

      if (exp->start != start || exp->end != end)
	{
	  g_print ("failed \t(got [%d, %d], expected [%d, %d])\n",
		   start, end, exp->start, exp->end);
	  ret = FALSE;
	  goto exit;
	}

      l_exp = g_slist_next (l_exp);
    }

exit:

  g_match_info_free (match_info);
  g_regex_unref (regex);
  g_slist_foreach (expected, free_match, NULL);
  g_slist_free (expected);

  return ret;
}

#define TEST_MATCH_ALL1(pattern, string, string_len, start_position, \
			t1, s1, e1) { \
  total++; \
  if (test_match_all_full (pattern, string, string_len, start_position, \
			   t1, s1, e1, NULL)) \
    PASS; \
  else \
    FAIL; \
  if (string_len == -1 && start_position == 0) \
  { \
    total++; \
    if (test_match_all (pattern, string, t1, s1, e1, NULL)) \
      PASS; \
    else \
      FAIL; \
  } \
}

#define TEST_MATCH_ALL2(pattern, string, string_len, start_position, \
			t1, s1, e1, t2, s2, e2) { \
  total++; \
  if (test_match_all_full (pattern, string, string_len, start_position, \
			   t1, s1, e1, t2, s2, e2, NULL)) \
    PASS; \
  else \
    FAIL; \
  if (string_len == -1 && start_position == 0) \
  { \
    total++; \
    if (test_match_all (pattern, string, t1, s1, e1, t2, s2, e2, NULL)) \
      PASS; \
    else \
      FAIL; \
  } \
}

#define TEST_MATCH_ALL3(pattern, string, string_len, start_position, \
			t1, s1, e1, t2, s2, e2, t3, s3, e3) { \
  total++; \
  if (test_match_all_full (pattern, string, string_len, start_position, \
			   t1, s1, e1, t2, s2, e2, t3, s3, e3, NULL)) \
    PASS; \
  else \
    FAIL; \
  if (string_len == -1 && start_position == 0) \
  { \
    total++; \
    if (test_match_all (pattern, string, t1, s1, e1, t2, s2, e2, t3, s3, e3, NULL)) \
      PASS; \
    else \
      FAIL; \
  } \
}
#if 0
int
main (int argc, char *argv[])
{
/* TEST_MATCH_ALL#(pattern, string, string_len, start_position, ...) */
  TEST_MATCH_ALL0("<.*>", "", -1, 0);
  TEST_MATCH_ALL0("a+", "", -1, 0);
  TEST_MATCH_ALL0("a+", "a", 0, 0);
  TEST_MATCH_ALL0("a+", "a", -1, 1);
  TEST_MATCH_ALL1("<.*>", "<a>", -1, 0, "<a>", 0, 3);
  TEST_MATCH_ALL1("a+", "a", -1, 0, "a", 0, 1);
  TEST_MATCH_ALL1("a+", "aa", 1, 0, "a", 0, 1);
  TEST_MATCH_ALL1("a+", "aa", -1, 1, "a", 1, 2);
  TEST_MATCH_ALL1("a+", "aa", 2, 1, "a", 1, 2);
  TEST_MATCH_ALL1(".+", ENG, -1, 0, ENG, 0, 2);
  TEST_MATCH_ALL2("<.*>", "<a><b>", -1, 0, "<a><b>", 0, 6, "<a>", 0, 3);
  TEST_MATCH_ALL2("a+", "aa", -1, 0, "aa", 0, 2, "a", 0, 1);
  TEST_MATCH_ALL2(".+", ENG EURO, -1, 0, ENG EURO, 0, 5, ENG, 0, 2);
  TEST_MATCH_ALL3("<.*>", "<a><b><c>", -1, 0, "<a><b><c>", 0, 9,
		  "<a><b>", 0, 6, "<a>", 0, 3);
  TEST_MATCH_ALL3("a+", "aaa", -1, 0, "aaa", 0, 3, "aa", 0, 2, "a", 0, 1);
}
#endif

