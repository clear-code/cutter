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
void test_match_all (void);
void test_sub_pattern (void);
void test_named_sub_pattern (void);
void test_fetch_all (void);
void test_split_simple (void);
void test_split (void);

typedef struct _Match
{
  gchar *string;
  gint start, end;
} Match;

static GRegex *regex;
static GMatchInfo *match_info;
static GSList *expected_matches, *actual_matches;
static gchar *escaped_string, *replaced_string, *expanded_string, *substring;

void
setup (void)
{
  regex = NULL;
  match_info = NULL;
  expected_matches = NULL;
  actual_matches = NULL;
  escaped_string = NULL;
  replaced_string = NULL;
  expanded_string = NULL;
  substring = NULL;
}

static void
free_regex (void)
{
  if (regex)
    g_regex_unref (regex);
  regex = NULL;
}

static void
free_match_info (void)
{
  if (match_info)
    g_match_info_free (match_info);
  match_info = NULL;

}

static void
free_match (gpointer data, gpointer user_data)
{
  Match *match = data;
  if (match == NULL)
    return;
  g_free (match->string);
  g_free (match);
}

static void
free_expected_matches (void)
{
  g_slist_foreach (expected_matches, free_match, NULL);
  g_slist_free (expected_matches);
  expected_matches = NULL;
}

static void
free_actual_matches (void)
{
  g_slist_foreach (actual_matches, free_match, NULL);
  g_slist_free (actual_matches);
  actual_matches = NULL;
}

static void
free_escaped_string (void)
{
  g_free (escaped_string);
  escaped_string = NULL;
}

static void
free_replaced_string (void)
{
  g_free (replaced_string);
  replaced_string = NULL;
}

static void
free_expanded_string (void)
{
  g_free (expanded_string);
  expanded_string = NULL;
}

static void
free_substring (void)
{
  g_free (substring);
  substring = NULL;
}

void
teardown (void)
{
  free_regex ();
  free_match_info ();
  free_expected_matches ();
  free_actual_matches ();
  free_escaped_string ();
  free_replaced_string ();
  free_expanded_string ();
  free_substring ();
}

static void
cut_assert_regex_new_without_free (const gchar       *pattern,
                                   GRegexCompileFlags compile_opts,
                                   GRegexMatchFlags   match_opts)
{
  regex = g_regex_new (pattern, compile_opts, match_opts, NULL);
  cut_assert (regex,
              cut_message ("failed (pattern: \"%s\", compile: %d, match %d)",
                           pattern, compile_opts, match_opts));
  cut_assert_equal_string (pattern, g_regex_get_pattern (regex));
}

static void
cut_assert_regex_new (const gchar       *pattern,
		      GRegexCompileFlags compile_opts,
		      GRegexMatchFlags   match_opts)
{
  cut_assert_regex_new_without_free (pattern, compile_opts, match_opts);
  free_regex ();
}

void
test_regex_new (void)
{
  cut_assert_regex_new ("", 0, 0);
  cut_assert_regex_new (".*", 0, 0);
  cut_assert_regex_new (".*", G_REGEX_OPTIMIZE, 0);
  cut_assert_regex_new (".*", G_REGEX_MULTILINE, 0);
  cut_assert_regex_new (".*", G_REGEX_DOTALL, 0);
  cut_assert_regex_new (".*", G_REGEX_DOTALL, G_REGEX_MATCH_NOTBOL);
  cut_assert_regex_new ("(123\\d*)[a-zA-Z]+(?P<hello>.*)", 0, 0);
  cut_assert_regex_new ("(123\\d*)[a-zA-Z]+(?P<hello>.*)", G_REGEX_CASELESS, 0);
  cut_assert_regex_new ("(123\\d*)[a-zA-Z]+(?P<hello>.*)",
                        G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0);
  cut_assert_regex_new ("(?P<A>x)|(?P<A>y)", G_REGEX_DUPNAMES, 0);
  cut_assert_regex_new ("(?P<A>x)|(?P<A>y)",
                        G_REGEX_DUPNAMES | G_REGEX_OPTIMIZE, 0);
  /* This gives "internal error: code overflow" with pcre 6.0 */
  cut_assert_regex_new ("(?i)(?-i)", 0, 0);
}

static void
cut_assert_regex_new_fail (const gchar       *pattern,
			   GRegexCompileFlags compile_opts)
{
  regex = g_regex_new (pattern, compile_opts, 0, NULL);
  cut_assert_null (regex,
                   cut_message ("failed (pattern: \"%s\", compile: %d)",
                                pattern, compile_opts));
}

void
test_regex_new_fail (void)
{
  cut_assert_regex_new_fail ("(", 0);
  cut_assert_regex_new_fail (")", 0);
  cut_assert_regex_new_fail ("[", 0);
  cut_assert_regex_new_fail ("*", 0);
  cut_assert_regex_new_fail ("?", 0);
  cut_assert_regex_new_fail ("(?P<A>x)|(?P<A>y)", 0);
}

#define cut_assert_match_simple(pattern, string,                        \
                                compile_options, match_options)         \
  cut_assert (g_regex_match_simple (pattern, string,                    \
                                    compile_options, match_options))

#define cut_assert_not_match_simple(pattern, string,                    \
                                    compile_options, match_options)     \
  cut_assert (!g_regex_match_simple (pattern, string,                   \
                                     compile_options, match_options))

void
test_match_simple (void)
{
  cut_assert_not_match_simple ("a", "", 0, 0);
  cut_assert_not_match_simple ("a", "", 0, 0);
  cut_assert_match_simple ("a", "a", 0, 0);
  cut_assert_match_simple ("a", "ba", 0, 0);
  cut_assert_not_match_simple ("^a", "ba", 0, 0);
  cut_assert_not_match_simple ("a", "ba", G_REGEX_ANCHORED, 0);
  cut_assert_not_match_simple ("a", "ba", 0, G_REGEX_MATCH_ANCHORED);
  cut_assert_match_simple ("a", "ab", G_REGEX_ANCHORED, 0);
  cut_assert_match_simple ("a", "ab", 0, G_REGEX_MATCH_ANCHORED);
  cut_assert_match_simple ("a", "a", G_REGEX_CASELESS, 0);
  cut_assert_match_simple ("a", "A", G_REGEX_CASELESS, 0);
  /* These are needed to test extended properties. */
  cut_assert_match_simple (AGRAVE, AGRAVE, G_REGEX_CASELESS, 0);
  cut_assert_match_simple (AGRAVE, AGRAVE_UPPER, G_REGEX_CASELESS, 0);
  cut_assert_match_simple ("\\p{L}", "a", 0, 0);
  cut_assert_not_match_simple ("\\p{L}", "1", 0, 0);
  cut_assert_match_simple ("\\p{L}", AGRAVE, 0, 0);
  cut_assert_match_simple ("\\p{L}", AGRAVE_UPPER, 0, 0);
  cut_assert_match_simple ("\\p{L}", SHEEN, 0, 0);
  cut_assert_not_match_simple ("\\p{L}", ETH30, 0, 0);
  cut_assert_match_simple ("\\p{Ll}", "a", 0, 0);
  cut_assert_match_simple ("\\p{Ll}", AGRAVE, 0, 0);
  cut_assert_not_match_simple ("\\p{Ll}", AGRAVE_UPPER, 0, 0);
  cut_assert_not_match_simple ("\\p{Ll}", ETH30, 0, 0);
  cut_assert_not_match_simple ("\\p{Sc}", AGRAVE, 0, 0);
  cut_assert_match_simple ("\\p{Sc}", EURO, 0, 0);
  cut_assert_not_match_simple ("\\p{Sc}", ETH30, 0, 0);
  cut_assert_not_match_simple ("\\p{N}", "a", 0, 0);
  cut_assert_match_simple ("\\p{N}", "1", 0, 0);
  cut_assert_not_match_simple ("\\p{N}", AGRAVE, 0, 0);
  cut_assert_not_match_simple ("\\p{N}", AGRAVE_UPPER, 0, 0);
  cut_assert_not_match_simple ("\\p{N}", SHEEN, 0, 0);
  cut_assert_match_simple ("\\p{N}", ETH30, 0, 0);
  cut_assert_not_match_simple ("\\p{Nd}", "a", 0, 0);
  cut_assert_match_simple ("\\p{Nd}", "1", 0, 0);
  cut_assert_not_match_simple ("\\p{Nd}", AGRAVE, 0, 0);
  cut_assert_not_match_simple ("\\p{Nd}", AGRAVE_UPPER, 0, 0);
  cut_assert_not_match_simple ("\\p{Nd}", SHEEN, 0, 0);
  cut_assert_not_match_simple ("\\p{Nd}", ETH30, 0, 0);
  cut_assert_not_match_simple ("\\p{Common}", SHEEN, 0, 0);
  cut_assert_not_match_simple ("\\p{Common}", "a", 0, 0);
  cut_assert_not_match_simple ("\\p{Common}", AGRAVE, 0, 0);
  cut_assert_not_match_simple ("\\p{Common}", AGRAVE_UPPER, 0, 0);
  cut_assert_not_match_simple ("\\p{Common}", ETH30, 0, 0);
  cut_assert_match_simple ("\\p{Common}", "%", 0, 0);
  cut_assert_match_simple ("\\p{Common}", "1", 0, 0);
  cut_assert_match_simple ("\\p{Arabic}", SHEEN, 0, 0);
  cut_assert_not_match_simple ("\\p{Arabic}", "a", 0, 0);
  cut_assert_not_match_simple ("\\p{Arabic}", AGRAVE, 0, 0);
  cut_assert_not_match_simple ("\\p{Arabic}", AGRAVE_UPPER, 0, 0);
  cut_assert_not_match_simple ("\\p{Arabic}", ETH30, 0, 0);
  cut_assert_not_match_simple ("\\p{Arabic}", "%", 0, 0);
  cut_assert_not_match_simple ("\\p{Arabic}", "1", 0, 0);
  cut_assert_not_match_simple ("\\p{Latin}", SHEEN, 0, 0);
  cut_assert_match_simple ("\\p{Latin}", "a", 0, 0);
  cut_assert_match_simple ("\\p{Latin}", AGRAVE, 0, 0);
  cut_assert_match_simple ("\\p{Latin}", AGRAVE_UPPER, 0, 0);
  cut_assert_not_match_simple ("\\p{Latin}", ETH30, 0, 0);
  cut_assert_not_match_simple ("\\p{Latin}", "%", 0, 0);
  cut_assert_not_match_simple ("\\p{Latin}", "1", 0, 0);
  cut_assert_not_match_simple ("\\p{Ethiopic}", SHEEN, 0, 0);
  cut_assert_not_match_simple ("\\p{Ethiopic}", "a", 0, 0);
  cut_assert_not_match_simple ("\\p{Ethiopic}", AGRAVE, 0, 0);
  cut_assert_not_match_simple ("\\p{Ethiopic}", AGRAVE_UPPER, 0, 0);
  cut_assert_match_simple ("\\p{Ethiopic}", ETH30, 0, 0);
  cut_assert_not_match_simple ("\\p{Ethiopic}", "%", 0, 0);
  cut_assert_not_match_simple ("\\p{Ethiopic}", "1", 0, 0);
  cut_assert_match_simple ("\\p{L}(?<=\\p{Arabic})", SHEEN, 0, 0);
  cut_assert_not_match_simple ("\\p{L}(?<=\\p{Latin})", SHEEN, 0, 0);
  /* Invalid patterns. */
  cut_assert_not_match_simple ("\\", "a", 0, 0);
  cut_assert_not_match_simple ("[", "", 0, 0);
}

static void
cut_assert_match_full (const gchar        *pattern,
                       GRegexCompileFlags  compile_options,
                       GRegexMatchFlags    pattern_match_options,
                       const gchar        *string,
                       gssize              length,
                       gint                start_position,
                       GRegexMatchFlags    match_options)
{
  cut_assert_regex_new_without_free (pattern, compile_options,
                                     pattern_match_options);
  cut_assert (g_regex_match_full (regex, string, length, start_position,
                                  match_options, NULL, NULL),
              cut_message ("/%s/ =~ <%s>", pattern, string));
  if (length == -1 && start_position == 0)
    {
      cut_assert (g_regex_match (regex, string, match_options, NULL),
	          cut_message ("failed (pattern: \"%s\", string: \"%s\")",
                               pattern, string));
    }
  free_regex ();
}

void
test_match (void)
{
  cut_assert_match_full ("a", 0, 0, "a", -1, 0, 0);
  cut_assert_match_full ("a", G_REGEX_CASELESS, 0, "A", -1, 0, 0);
  cut_assert_match_full ("a", 0, 0, "bab", -1, 0, 0);
  cut_assert_match_full ("a", 0, G_REGEX_ANCHORED, "a", -1, 0, 0);
  cut_assert_match_full ("a|b", 0, 0, "a", -1, 0, 0);
  cut_assert_match_full ("^.$", 0, 0, EURO, -1, 0, 0);
  cut_assert_match_full ("^.{3}$", G_REGEX_RAW, 0, EURO, -1, 0, 0);
  cut_assert_match_full (AGRAVE, G_REGEX_CASELESS, 0, AGRAVE_UPPER, -1, 0, 0);
  cut_assert_match_full ("a", 0, 0, "a", -1, 0, G_REGEX_ANCHORED);

  /* New lines handling. */
  cut_assert_match_full ("^a\\Rb$", 0, 0, "a\r\nb", -1, 0, 0);
  cut_assert_match_full ("^a\\Rb$", 0, 0, "a\nb", -1, 0, 0);
  cut_assert_match_full ("^a\\Rb$", 0, 0, "a\rb", -1, 0, 0);
  cut_assert_match_full ("^a\\R\\Rb$", 0, 0, "a\n\rb", -1, 0, 0);
  cut_assert_match_full ("^a\\r\\nb$", 0, 0, "a\r\nb", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE, 0, "a\nb\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE, 0, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE, 0, "a\rb\rc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_LF, 0, "a\nb\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CRLF, 0, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, "a\rb\rc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_LF, "a\nb\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CRLF, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CR, "a\rb\rc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_ANY, "a\nb\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_ANY, "a\rb\rc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_ANY, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_LF, "a\nb\nc", -1, 0, 0);
  cut_assert_match_full ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_CRLF, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_match_full ("a#\nb", G_REGEX_EXTENDED | G_REGEX_NEWLINE_CR, 0, "a", -1, 0, 0);
}

static void
cut_assert_mismatch (const gchar       *pattern,
		     GRegexCompileFlags compile_opts,
		     GRegexMatchFlags   match_opts,
		     const gchar       *string,
		     gsize              string_len,
		     gint               start_position,
		     GRegexMatchFlags   match_opts2)
{
  cut_assert_regex_new_without_free (pattern, compile_opts, match_opts);
  cut_assert (!g_regex_match_full (regex, string, string_len,
                                   start_position, match_opts2, NULL, NULL),
              cut_message ("/%s/ !~ <%s>", pattern, string));
  if (string_len == -1 && start_position == 0)
    {
      cut_assert (!g_regex_match (regex, string, match_opts2, NULL),
	          cut_message ("/%s/ !~ <%s>", pattern, string));
    }
  free_regex ();
}

void
test_mismatch (void)
{
  cut_assert_mismatch ("a", 0, 0, "A", -1, 0, 0);
  cut_assert_mismatch ("a", 0, 0, "ab", -1, 1, 0);
  cut_assert_mismatch ("a", 0, 0, "ba", 1, 0, 0);
  cut_assert_mismatch ("a", 0, 0, "b", -1, 0, 0);
  cut_assert_mismatch ("a", 0, G_REGEX_ANCHORED, "ab", -1, 1, 0);
  cut_assert_mismatch ("a", 0, G_REGEX_ANCHORED, "ba", 1, 0, 0);
  cut_assert_mismatch ("a", 0, G_REGEX_ANCHORED, "bab", -1, 0, 0);
  cut_assert_mismatch ("a", 0, G_REGEX_ANCHORED, "b", -1, 0, 0);
  cut_assert_mismatch ("a", 0, 0, "ab", -1, 1, G_REGEX_ANCHORED);
  cut_assert_mismatch ("a", 0, 0, "ba", 1, 0, G_REGEX_ANCHORED);
  cut_assert_mismatch ("a", 0, 0, "bab", -1, 0, G_REGEX_ANCHORED);
  cut_assert_mismatch ("a", 0, 0, "b", -1, 0, G_REGEX_ANCHORED);
  cut_assert_mismatch ("\\d", 0, 0, EURO, -1, 0, 0);
  cut_assert_mismatch ("^.{3}$", 0, 0, EURO, -1, 0, 0);
  cut_assert_mismatch ("^.$", G_REGEX_RAW, 0, EURO, -1, 0, 0);

  /* New lines handling. */
  cut_assert_mismatch ("^a\\Rb$", 0, 0, "a\n\rb", -1, 0, 0);
  cut_assert_mismatch ("^a\\nb$", 0, 0, "a\r\nb", -1, 0, 0);

  cut_assert_mismatch ("^b$", 0, 0, "a\nb\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, "a\nb\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CRLF, 0, "a\nb\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_LF, 0, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_LF, 0, "a\rb\rc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CRLF, 0, "a\rb\rc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CR, "a\nb\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CRLF, "a\nb\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CR, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_LF, "a\r\nb\r\nc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_LF, "a\rb\rc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE, G_REGEX_MATCH_NEWLINE_CRLF, "a\rb\rc", -1, 0, 0);

  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_LF, "a\rb\rc", -1, 0, 0);
  cut_assert_mismatch ("^b$", G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, G_REGEX_MATCH_NEWLINE_CRLF, "a\rb\rc", -1, 0, 0);

  cut_assert_mismatch ("a#\nb", G_REGEX_EXTENDED, 0, "a", -1, 0, 0);
  cut_assert_mismatch ("a#\r\nb", G_REGEX_EXTENDED, 0, "a", -1, 0, 0);
  cut_assert_mismatch ("a#\rb", G_REGEX_EXTENDED, 0, "a", -1, 0, 0);
  cut_assert_mismatch ("a#\nb", G_REGEX_EXTENDED, G_REGEX_MATCH_NEWLINE_CR, "a", -1, 0, 0);
}

static void
cut_assert_match_count (const gchar     *pattern,
			const gchar     *string,
			gint             start_position,
                  	GRegexMatchFlags match_opts,
			gint             expected_count)
{
  cut_assert_regex_new_without_free (pattern, 0, 0);
  if (expected_count == 0)
    cut_assert (!g_regex_match_full (regex, string, -1, start_position,
                                     match_opts, &match_info, NULL),
                cut_message ("/%s/ !~ <%s>", pattern, string));
  else
    cut_assert (g_regex_match_full (regex, string, -1, start_position,
                                    match_opts, &match_info, NULL),
                cut_message ("/%s/ =~ <%s>", pattern, string));
  cut_assert_equal_int (expected_count,
                        g_match_info_get_match_count (match_info));
  free_regex ();
  free_match_info ();
}

void
test_match_count (void)
{
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
  cut_assert_regex_new_without_free (pattern, 0, 0);
  cut_assert_equal_int (expected_num, g_regex_get_string_number (regex, name));
  free_regex ();
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
  escaped_string = g_regex_escape_string (string, length);
  cut_assert_equal_string (expected, escaped_string,
                           cut_message ("<%s> -> <%s>", string, escaped_string));
  free_escaped_string ();
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

static void
cut_assert_replace (const gchar *pattern,
		    const gchar *string,
		    gint         start_position,
		    const gchar *replacement,
		    const gchar *expected)
{
  cut_assert_regex_new_without_free (pattern, 0, 0);
  replaced_string = g_regex_replace (regex, string, -1, start_position,
                                     replacement, 0, NULL);
  cut_assert_equal_string_or_null (expected, replaced_string);
  free_regex ();
  free_replaced_string ();
}

void
test_replace (void)
{
  cut_assert_replace ("a", "ababa", 0, "A", "AbAbA");
  cut_assert_replace ("a", "ababa", 1, "A", "abAbA");
  cut_assert_replace ("a", "ababa", 2, "A", "abAbA");
  cut_assert_replace ("a", "ababa", 3, "A", "ababA");
  cut_assert_replace ("a", "ababa", 4, "A", "ababA");
  cut_assert_replace ("a", "ababa", 5, "A", "ababa");
  cut_assert_replace ("a", "ababa", 6, "A", "ababa");
  cut_assert_replace ("a", "abababa", 2, "A", "abAbAbA");
  cut_assert_replace ("a", "abab", 0, "A", "AbAb");
  cut_assert_replace ("a", "baba", 0, "A", "bAbA");
  cut_assert_replace ("a", "bab", 0, "A", "bAb");
  cut_assert_replace ("$^", "abc", 0, "X", "abc");
  cut_assert_replace ("(.)a", "ciao", 0, "a\\1", "caio");
  cut_assert_replace ("a.", "abc", 0, "\\0\\0", "ababc");
  cut_assert_replace ("a", "asd", 0, "\\0101", "Asd");
  cut_assert_replace ("(a).\\1", "aba cda", 0, "\\1\\n", "a\n cda");
  cut_assert_replace ("a" AGRAVE "a", "a" AGRAVE "a", 0, "x", "x");
  cut_assert_replace ("a" AGRAVE "a", "a" AGRAVE "a", 0, OGRAVE, OGRAVE);
  cut_assert_replace ("[^-]", "-" EURO "-x-" HSTROKE, 0, "a", "-a-a-a");
  cut_assert_replace ("[^-]", "-" EURO "-" HSTROKE, 0, "a\\g<0>a", "-a" EURO "a-a" HSTROKE "a");
  cut_assert_replace ("-", "-" EURO "-" HSTROKE, 0, "", EURO HSTROKE);
  cut_assert_replace (".*", "hello", 0, "\\U\\0\\E", "HELLO");
  cut_assert_replace (".*", "hello", 0, "\\u\\0", "Hello");
  cut_assert_replace ("\\S+", "hello world", 0, "\\U-\\0-", "-HELLO- -WORLD-");
  cut_assert_replace (".", "a", 0, "\\A", NULL);
  cut_assert_replace (".", "a", 0, "\\g", NULL);
}

static void
cut_assert_partial_match (const gchar *pattern,
			  const gchar *string)
{
  cut_assert_regex_new_without_free (pattern, 0, 0);
  g_regex_match (regex, string, G_REGEX_MATCH_PARTIAL, &match_info);
  cut_assert (!g_match_info_fetch_pos (match_info, 0, NULL, NULL),
              cut_message ("/%s/ =~ <%s>", pattern, string));
  cut_assert (!g_match_info_fetch_pos (match_info, 1, NULL, NULL),
              cut_message ("/%s/ =~ <%s>", pattern, string));

  free_regex ();
  free_match_info ();
}

static void
cut_assert_partial_mismatch (const gchar *pattern,
			     const gchar *string)
{
  cut_assert_regex_new_without_free (pattern, 0, 0);
  g_regex_match (regex, string, G_REGEX_MATCH_PARTIAL, &match_info);
  cut_assert (!g_match_info_is_partial_match (match_info),
              cut_message ("/%s/ =~ <%s>", pattern, string));

  free_regex ();
  free_match_info ();
}

void
test_partial_match (void)
{
  cut_assert_partial_match ("^ab", "a");
  cut_assert_partial_match ("ab", "xa");
  cut_assert_partial_match ("(a)+b", "aa");
  cut_assert_partial_match ("a?b", "a");
  cut_assert_partial_mismatch ("^ab", "xa");
  cut_assert_partial_mismatch ("ab", "ab"); /* normal match. */
  cut_assert_partial_mismatch ("a+b", "aa"); /* PCRE_ERROR_BAD_PARTIAL */
}

static void
cut_assert_replacement (const gchar *string_to_expand,
			gboolean     expected_refs)
{
  gboolean has_refs;
  cut_assert (g_regex_check_replacement (string_to_expand, &has_refs, NULL));
  cut_assert_equal_int (expected_refs, has_refs);
}

void
test_check_replacement (void)
{
  cut_assert_replacement ("", FALSE);
  cut_assert_replacement ("a", FALSE);
  cut_assert_replacement ("\\t\\n\\v\\r\\f\\a\\b\\\\\\x{61}", FALSE);
  cut_assert_replacement ("\\0", TRUE);
  cut_assert_replacement ("\\n\\2", TRUE);
  cut_assert_replacement ("\\g<foo>", TRUE);

  /* Invalid strings */
  cut_assert (!g_regex_check_replacement ("\\Q", NULL, NULL));
  cut_assert (!g_regex_check_replacement ("x\\Ay", NULL, NULL));
}

static void
cut_assert_replace_lit (const gchar *pattern,
			const gchar *string,
			gint         start_position,
			const gchar *replacement,
			const gchar *expected)
{
  cut_assert_regex_new_without_free (pattern, 0, 0);
  replaced_string = g_regex_replace_literal (regex, string, -1, start_position,
                                             replacement, 0, NULL);
  cut_assert_equal_string (expected, replaced_string);

  free_regex ();
  free_replaced_string ();
}

void
test_replace_lit (void)
{
  cut_assert_replace_lit ("a", "ababa", 0, "A", "AbAbA");
  cut_assert_replace_lit ("a", "ababa", 1, "A", "abAbA");
  cut_assert_replace_lit ("a", "ababa", 2, "A", "abAbA");
  cut_assert_replace_lit ("a", "ababa", 3, "A", "ababA");
  cut_assert_replace_lit ("a", "ababa", 4, "A", "ababA");
  cut_assert_replace_lit ("a", "ababa", 5, "A", "ababa");
  cut_assert_replace_lit ("a", "ababa", 6, "A", "ababa");
  cut_assert_replace_lit ("a", "abababa", 2, "A", "abAbAbA");
  cut_assert_replace_lit ("a", "abcadaa", 0, "A", "AbcAdAA");
  cut_assert_replace_lit ("$^", "abc", 0, "X", "abc");
  cut_assert_replace_lit ("(.)a", "ciao", 0, "a\\1", "ca\\1o");
  cut_assert_replace_lit ("a.", "abc", 0, "\\0\\0\\n", "\\0\\0\\nc");
  cut_assert_replace_lit ("a" AGRAVE "a", "a" AGRAVE "a", 0, "x", "x");
  cut_assert_replace_lit ("a" AGRAVE "a", "a" AGRAVE "a", 0, OGRAVE, OGRAVE);
  cut_assert_replace_lit (AGRAVE, "-" AGRAVE "-" HSTROKE, 0, "a" ENG "a", "-a" ENG "a-" HSTROKE);
  cut_assert_replace_lit ("[^-]", "-" EURO "-" AGRAVE "-" HSTROKE, 0, "a", "-a-a-a");
  cut_assert_replace_lit ("[^-]", "-" EURO "-" AGRAVE, 0, "a\\g<0>a", "-a\\g<0>a-a\\g<0>a");
  cut_assert_replace_lit ("-", "-" EURO "-" AGRAVE "-" HSTROKE, 0, "", EURO AGRAVE HSTROKE);
}

static void
cut_assert_expand (const gchar *pattern,
		   const gchar *string,
		   const gchar *string_to_expand,
		   gboolean     raw,
		   const gchar *expected)
{
  cut_assert_regex_new_without_free (pattern, raw ? G_REGEX_RAW : 0, 0);

  g_regex_match (regex, string, 0, &match_info);
  expanded_string = g_match_info_expand_references (match_info,
                                                    string_to_expand, NULL);
  cut_assert_equal_string_or_null (expected, expanded_string);

  free_regex ();
  free_match_info ();
  free_expanded_string ();
}

static void
cut_assert_expand_null (const gchar *string_to_expand,
			const gchar *expected)
{
  expanded_string = g_match_info_expand_references (NULL, string_to_expand,
                                                    NULL);
  cut_assert_equal_string (expected, expanded_string);
  free_expanded_string ();
}

void
test_expand (void)
{
  /* TEST_EXPAND(pattern, string, string_to_expand, raw, expected) */
  cut_assert_expand ("a", "a", "", FALSE, "");
  cut_assert_expand ("a", "a", "\\0", FALSE, "a");
  cut_assert_expand ("a", "a", "\\1", FALSE, "");
  cut_assert_expand ("(a)", "ab", "\\1", FALSE, "a");
  cut_assert_expand ("(a)", "a", "\\1", FALSE, "a");
  cut_assert_expand ("(a)", "a", "\\g<1>", FALSE, "a");
  cut_assert_expand ("a", "a", "\\0130", FALSE, "X");
  cut_assert_expand ("a", "a", "\\\\\\0", FALSE, "\\a");
  cut_assert_expand ("a(?P<G>.)c", "xabcy", "X\\g<G>X", FALSE, "XbX");
  cut_assert_expand ("(.)(?P<1>.)", "ab", "\\1", FALSE, "a");
  cut_assert_expand ("(.)(?P<1>.)", "ab", "\\g<1>", FALSE, "a");
  cut_assert_expand (".", EURO, "\\0", FALSE, EURO);
  cut_assert_expand ("(.)", EURO, "\\1", FALSE, EURO);
  cut_assert_expand ("(?P<G>.)", EURO, "\\g<G>", FALSE, EURO);
  cut_assert_expand (".", "a", EURO, FALSE, EURO);
  cut_assert_expand (".", "a", EURO "\\0", FALSE, EURO "a");
  cut_assert_expand (".", "", "\\Lab\\Ec", FALSE, "abc");
  cut_assert_expand (".", "", "\\LaB\\EC", FALSE, "abC");
  cut_assert_expand (".", "", "\\Uab\\Ec", FALSE, "ABc");
  cut_assert_expand (".", "", "a\\ubc", FALSE, "aBc");
  cut_assert_expand (".", "", "a\\lbc", FALSE, "abc");
  cut_assert_expand (".", "", "A\\uBC", FALSE, "ABC");
  cut_assert_expand (".", "", "A\\lBC", FALSE, "AbC");
  cut_assert_expand (".", "", "A\\l\\\\BC", FALSE, "A\\BC");
  cut_assert_expand (".", "", "\\L" AGRAVE "\\E", FALSE, AGRAVE);
  cut_assert_expand (".", "", "\\U" AGRAVE "\\E", FALSE, AGRAVE_UPPER);
  cut_assert_expand (".", "", "\\u" AGRAVE "a", FALSE, AGRAVE_UPPER "a");
  cut_assert_expand (".", "ab", "x\\U\\0y\\Ez", FALSE, "xAYz");
  cut_assert_expand (".(.)", "AB", "x\\L\\1y\\Ez", FALSE, "xbyz");
  cut_assert_expand (".", "ab", "x\\u\\0y\\Ez", FALSE, "xAyz");
  cut_assert_expand (".(.)", "AB", "x\\l\\1y\\Ez", FALSE, "xbyz");
  cut_assert_expand (".(.)", "a" AGRAVE_UPPER, "x\\l\\1y", FALSE, "x" AGRAVE "y");
  cut_assert_expand ("a", "bab", "\\x{61}", FALSE, "a");
  cut_assert_expand ("a", "bab", "\\x61", FALSE, "a");
  cut_assert_expand ("a", "bab", "\\x5a", FALSE, "Z");
  cut_assert_expand ("a", "bab", "\\0\\x5A", FALSE, "aZ");
  cut_assert_expand ("a", "bab", "\\1\\x{5A}", FALSE, "Z");
  cut_assert_expand ("a", "bab", "\\x{00E0}", FALSE, AGRAVE);
  cut_assert_expand ("", "bab", "\\x{0634}", FALSE, SHEEN);
  cut_assert_expand ("", "bab", "\\x{634}", FALSE, SHEEN);
  cut_assert_expand ("", "", "\\t", FALSE, "\t");
  cut_assert_expand ("", "", "\\v", FALSE, "\v");
  cut_assert_expand ("", "", "\\r", FALSE, "\r");
  cut_assert_expand ("", "", "\\n", FALSE, "\n");
  cut_assert_expand ("", "", "\\f", FALSE, "\f");
  cut_assert_expand ("", "", "\\a", FALSE, "\a");
  cut_assert_expand ("", "", "\\b", FALSE, "\b");
  cut_assert_expand ("a(.)", "abc", "\\0\\b\\1", FALSE, "ab\bb");
  cut_assert_expand ("a(.)", "abc", "\\0141", FALSE, "a");
  cut_assert_expand ("a(.)", "abc", "\\078", FALSE, "\a8");
  cut_assert_expand ("a(.)", "abc", "\\077", FALSE, "?");
  cut_assert_expand ("a(.)", "abc", "\\0778", FALSE, "?8");
  cut_assert_expand ("a(.)", "a" AGRAVE "b", "\\1", FALSE, AGRAVE);
  cut_assert_expand ("a(.)", "a" AGRAVE "b", "\\1", TRUE, "\xc3");
  cut_assert_expand ("a(.)", "a" AGRAVE "b", "\\0", TRUE, "a\xc3");

  /* Invalid strings. */
  cut_assert_expand ("", "", "\\Q", FALSE, NULL);
  cut_assert_expand ("", "", "x\\Ay", FALSE, NULL);
  cut_assert_expand ("", "", "\\g<", FALSE, NULL);
  cut_assert_expand ("", "", "\\g<>", FALSE, NULL);
  cut_assert_expand ("", "", "\\g<1a>", FALSE, NULL);
  cut_assert_expand ("", "", "\\g<a$>", FALSE, NULL);
  cut_assert_expand ("", "", "\\", FALSE, NULL);
  cut_assert_expand ("a", "a", "\\x{61", FALSE, NULL);
  cut_assert_expand ("a", "a", "\\x6X", FALSE, NULL);
  /* Pattern-less. */
  cut_assert_expand_null ("", "");
  cut_assert_expand_null ("\\n", "\n");
  /* Invalid strings */
  cut_assert_null (g_match_info_expand_references (NULL, "\\Q", NULL));
  cut_assert_null (g_match_info_expand_references (NULL, "x\\Ay", NULL));
}

static void
collect_expected_matches (va_list args)
{
  /* The va_list is a NULL-terminated sequence of: extected matched string,
   * expected start and expected end. */
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
      expected_matches = g_slist_prepend (expected_matches, match);
    }
  expected_matches = g_slist_reverse (expected_matches);
}

static void
collect_actual_matches (GRegex **regex, const gchar *pattern,
                        const gchar *string, gssize string_len,
                        gint start_position, GMatchInfo **match_info)
{
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
      actual_matches = g_slist_prepend (actual_matches, match);
      g_match_info_next (*match_info, NULL);
    }
  actual_matches = g_slist_reverse (actual_matches);
}

static void
cut_assert_match_next (const gchar *pattern,
                       const gchar *string,
                       gssize       string_length,
                       gint         start_position,
                       ...)
{
  GSList *actual_node, *expected_node;
  va_list args;

  va_start (args, start_position);
  collect_expected_matches (args);
  va_end (args);

  collect_actual_matches (&regex, pattern,
                          string, string_length,
                          start_position, &match_info);
  cut_assert (regex == g_match_info_get_regex (match_info));
  cut_assert_equal_string (string, g_match_info_get_string (match_info));
  free_match_info ();

  cut_assert_equal_int(g_slist_length (expected_matches),
                       g_slist_length (actual_matches));

  expected_node = expected_matches;
  actual_node = actual_matches;
  while (expected_node)
    {
      Match *exp = expected_node->data;
      Match *act = actual_node->data;

      cut_assert_equal_string_or_null (exp->string, act->string);

      expected_node = g_slist_next (expected_node);
      actual_node = g_slist_next (actual_node);
    }

  free_regex ();
  free_expected_matches ();
  free_actual_matches ();
}

void
test_match_next (void)
{
  /* TEST_MATCH_NEXT#(pattern, string, string_len, start_position, ...) */
  cut_assert_match_next ("a", "x", -1, 0, NULL);
  cut_assert_match_next ("a", "ax", -1, 1, NULL);
  cut_assert_match_next ("a", "xa", 1, 0, NULL);
  cut_assert_match_next ("a", "axa", 1, 2, NULL);
  cut_assert_match_next ("a", "a", -1, 0, "a", 0, 1, NULL);
  cut_assert_match_next ("a", "xax", -1, 0, "a", 1, 2, NULL);
  cut_assert_match_next (EURO, ENG EURO, -1, 0, EURO, 2, 5, NULL);
  cut_assert_match_next ("a*", "", -1, 0, "", 0, 0, NULL);
  cut_assert_match_next ("a*", "aa", -1, 0, "aa", 0, 2, "", 2, 2, NULL);
  cut_assert_match_next (EURO "*", EURO EURO, -1, 0, EURO EURO, 0, 6, "", 6, 6, NULL);
  cut_assert_match_next ("a", "axa", -1, 0, "a", 0, 1, "a", 2, 3, NULL);
  cut_assert_match_next ("a+", "aaxa", -1, 0, "aa", 0, 2, "a", 3, 4, NULL);
  cut_assert_match_next ("a", "aa", -1, 0, "a", 0, 1, "a", 1, 2, NULL);
  cut_assert_match_next ("a", "ababa", -1, 2, "a", 2, 3, "a", 4, 5, NULL);
  cut_assert_match_next (EURO "+", EURO "-" EURO, -1, 0, EURO, 0, 3, EURO, 4, 7, NULL);
  cut_assert_match_next ("", "ab", -1, 0, "", 0, 0, "", 1, 1, "", 2, 2, NULL);
  cut_assert_match_next ("", AGRAVE "b", -1, 0, "", 0, 0, "", 2, 2, "", 3, 3, NULL);
  cut_assert_match_next ("a", "aaxa", -1, 0, "a", 0, 1, "a", 1, 2, "a", 3, 4, NULL);
  cut_assert_match_next ("a", "aa" OGRAVE "a", -1, 0, "a", 0, 1, "a", 1, 2, "a", 4, 5, NULL);
  cut_assert_match_next ("a*", "aax", -1, 0, "aa", 0, 2, "", 2, 2, "", 3, 3, NULL);
  cut_assert_match_next ("a*", "aaxa", -1, 0, "aa", 0, 2, "", 2, 2, "a", 3, 4, "", 4, 4, NULL);

}

static void
cut_assert_sub_pattern (const gchar *pattern,
			const gchar *string,
			gint         start_position,
			gint         sub_n,
			const gchar *expected_sub,
			gint         expected_start,
			gint         expected_end)
{
  gint start = UNTOUCHED, end = UNTOUCHED;
  cut_assert_regex_new_without_free (pattern, 0, 0);
  cut_assert (g_regex_match_full (regex, string, -1, start_position, 0,
                                  &match_info, NULL),
              cut_message ("/%s/ =~ <%s>", pattern, string));
  substring = g_match_info_fetch (match_info, sub_n);
  cut_assert_equal_string_or_null (expected_sub, substring);
  free_substring ();

  if (expected_sub)
    cut_assert (g_match_info_fetch_pos (match_info, sub_n, &start, &end),
                cut_message ("$%d", sub_n));
  else
    cut_assert (!g_match_info_fetch_pos (match_info, sub_n, &start, &end),
                cut_message ("!$%d", sub_n));
  cut_assert_equal_int (expected_start, start); /* FIXME */
  cut_assert_equal_int (expected_end, end); /* assert_equal_int_array() */

  free_regex ();
  free_match_info ();
}

void
test_sub_pattern (void)
{
  /* TEST_SUB_PATTERN(pattern, string, start_position, sub_n, expected_sub,
   * 		      expected_start, expected_end) */
  cut_assert_sub_pattern ("a", "a", 0, 0, "a", 0, 1);
  cut_assert_sub_pattern ("a(.)", "ab", 0, 1, "b", 1, 2);
  cut_assert_sub_pattern ("a(.)", "a" EURO, 0, 1, EURO, 1, 4);
  cut_assert_sub_pattern ("(?:.*)(a)(.)", "xxa" ENG, 0, 2, ENG, 3, 5);
  cut_assert_sub_pattern ("(" HSTROKE ")", "a" HSTROKE ENG, 0, 1, HSTROKE, 1, 3);
  cut_assert_sub_pattern ("a", "a", 0, 1, NULL, UNTOUCHED, UNTOUCHED);
  cut_assert_sub_pattern ("(a)?(b)", "b", 0, 0, "b", 0, 1);
  cut_assert_sub_pattern ("(a)?(b)", "b", 0, 1, "", -1, -1);
  cut_assert_sub_pattern ("(a)?(b)", "b", 0, 2, "b", 0, 1);
}

#define TEST_NAMED_SUB_PATTERN(pattern, string, start_position,         \
                               sub_name, expected_sub,                  \
                               expected_start, expected_end) {          \
  gint start = UNTOUCHED, end = UNTOUCHED;                              \
  gchar *sub_expr;                                                      \
  regex = g_regex_new (pattern, 0, 0, NULL);                            \
  g_regex_match_full (regex, string, -1, start_position, 0,             \
                      &match_info, NULL);                               \
  sub_expr = g_match_info_fetch_named (match_info, sub_name);           \
  cut_assert_equal_string (expected_sub, sub_expr);                     \
  g_free (sub_expr);                                                    \
  g_match_info_fetch_named_pos (match_info, sub_name, &start, &end);    \
  cut_assert (start == expected_start && end == expected_end,           \
              cut_message ("failed (got [%d, %d], expected [%d, %d])",  \
                           start, end, expected_start, expected_end));  \
  g_regex_unref (regex);                                                \
  g_match_info_free (match_info);                                       \
  regex = NULL;                                                         \
  match_info = NULL;                                                    \
}

#define TEST_NAMED_SUB_PATTERN_NULL(pattern, string, start_position,    \
                                    sub_name,                           \
                                    expected_start, expected_end) {     \
  gint start = UNTOUCHED, end = UNTOUCHED;                              \
  gchar *sub_expr;                                                      \
  regex = g_regex_new (pattern, 0, 0, NULL);                            \
  g_regex_match_full (regex, string, -1, start_position, 0,             \
                      &match_info, NULL);                               \
  sub_expr = g_match_info_fetch_named (match_info, sub_name);           \
  cut_assert_null (sub_expr);                                           \
  g_match_info_fetch_named_pos (match_info, sub_name, &start, &end);    \
  cut_assert (start == expected_start && end == expected_end,           \
              cut_message ("failed (got [%d, %d], expected [%d, %d])",  \
                           start, end, expected_start, expected_end));  \
  g_regex_unref (regex);                                                \
  g_match_info_free (match_info);                                       \
  regex = NULL;                                                         \
  match_info = NULL;                                                    \
}

#define TEST_NAMED_SUB_PATTERN_DUPNAMES(pattern, string,                \
                                        start_position, sub_name,       \
					expected_sub,                   \
                                        expected_start, expected_end) { \
  gint start = UNTOUCHED, end = UNTOUCHED;                              \
  gchar *sub_expr;                                                      \
  regex = g_regex_new (pattern, G_REGEX_DUPNAMES, 0, NULL);             \
  g_regex_match_full (regex, string, -1, start_position, 0, \
                      &match_info, NULL);                               \
  sub_expr = g_match_info_fetch_named (match_info, sub_name);           \
  cut_assert_equal_string (expected_sub, sub_expr);                     \
  g_free (sub_expr);                                                    \
  g_match_info_fetch_named_pos (match_info, sub_name, &start, &end);    \
  cut_assert (start == expected_start && end == expected_end,           \
              cut_message ("failed (got [%d, %d], expected [%d, %d])",  \
                           start, end, expected_start, expected_end));  \
  g_regex_unref (regex);                                                \
  g_match_info_free (match_info);                                       \
  regex = NULL;                                                         \
  match_info = NULL;                                                    \
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

static void
cut_assert_match_all_each (const gchar *pattern,
                           gboolean     use_full,
                           const gchar *string,
                           gssize       string_len,
                           gint         start_position,
                           va_list      args)
{
  GSList *expected_node;
  gint expected_length;
  gint match_count;
  gint i;

  collect_expected_matches (args);
  expected_length = g_slist_length (expected_matches);

  regex = g_regex_new (pattern, 0, 0, NULL);
  if (expected_length == 0)
    {
      if (use_full)
        cut_assert (!g_regex_match_all_full (regex, string, string_len,
                                             start_position, 0, &match_info,
                                             NULL),
                    cut_message ("/%s/ !~ <%s>", pattern, string));
      else
        cut_assert (!g_regex_match_all (regex, string, 0, &match_info),
                    cut_message ("/%s/ !~ <%s>", pattern, string));
    }
  else
    {
      if (use_full)
        cut_assert (g_regex_match_all_full (regex, string, string_len,
                                            start_position, 0, &match_info,
                                            NULL),
                    cut_message ("/%s/ =~ <%s>", pattern, string));
      else
        cut_assert (g_regex_match_all (regex, string, 0, &match_info),
                    cut_message ("/%s/ =~ <%s>", pattern, string));
    }

  match_count = g_match_info_get_match_count (match_info);
  cut_assert_equal_int (expected_length, match_count);

  expected_node = expected_matches;
  for (i = 0; i < match_count; i++)
    {
      gint start, end;
      gchar *matched_string;
      Match *expected_match = expected_node->data;

      matched_string = g_match_info_fetch (match_info, i);
      cut_assert_equal_string (expected_match->string, matched_string,
                               cut_message ("/%s/ =~ <%s>",
                                            pattern, expected_match->string));
      g_free (matched_string);

      cut_assert (g_match_info_fetch_pos (match_info, i, &start, &end));
      cut_assert_equal_int (expected_match->start, start,
                            cut_message ("/%s/ =~ <%s>",
                                         pattern, expected_match->string));
      cut_assert_equal_int (expected_match->end, end,
                            cut_message ("/%s/ =~ <%s>",
                                         pattern, expected_match->string));

      expected_node = g_slist_next (expected_node);
    }

  free_regex ();
  free_match_info ();
  free_expected_matches ();
}

static void
cut_assert_match_all (const gchar *pattern,
                      const gchar *string,
                      gssize string_length,
                      gint start_position,
                      ...)
{
  va_list args, copied_args;

  va_start (args, start_position);

  va_copy (copied_args, args);
  cut_assert_match_all_each (pattern, TRUE,
                             string, string_length, start_position,
                             copied_args);
  va_end(copied_args);

  if (string_length == -1 && start_position == 0)
    {
      va_copy (copied_args, args);
      cut_assert_match_all_each (pattern, FALSE,
                                 string, string_length, start_position,
                                 copied_args);
      va_end(copied_args);
    }

  va_end (args);
}

void
test_match_all (void)
{
  cut_assert_match_all ("<.*>", "", -1, 0, NULL);
  cut_assert_match_all ("a+", "", -1, 0, NULL);
  cut_assert_match_all ("a+", "a", 0, 0, NULL);
  cut_assert_match_all ("a+", "a", -1, 1, NULL);
  cut_assert_match_all ("<.*>", "<a>", -1, 0, "<a>", 0, 3, NULL);
  cut_assert_match_all ("a+", "a", -1, 0, "a", 0, 1, NULL);
  cut_assert_match_all ("a+", "aa", 1, 0, "a", 0, 1, NULL);
  cut_assert_match_all ("a+", "aa", -1, 1, "a", 1, 2, NULL);
  cut_assert_match_all ("a+", "aa", 2, 1, "a", 1, 2, NULL);
  cut_assert_match_all (".+", ENG, -1, 0, ENG, 0, 2, NULL);
  cut_assert_match_all ("<.*>", "<a><b>", -1, 0, "<a><b>", 0, 6,
                        "<a>", 0, 3, NULL);
  cut_assert_match_all ("a+", "aa", -1, 0, "aa", 0, 2, "a", 0, 1, NULL);
  cut_assert_match_all (".+", ENG EURO, -1, 0, ENG EURO, 0, 5, ENG, 0, 2, NULL);
  cut_assert_match_all ("<.*>", "<a><b><c>", -1, 0, "<a><b><c>", 0, 9,
                        "<a><b>", 0, 6, "<a>", 0, 3, NULL);
  cut_assert_match_all ("a+", "aaa", -1, 0, "aaa", 0, 3, "aa", 0, 2,
                        "a", 0, 1, NULL);
}
