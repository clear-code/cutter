#ifndef CUTTER_ASSERSIONS_H
#define CUTTER_ASSERSIONS_H

G_BEGIN_DECLS

#include <stdio.h>
#include <string.h>

#define ASSERT_MESSAGE_BUFFER_SIZE 512

#define CUT_PASS return TRUE

#define CUT_ASSERT(expect, message)                          \
if(!(expect))                                               \
{                                                           \
  CUT_INFO->logerror(CUT_INFO, __LINE__ ,__FILE__, message);  \
  return FALSE;                                             \
}

#define CUT_FAIL(message)                                    \
if(1)                                                       \
{                                                           \
  CUT_INFO->logerror(CUT_INFO, __LINE__, __FILE__, message);  \
  return FALSE;                                             \
}

#define CUT_ASSERT_EQUAL_INT(expect, actual, message)    \
if (expect != actual)                                   \
{                                                       \
  char buffer[ASSERT_MESSAGE_BUFFER_SIZE];              \
  snprintf(buffer,                                      \
           ASSERT_MESSAGE_BUFFER_SIZE - 1,              \
           "%s\n expected: <%d>\n  but was: <%d>",      \
           message, expect, actual);                    \
  CUT_INFO->logerror(CUT_INFO,__LINE__,__FILE__,buffer);  \
  return FALSE;                                         \
}

#define CUT_ASSERT_EQUAL_DOUBLE(expect, error, actual, message)  \
do {                                                            \
  double _expect = expect;                                      \
  double _actual = actual;                                      \
  double _error = error;                                        \
  if (!(_expect - _error <= _actual &&                          \
          _actual <= _expect + _error))                         \
  {                                                             \
    char buffer[ASSERT_MESSAGE_BUFFER_SIZE];                    \
    snprintf(buffer,                                            \
             ASSERT_MESSAGE_BUFFER_SIZE - 1,                    \
             "%s\n expected: <%g> +/-<%g>\n  but was: <%g>",    \
             message, _expect, _error, _actual);                \
    CUT_INFO->logerror(CUT_INFO,__LINE__,__FILE__,buffer);        \
    return FALSE;                                               \
  }                                                             \
} while(0)

#define CUT_ASSERT_EQUAL_STRING(expect, actual, message) \
if (strcmp(expect, actual) != 0)                        \
{                                                       \
  char buffer[ASSERT_MESSAGE_BUFFER_SIZE];              \
  snprintf(buffer,                                      \
           ASSERT_MESSAGE_BUFFER_SIZE - 1,              \
           "%s\n expected: <%s>\n  but was: <%s>",      \
           message, expect, actual);                    \
  CUT_INFO->logerror(CUT_INFO,__LINE__,__FILE__,buffer);  \
  return FALSE;                                         \
}

G_END_DECLS

#endif /* CUTTER_ASSERSIONS_H */
