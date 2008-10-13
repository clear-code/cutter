/* -*- c-file-style: "gnu" -*- */

#include <gcutter.h>

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <glib.h>

static gboolean error_test_pre_parse_fail = FALSE;
static gboolean error_test_post_parse_fail = FALSE;

static int callback_test2_int;

static gchar *callback_test_optional_string;
static gboolean callback_test_optional_boolean;

static gboolean test_boolean;
static gint test_int;
static gint64 test_int64;
static gint64 test_int64_2;
static gdouble test_double;
static gchar *test_string;
static gchar **test_array;

static GOptionContext *context = NULL;
static GError *error = NULL;
static gchar **argv = NULL;
static gint argc;

void test_parse_int_error (void);
void test_parse_string_error (void);
void test_parse_boolean_error (void);
void test_arg_int (void);
void test_arg_string (void);
void test_arg_string_filename (void);
void test_arg_double (void);
void test_arg_double_de_DE_locale (void);
void test_arg_int64 (void);
void test_callback1 (void);
void test_callback_int (void);
void test_add (void);
void test_arg_array (void);
void test_ignore1 (void);
void test_ignore2 (void);
void test_ignore3 (void);
void test_empty (void);
void test_empty_argv_and_argc (void);
void test_empty_argv (void);
void test_unknown_short (void);
void test_lonely_dash (void);
void test_missing_arg (void);
void test_callback_optional_1 (void);
void test_callback_optional_2 (void);
void test_callback_optional_3 (void);
void test_callback_optional_4 (void);
void test_callback_optional_5 (void);
void test_callback_optional_6 (void);
void test_callback_optional_7 (void);
void test_callback_optional_8 (void);
void test_callback_remaining (void);
void test_rest1 (void);
void test_rest2 (void);
void test_rest2a (void);
void test_rest2b (void);
void test_rest2c (void);
void test_rest2d (void);
void test_rest3 (void);
void test_rest4 (void);
void test_rest5 (void);

void
setup (void)
{
  context = g_option_context_new (NULL);
  argv = NULL;
  error = NULL;
  test_string = NULL;
  callback_test2_int = 0;
}

void
teardown (void)
{
  if (argv)
    g_strfreev (argv);
  if (context)
    g_option_context_free (context);
  if (error)
    g_error_free (error);
  if (test_string)
    g_free (test_string);
}

static gchar **
split_string (const char *str, int *argc)
{
  gchar **strings;
  int len;
  
  strings = g_strsplit (str, " ", 0);

  for (len = 0; strings[len] != NULL; len++);

  if (argc)
    *argc = len;
    
  return strings;
}

static gchar *
join_stringv (int argc, char **strings)
{
  int i;
  GString *str;

  str = g_string_new (NULL);

  for (i = 0; i < argc; i++)
    {
      g_string_append (str, strings[i]);

      if (i < argc - 1)
	g_string_append_c (str, ' ');
    }

  return g_string_free (str, FALSE);
}

/* Performs a shallow copy */
static char **
copy_stringv (char **strings, int argc)
{
  return g_memdup (strings, sizeof (char *) * (argc + 1));
}


static gboolean
error_pre_parse_int (GOptionContext *context,
		     GOptionGroup   *group,
		     gpointer	     data,
		     GError        **error)
{
  if (0x12345678 != test_int)
    error_test_pre_parse_fail = TRUE;

  return TRUE;
}

static gboolean
error_post_parse_int (GOptionContext *context,
		      GOptionGroup   *group,
		      gpointer	      data,
		      GError        **error)
{
  if (20 != test_int)
    error_test_post_parse_fail = TRUE;

  /* Set an error in the post hook */
  g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "");

  return FALSE;
}

void
test_parse_int_error (void)
{
  GOptionGroup *main_group;
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_INT, &test_int, NULL, NULL },
      { NULL } };
  test_int = 0x12345678;

  error_test_pre_parse_fail = FALSE;
  error_test_post_parse_fail = FALSE;
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Set pre and post parse hooks */
  main_group = g_option_context_get_main_group (context);
  g_option_group_set_parse_hooks (main_group,
				  error_pre_parse_int, error_post_parse_int);
  
  /* Now try parsing */
  argv = split_string ("program --test 20", &argc);

  cut_assert (!g_option_context_parse (context, &argc, &argv, &error));
  cut_assert (!error_test_pre_parse_fail);
  cut_assert (!error_test_post_parse_fail);

  /* On failure, values should be reset */
  cut_assert_equal_int (0x12345678, test_int);
}

static gboolean
error_test2_pre_parse (GOptionContext *context,
		       GOptionGroup   *group,
		       gpointer	  data,
		       GError        **error)
{
  if (strcmp (test_string, "foo") != 0)
    error_test_pre_parse_fail = TRUE;

  return TRUE;
}

static gboolean
error_test2_post_parse (GOptionContext *context,
			GOptionGroup   *group,
			gpointer	  data,
			GError        **error)
{
  if (strcmp (test_string, "bar") != 0)
    error_test_post_parse_fail = TRUE;

  /* Set an error in the post hook */
  g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "");

  return FALSE;
}

void
test_parse_string_error (void)
{
  GOptionGroup *main_group;
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_STRING, &test_string, NULL, NULL },
      { NULL } };
  test_string = g_strdup ("foo");
  error_test_pre_parse_fail = FALSE;
  error_test_post_parse_fail = FALSE;

  g_option_context_add_main_entries (context, entries, NULL);

  /* Set pre and post parse hooks */
  main_group = g_option_context_get_main_group (context);
  g_option_group_set_parse_hooks (main_group,
				  error_test2_pre_parse, error_test2_post_parse);
  
  /* Now try parsing */
  argv = split_string ("program --test bar", &argc);
  cut_assert (!g_option_context_parse (context, &argc, &argv, &error));
  cut_assert (!error_test_pre_parse_fail);
  cut_assert (!error_test_post_parse_fail);

  cut_assert_equal_string ("foo", test_string);
}

static gboolean
error_test3_pre_parse (GOptionContext *context,
		       GOptionGroup   *group,
		       gpointer	  data,
		       GError        **error)
{
  if (test_boolean)
    error_test_pre_parse_fail = TRUE;

  return TRUE;
}

static gboolean
error_test3_post_parse (GOptionContext *context,
			GOptionGroup   *group,
			gpointer	  data,
			GError        **error)
{
  if (!test_boolean)
    error_test_post_parse_fail = TRUE;

  /* Set an error in the post hook */
  g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "");

  return FALSE;
}

void
test_parse_boolean_error (void)
{
  GOptionGroup *main_group;
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } };
  test_boolean = FALSE;
  error_test_pre_parse_fail = FALSE;
  error_test_post_parse_fail = FALSE;

  g_option_context_add_main_entries (context, entries, NULL);

  /* Set pre and post parse hooks */
  main_group = g_option_context_get_main_group (context);
  g_option_group_set_parse_hooks (main_group,
				  error_test3_pre_parse, error_test3_post_parse);
  
  /* Now try parsing */
  argv = split_string ("program --test", &argc);
  cut_assert (!g_option_context_parse (context, &argc, &argv, &error));
  cut_assert (!error_test_pre_parse_fail);
  cut_assert (!error_test_post_parse_fail);

  cut_assert (!test_boolean);
}

void
test_arg_int (void)
{
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_INT, &test_int, NULL, NULL },
      { NULL } };

  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test 20 --test 30", &argc);

  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Last arg specified is the one that should be stored */
  cut_assert_equal_int (30, test_int);
}

void
test_arg_string (void)
{
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_STRING, &test_string, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test foo --test bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Last arg specified is the one that should be stored */
  cut_assert_equal_string ("bar", test_string);
}

void
test_arg_string_filename (void)
{
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_FILENAME, &test_string, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test foo.txt", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Last arg specified is the one that should be stored */
  cut_assert_equal_string ("foo.txt", test_string);
}


void
test_arg_double (void)
{
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_DOUBLE, &test_double, NULL, NULL },
      { NULL } };

  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test 20.0 --test 30.03", &argc);

  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Last arg specified is the one that should be stored */
  cut_assert_equal_double (30.03, 0.0, test_double);
}

void
test_arg_double_de_DE_locale (void)
{
  char *old_locale, *current_locale;
  const char *locale = "de_DE";
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_DOUBLE, &test_double, NULL, NULL },
      { NULL } };

  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test 20,0 --test 30,03", &argc);

  /* set it to some locale that uses commas instead of decimal points */
  
  old_locale = g_strdup (setlocale (LC_NUMERIC, locale));
  current_locale = setlocale (LC_NUMERIC, NULL);
  if (strcmp (current_locale, locale) != 0)
    {
      cut_omit ("Cannot set locale to %s, skipping", locale);
      goto cleanup;
    }

  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Last arg specified is the one that should be stored */
  cut_assert_equal_double (30.03, 0.0, test_double);

 cleanup:
  setlocale (LC_NUMERIC, old_locale);
  g_free (old_locale);
}

void
test_arg_int64 (void)
{
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_INT64, &test_int64, NULL, NULL },
      { "test2", 0, 0, G_OPTION_ARG_INT64, &test_int64_2, NULL, NULL },
      { NULL } };

  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test 4294967297 --test 4294967296 --test2 0xfffffffff", &argc);

  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Last arg specified is the one that should be stored */
  gcut_assert_equal_int64 (G_GINT64_CONSTANT(4294967296), test_int64);
  gcut_assert_equal_int64 (G_GINT64_CONSTANT(0xfffffffff), test_int64_2);
}

static gboolean
callback_parse1 (const gchar *option_name, const gchar *value,
		 gpointer data, GError **error)
{
  test_string = g_strdup (value);
  return TRUE;
}

void
test_callback1 (void)
{
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_CALLBACK, callback_parse1, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test foo.txt", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_equal_string ("foo.txt", test_string);
}

static gboolean
callback_parse2 (const gchar *option_name, const gchar *value,
		 gpointer data, GError **error)
{
  callback_test2_int++;
  return TRUE;
}

void
test_callback_int (void)
{
  GOptionEntry entries [] =
    { { "test", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, callback_parse2, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test --test", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_equal_int (2, callback_test2_int);
}

static gboolean
callback_parse_optional (const gchar *option_name, const gchar *value,
		 gpointer data, GError **error)
{
  callback_test_optional_boolean = TRUE;
  if (value)
    callback_test_optional_string = g_strdup (value);
  else
    callback_test_optional_string = NULL;
  return TRUE;
}

void
test_callback_optional_1 (void)
{
  GOptionEntry entries [] =
    { { "test", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test foo.txt", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_equal_string ("foo.txt", callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}

void
test_callback_optional_2 (void)
{
  GOptionEntry entries [] =
    { { "test", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_null (callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}

void
test_callback_optional_3 (void)
{
  GOptionEntry entries [] =
    { { "test", 't', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program -t foo.txt", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_equal_string ("foo.txt", callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}


void
test_callback_optional_4 (void)
{
  GOptionEntry entries [] =
    { { "test", 't', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program -t", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_null (callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}

void
test_callback_optional_5 (void)
{
  gboolean dummy;
  GOptionEntry entries [] =
    { { "dummy", 'd', 0, G_OPTION_ARG_NONE, &dummy, NULL },
      { "test", 't', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test --dummy", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_null (callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}

void
test_callback_optional_6 (void)
{
  gboolean dummy;
  GOptionEntry entries [] =
    { { "dummy", 'd', 0, G_OPTION_ARG_NONE, &dummy, NULL },
      { "test", 't', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program -t -d", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_null (callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}

void
test_callback_optional_7 (void)
{
  gboolean dummy;
  GOptionEntry entries [] =
    { { "dummy", 'd', 0, G_OPTION_ARG_NONE, &dummy, NULL },
      { "test", 't', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program -td", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_null (callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}

void
test_callback_optional_8 (void)
{
  gboolean dummy;
  GOptionEntry entries [] =
    { { "dummy", 'd', 0, G_OPTION_ARG_NONE, &dummy, NULL },
      { "test", 't', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, 
	callback_parse_optional, NULL, NULL },
      { NULL } };
  
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program -dt foo.txt", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert (callback_test_optional_string);
  
  cut_assert (callback_test_optional_boolean);

  g_free (callback_test_optional_string);
}

static GPtrArray *callback_remaining_args;
static gboolean
callback_remaining_test1_callback (const gchar *option_name, const gchar *value,
		         gpointer data, GError **error)
{
  g_ptr_array_add (callback_remaining_args, g_strdup (value));
  return TRUE;
}

void
test_callback_remaining (void)
{
  GOptionEntry entries [] =
    { { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_CALLBACK, callback_remaining_test1_callback, NULL, NULL },
      { NULL } };
  
  callback_remaining_args = g_ptr_array_new ();
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo.txt blah.txt", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert_equal_int (2, callback_remaining_args->len);
  cut_assert_equal_string ("foo.txt", callback_remaining_args->pdata[0]);
  cut_assert_equal_string ("blah.txt", callback_remaining_args->pdata[1]);

  g_ptr_array_foreach (callback_remaining_args, (GFunc) g_free, NULL);
  g_ptr_array_free (callback_remaining_args, TRUE);
}

void
test_ignore1 (void)
{
  gchar **argv_copy;
  gchar *arg;
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } };

  g_option_context_set_ignore_unknown_options (context, TRUE);
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test --hello", &argc);
  argv_copy = copy_stringv (argv, argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  arg = join_stringv (argc, argv);
  cut_assert_equal_string ("program --hello", arg);

  g_free (arg);
  g_strfreev (argv_copy);
  g_free (argv);
  argv = NULL;
}

void
test_ignore2 (void)
{
  gchar *arg;
  GOptionEntry entries [] =
    { { "test", 't', 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } };

  g_option_context_set_ignore_unknown_options (context, TRUE);
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program -test", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  arg = join_stringv (argc, argv);
  cut_assert_equal_string ("program -es", arg);

  g_free (arg);
}

void
test_ignore3 (void)
{
  gchar **argv_copy;
  gchar *arg;
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_STRING, &test_string, NULL, NULL },
      { NULL } };

  g_option_context_set_ignore_unknown_options (context, TRUE);
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test foo --hello", &argc);
  argv_copy = copy_stringv (argv, argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  arg = join_stringv (argc, argv);
  cut_assert_equal_string ("program --hello", arg);

  cut_assert_equal_string ("foo", test_string);

  g_free (arg);
  g_strfreev (argv_copy);
  g_free (argv);
  argv = NULL;
}

void
test_arg_array (void)
{
  int argc;
  GOptionEntry entries [] =
    { { "test", 0, 0, G_OPTION_ARG_STRING_ARRAY, &test_array, NULL, NULL },
      { NULL } };
        
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test foo --test bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert_equal_string ("foo", test_array[0]);
  cut_assert_equal_string ("bar", test_array[1]);
  cut_assert_null (test_array[2]);

  g_strfreev (test_array);
}

void
test_add (void)
{
  GOptionEntry entries1 [] =
    { { "test1", 0, 0, G_OPTION_ARG_STRING_ARRAY, NULL, NULL, NULL },
      { NULL } };
  GOptionEntry entries2 [] =
    { { "test2", 0, 0, G_OPTION_ARG_STRING_ARRAY, NULL, NULL, NULL },
      { NULL } };

  g_option_context_add_main_entries (context, entries1, NULL);
  g_option_context_add_main_entries (context, entries2, NULL);
}

void
test_empty (void)
{
  GOptionEntry entries [] =
    { { NULL } };
  char *prgname;

  g_set_prgname (NULL);

  g_option_context_add_main_entries (context, entries, NULL);
  
  g_option_context_parse (context, NULL, NULL, NULL);

  prgname = g_get_prgname ();
  cut_assert (prgname);
  cut_assert_equal_string ("<unknown>", prgname);
}

void
test_empty_argv_and_argc (void)
{
  cut_assert (g_option_context_parse (context, NULL, NULL, NULL));
}

void
test_empty_argv (void)
{
  gint argc;

  argc = 0;
  argv = NULL;

  cut_assert (g_option_context_parse (context, &argc, &argv, NULL));
}

/* check that non-option arguments are left in argv by default */
void
test_rest1 (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } 
  };
        
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo --test bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("program", argv[0]);
  cut_assert_equal_string ("foo", argv[1]);
  cut_assert_equal_string ("bar", argv[2]);
  cut_assert_null (argv[3]);
}

/* check that -- works */
void
test_rest2 (void)
{
  gboolean retval;
  int argc;
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } 
  };
        
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo --test -- -bar", &argc);
  
  retval = g_option_context_parse (context, &argc, &argv, &error);
  cut_assert (retval);

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("program", argv[0]);
  cut_assert_equal_string ("foo", argv[1]);
  cut_assert_equal_string ("--", argv[2]);
  cut_assert_equal_string ("-bar", argv[3]);
  cut_assert_null (argv[4]);
}

/* check that -- stripping works */
void
test_rest2a (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } 
  };
        
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo --test -- bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("program", argv[0]);
  cut_assert_equal_string ("foo", argv[1]);
  cut_assert_equal_string ("bar", argv[2]);
  cut_assert_null (argv[3]);
}

void
test_rest2b (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } 
  };
        
  g_option_context_set_ignore_unknown_options (context, TRUE);
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo --test -bar --", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("program", argv[0]);
  cut_assert_equal_string ("foo", argv[1]);
  cut_assert_equal_string ("-bar", argv[2]);
  cut_assert_null (argv[3]);
}

void
test_rest2c (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } 
  };

  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test foo -- bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("program", argv[0]);
  cut_assert_equal_string ("foo", argv[1]);
  cut_assert_equal_string ("bar", argv[2]);
  cut_assert_null (argv[3]);
}

void
test_rest2d (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { NULL } 
  };
        
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test -- -bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("program", argv[0]);
  cut_assert_equal_string ("--", argv[1]);
  cut_assert_equal_string ("-bar", argv[2]);
  cut_assert_null (argv[3]);
}


/* check that G_OPTION_REMAINING collects non-option arguments */
void
test_rest3 (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &test_array, NULL, NULL },
      { NULL } 
  };
        
  context = g_option_context_new (NULL);
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo --test bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("foo", test_array[0]);
  cut_assert_equal_string ("bar", test_array[1]);
  cut_assert_null (argv[2]);

  g_strfreev (test_array);
}


/* check that G_OPTION_REMAINING and -- work together */
void
test_rest4 (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &test_array, NULL, NULL },
      { NULL } 
  };
        
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo --test -- -bar", &argc);
  
  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("foo", test_array[0]);
  cut_assert_equal_string ("-bar", test_array[1]);
  cut_assert_null (argv[2]);

  g_strfreev (test_array);
}

/* test that G_OPTION_REMAINING works with G_OPTION_ARG_FILENAME_ARRAY */
void
test_rest5 (void)
{
  GOptionEntry entries [] = { 
      { "test", 0, 0, G_OPTION_ARG_NONE, &test_boolean, NULL, NULL },
      { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &test_array, NULL, NULL },
      { NULL } 
  };
        
  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program foo --test bar", &argc);

  cut_assert (g_option_context_parse (context, &argc, &argv, &error));
  
  /* Check array */
  cut_assert (test_boolean);
  cut_assert_equal_string ("foo", test_array[0]);
  cut_assert_equal_string ("bar", test_array[1]);
  cut_assert_null (argv[2]);

  g_strfreev (test_array);
}

void
test_unknown_short (void)
{
  GOptionEntry entries [] = { { NULL } };

  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program -0", &argc);

  cut_assert (!g_option_context_parse (context, &argc, &argv, &error));
}

/* test that lone dashes are treated as non-options */
void
test_lonely_dash (void)
{
  /* Now try parsing */
  argv = split_string ("program -", &argc);

  cut_assert (g_option_context_parse (context, &argc, &argv, &error));

  cut_assert (argv[1]);
  cut_assert ("-", argv[1]);
}

void
test_missing_arg (void)
{
  gchar *arg = NULL;
  GOptionEntry entries [] =
    { { "test", 't', 0, G_OPTION_ARG_STRING, &arg, NULL, NULL },
      { NULL } };

  g_option_context_add_main_entries (context, entries, NULL);

  /* Now try parsing */
  argv = split_string ("program --test", &argc);

  cut_assert (!g_option_context_parse (context, &argc, &argv, &error));
  g_clear_error (&error);

  /* Try parsing again */
  argv = split_string ("program --t", &argc);

  cut_assert (!g_option_context_parse (context, &argc, &argv, &error));
}

