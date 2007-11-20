#include <cutter.h>
#include <time.h>
#include <stdlib.h>

#include <glib.h>

#define MAX_THREADS            50
#define MAX_SORTS              5    /* only applies if
				       ASYC_QUEUE_DO_SORT is set to 1 */ 
#define MAX_TIME               20   /* seconds */
#define MIN_TIME               5    /* seconds */

#define SORT_QUEUE_AFTER       1
#define SORT_QUEUE_ON_PUSH     1    /* if this is done, the
				       SORT_QUEUE_AFTER is ignored */
#define QUIT_WHEN_DONE         1


#if SORT_QUEUE_ON_PUSH == 1
#  undef SORT_QUEUE_AFTER
#  define SORT_QUEUE_AFTER     0
#endif

void test_asyncqueue (void);

static GMainLoop *main_loop = NULL;
static GThreadPool *thread_pool = NULL;
static GAsyncQueue *async_queue = NULL;


static gint
sort_compare (gconstpointer p1, gconstpointer p2, gpointer user_data)
{
  gint32 id1;
  gint32 id2;

  id1 = GPOINTER_TO_INT (p1);
  id2 = GPOINTER_TO_INT (p2);

  return (id1 > id2 ? +1 : id1 == id2 ? 0 : -1);
}

static gboolean
sort_queue (gpointer user_data)
{
  static gint     sorts = 0;
  static gpointer last_p = NULL;
  gpointer        p;
  gboolean        can_quit = FALSE;
  gint            sort_multiplier;
  gint            len;
  gint            i;

  sort_multiplier = GPOINTER_TO_INT (user_data);

  if (SORT_QUEUE_AFTER) {
    g_async_queue_sort (async_queue, sort_compare, NULL);

    sorts++;

    if (sorts >= sort_multiplier) {
      can_quit = TRUE;
    }
    
    g_async_queue_sort (async_queue, sort_compare, NULL);
    len = g_async_queue_length (async_queue);

  } else {
    can_quit = TRUE;
    len = g_async_queue_length (async_queue);
  }

  for (i = 0, last_p = NULL; i < len; i++) {
    p = g_async_queue_pop (async_queue);

    if (last_p) {
      cut_assert (GPOINTER_TO_INT (last_p) <= GPOINTER_TO_INT (p));
    }

    last_p = p;
  }
  
  if (can_quit && QUIT_WHEN_DONE) {
    g_main_loop_quit (main_loop);
  }

  return !can_quit;
}

static void
enter_thread (gpointer data, gpointer user_data)
{
  gint   len;
  gint   id;
  gulong ms;

  id = GPOINTER_TO_INT (data);
  
  ms = g_random_int_range (MIN_TIME * 1000, MAX_TIME * 1000);

  g_usleep (ms * 1000);

  if (SORT_QUEUE_ON_PUSH) {
    g_async_queue_push_sorted (async_queue, GINT_TO_POINTER (id), sort_compare, NULL);
  } else {
    g_async_queue_push (async_queue, GINT_TO_POINTER (id));
  }

  len = g_async_queue_length (async_queue);
}

void
test_asyncqueue (void)
{
  gint   i;
  gint   max_threads = MAX_THREADS;
  gint   max_unused_threads = MAX_THREADS;
  gint   sort_multiplier = MAX_SORTS;
  gint   sort_interval;


  async_queue = g_async_queue_new ();

  cut_assert (async_queue);

  thread_pool = g_thread_pool_new (enter_thread,
				   async_queue,
				   max_threads,
				   FALSE,
				   NULL);

  cut_assert (thread_pool);

  g_thread_pool_set_max_unused_threads (max_unused_threads);

  for (i = 1; i <= max_threads; i++) {
    GError *error = NULL;
  
    g_thread_pool_push (thread_pool, GINT_TO_POINTER (i), &error);
    
    cut_assert_null (error);
  }

  if (!SORT_QUEUE_AFTER) {
    sort_multiplier = 1;
  }
  
  sort_interval = ((MAX_TIME / sort_multiplier) + 2)  * 1000;
  g_timeout_add (sort_interval, sort_queue, GINT_TO_POINTER (sort_multiplier));

  main_loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (main_loop);
}
