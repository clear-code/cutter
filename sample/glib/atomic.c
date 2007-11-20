#include <cutter.h>
#include <glib.h>

void test_atomic (void);

/* Obviously we can't test that the operations are atomic, but we can
 * at least test, that they do, what they ought to do */

void
test_atomic (void)
{
  gint i;
  gint atomic = -5;
  gpointer atomic_pointer = NULL;
  gpointer biggest_pointer = (gpointer)((gsize)atomic_pointer - 1);

  for (i = 0; i < 15; i++)
    g_atomic_int_inc (&atomic);
  cut_assert_equal_int (10, atomic);
  for (i = 0; i < 9; i++)
    cut_assert (!g_atomic_int_dec_and_test (&atomic));
  cut_assert (g_atomic_int_dec_and_test (&atomic));
  cut_assert_equal_int (0, atomic);

  cut_assert_equal_int (0, g_atomic_int_exchange_and_add (&atomic, 5));
  cut_assert_equal_int (5, atomic);

  cut_assert_equal_int (5, g_atomic_int_exchange_and_add (&atomic, -10));
  cut_assert_equal_int (-5, atomic);

  g_atomic_int_add (&atomic, 20);
  cut_assert_equal_int (15, atomic);

  g_atomic_int_add (&atomic, -35);
  cut_assert_equal_int (-20, atomic);

  cut_assert_equal_int (atomic, g_atomic_int_get (&atomic));

  cut_assert (g_atomic_int_compare_and_exchange (&atomic, -20, 20));
  cut_assert_equal_int (20, atomic);
  
  cut_assert (!g_atomic_int_compare_and_exchange (&atomic, 42, 12));
  cut_assert_equal_int (20, atomic);
  
  cut_assert (g_atomic_int_compare_and_exchange (&atomic, 20, G_MAXINT));
  cut_assert_equal_int (G_MAXINT, atomic);

  cut_assert (g_atomic_int_compare_and_exchange (&atomic, G_MAXINT, G_MININT));
  cut_assert_equal_int (G_MININT, atomic);

  cut_assert (g_atomic_pointer_compare_and_exchange (&atomic_pointer, 
						     NULL, biggest_pointer));
  cut_assert_equal_int (biggest_pointer, atomic_pointer);

  cut_assert_equal_int (atomic_pointer, g_atomic_pointer_get (&atomic_pointer));

  cut_assert (g_atomic_pointer_compare_and_exchange (&atomic_pointer, 
						     biggest_pointer, NULL));
  cut_assert_null (biggest_pointer);
}
