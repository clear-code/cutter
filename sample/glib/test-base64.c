#include <cutter.h>

#include "config.h"

#include <glib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>

#define DATA_SIZE 1024
#define BLOCK_SIZE 32
#define NUM_BLOCKS 32
static guchar data[DATA_SIZE];
static guchar *data2 = NULL;
static gchar *text = NULL;

void test_full (void);
void test_incremental_with_line_breaks1 (void);
void test_incremental_with_line_breaks2 (void);
void test_incremental_with_line_breaks3 (void);
void test_incremental_no_line_breaks1 (void);
void test_incremental_no_line_breaks2 (void);
void test_incremental_no_line_breaks3 (void);

void
setup (void)
{
  int i;
  for (i = 0; i < DATA_SIZE; i++)
    data[i] = (guchar)i;
  data2 = NULL;
  text = NULL;
}

void
teardown (void)
{
  g_free (data2);
  g_free (text);
}

static void
test_incremental (gboolean line_break, 
		  gint     length,
		  gsize   *encoded_length,
		  gsize   *decoded_length)
{
  char *p;
  gsize len, max, input_len, block_size;
  int state, save;
  guint decoder_save;

  data2 = g_malloc (length);
  text = g_malloc (length * 2);

  len = 0;
  state = 0;
  save = 0;
  input_len = 0;
  while (input_len < length)
    {
      block_size = MIN (BLOCK_SIZE, length - input_len);
      len += g_base64_encode_step (data + input_len, block_size,
				   line_break, text + len, &state, &save);
      input_len += block_size;
    }
  len += g_base64_encode_close (line_break, text + len, &state, &save);

  *encoded_length = len;

  if (line_break)
    max = length * 4 / 3 + length * 4 / (3 * 72) + 7;
  else
    max = length * 4 / 3 + 6;

  *decoded_length = 0;
  state = 0;
  decoder_save = 0;
  p = text;
  while (len > 0)
    {
      int chunk_len = MIN (BLOCK_SIZE, len);
      *decoded_length += g_base64_decode_step (p, 
					   chunk_len, 
					   data2 + *decoded_length,
					   &state, &decoder_save);
      p += chunk_len;
      len -= chunk_len;
    }
}

#define test_incremental_with_line_breaks_for_length(length) do         \
{                                                                       \
    gsize _length, encoded_length, decoded_length, max;                 \
    _length = (length);                                                 \
    max = _length * 4 / 3 + _length * 4 / (3 * 72) + 7;                 \
    test_incremental (FALSE, _length,                                   \
                      &encoded_length, &decoded_length);                \
    cut_assert (encoded_length <= max,                                  \
                "Too long encoded length: got %d, expected max %d",     \
                encoded_length, max);                                   \
    cut_assert_equal_memory (data, _length, data2, decoded_length);     \
} while (0)

#define test_incremental_no_line_breaks_for_length(length) do           \
{                                                                       \
    gsize _length, encoded_length, decoded_length, max;                 \
    _length = (length);                                                 \
    max = _length * 4 / 3 + 6;                                          \
    test_incremental (FALSE, _length,                                   \
                      &encoded_length, &decoded_length);                \
    cut_assert (encoded_length <= max,                                  \
                "Too long encoded length: got %d, expected max %d",     \
                encoded_length, max);                                   \
    cut_assert_equal_memory (data, _length, data2, decoded_length);     \
} while (0)

void
test_incremental_no_line_breaks1 (void)
{
  test_incremental_no_line_breaks_for_length (DATA_SIZE);
}

void
test_incremental_no_line_breaks2 (void)
{
  test_incremental_no_line_breaks_for_length (DATA_SIZE - 1);
}

void
test_incremental_no_line_breaks3 (void)
{
  test_incremental_no_line_breaks_for_length (DATA_SIZE - 2);
}

void
test_incremental_with_line_breaks1 (void)
{
  test_incremental_with_line_breaks_for_length (DATA_SIZE);
}

void
test_incremental_with_line_breaks2 (void)
{
  test_incremental_with_line_breaks_for_length (DATA_SIZE - 1); 
}

void
test_incremental_with_line_breaks3 (void)
{
  test_incremental_with_line_breaks_for_length (DATA_SIZE - 2);
}

void
test_full (void)
{
  gsize len;

  text = g_base64_encode (data, DATA_SIZE);
  data2 = g_base64_decode (text, &len);

  cut_assert_equal_int (DATA_SIZE, len);

  cut_assert_equal_memory (data, DATA_SIZE, data2, DATA_SIZE);
}

