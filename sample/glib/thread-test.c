#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <glib.h>

/* GStaticPrivate */

#define THREADS 10

static GStaticMutex test_g_static_private_mutex = G_STATIC_MUTEX_INIT;
static guint test_g_static_private_counter = 0;
static guint test_g_static_private_ready = 0;

static gpointer
test_g_static_private_constructor (void)
{
  g_static_mutex_lock (&test_g_static_private_mutex);
  test_g_static_private_counter++;
  g_static_mutex_unlock (&test_g_static_private_mutex);  
  return g_new (guint,1);
}

static void
test_g_static_private_destructor (gpointer data)
{
  g_static_mutex_lock (&test_g_static_private_mutex);
  test_g_static_private_counter--;
  g_static_mutex_unlock (&test_g_static_private_mutex);  
  g_free (data);
}


static gpointer
test_g_static_private_thread (gpointer data)
{
GStaticPrivate test_g_static_private_private1 = G_STATIC_PRIVATE_INIT;
GStaticPrivate test_g_static_private_private2 = G_STATIC_PRIVATE_INIT;
  guint number = GPOINTER_TO_INT (data);
  guint i;
  guint *private1, *private2;
  for (i = 0; i < 10; i++)
    {
      number = number * 11 + 1; /* A very simple and bad RNG ;-) */
      private1 = g_static_private_get (&test_g_static_private_private1);
      if (!private1 || number % 7 > 3)
	{
	  private1 = test_g_static_private_constructor ();
	  g_static_private_set (&test_g_static_private_private1, private1,
				test_g_static_private_destructor);
	}
      *private1 = number;
      private2 = g_static_private_get (&test_g_static_private_private2);
      if (!private2 || number % 13 > 5)
	{
	  private2 = test_g_static_private_constructor ();
	  g_static_private_set (&test_g_static_private_private2, private2,
				test_g_static_private_destructor);
	}
      *private2 = number * 2;
      g_usleep (G_USEC_PER_SEC / 5);
      g_assert (number == *private1);
      g_assert (number * 2 == *private2);      
    }
  g_static_mutex_lock (&test_g_static_private_mutex);
  test_g_static_private_ready++;
  g_static_mutex_unlock (&test_g_static_private_mutex);  

  /* Busy wait is not nice but that's just a test */
  while (test_g_static_private_ready != 0)
    g_usleep (G_USEC_PER_SEC / 5);  

  /* Reuse the static private */
  g_static_private_free (&test_g_static_private_private2);
  g_static_private_init (&test_g_static_private_private2);
  
  for (i = 0; i < 10; i++)
    {
      private2 = g_static_private_get (&test_g_static_private_private2);
      number = number * 11 + 1; /* A very simple and bad RNG ;-) */
      if (!private2 || number % 13 > 5)
	{
	  private2 = test_g_static_private_constructor ();
	  g_static_private_set (&test_g_static_private_private2, private2,
				test_g_static_private_destructor);
	}      
      *private2 = number * 2;
      g_usleep (G_USEC_PER_SEC / 5);
      g_assert (number * 2 == *private2);      
    }

  return GINT_TO_POINTER (GPOINTER_TO_INT (data) * 3);
}

static void
test_g_static_private (void)
{
  GThread *threads[THREADS];
  guint i;

  test_g_static_private_ready = 0;

  for (i = 0; i < THREADS; i++)
    {
      threads[i] = g_thread_create (test_g_static_private_thread, 
				    GINT_TO_POINTER (i), TRUE, NULL);      
    }

  /* Busy wait is not nice but that's just a test */
  while (test_g_static_private_ready != THREADS)
    g_usleep (G_USEC_PER_SEC / 5);

  test_g_static_private_ready = 0;

  for (i = 0; i < THREADS; i++)
    g_assert (GPOINTER_TO_INT (g_thread_join (threads[i])) == i * 3);
    
  g_assert (test_g_static_private_counter == 0); 
}

static gpointer
test (gpointer data)
{
  test_g_static_private ();
  return data;
}

static gpointer
hoge (gpointer data)
{
  return data;
}

int 
main (int   argc,
      char *argv[])
{
  GError *error = NULL;
  GList *threads = NULL, *node;
  int i;
  GThread *t;

  g_thread_init (NULL);

  for (i = 0; i < 100; i++)
    {
      GThread *thread;
      thread = g_thread_create(hoge, NULL, TRUE, &error);
      threads = g_list_append(threads, thread);
    }

    t = g_thread_create(test, NULL, TRUE, NULL);
    threads = g_list_append(threads, t);

  for (node = threads; node; node = g_list_next(node))
    {
      GThread *thread = node->data;

      g_thread_join(thread);
    }

  return 0;
}
