#ifndef CUTTER_ASSERSIONS_H
#define CUTTER_ASSERSIONS_H
#include <stdio.h>
#include <string.h>

#define ASSERT_MESSAGE_BUFFER_SIZE 512

#define UT_PASS return TRUE

#define UT_ASSERT(expect, message)                          \
if(!(expect))                                               \
{                                                           \
  UT_INFO->logerror(UT_INFO, __LINE__ ,__FILE__, message);  \
  return FALSE;                                             \
}

#define UT_FAIL(message)                                    \
if(1)                                                       \
{                                                           \
  UT_INFO->logerror(UT_INFO, __LINE__, __FILE__, message);  \
  return FALSE;                                             \
}

#define UT_ASSERT_EQUAL_INT(expect, actual, message)    \
if (expect != actual)                                   \
{                                                       \
  char buffer[ASSERT_MESSAGE_BUFFER_SIZE];              \
  snprintf(buffer,                                      \
           ASSERT_MESSAGE_BUFFER_SIZE - 1,              \
           "%s\n expected: <%d>\n  but was: <%d>",      \
           message, expect, actual);                    \
  UT_INFO->logerror(UT_INFO,__LINE__,__FILE__,buffer);  \
  return FALSE;                                         \
}

#define UT_ASSERT_EQUAL_DOUBLE(expect, error, actual, message)  \
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
    UT_INFO->logerror(UT_INFO,__LINE__,__FILE__,buffer);        \
    return FALSE;                                               \
  }                                                             \
} while(0)

#define UT_ASSERT_EQUAL_STRING(expect, actual, message) \
if (strcmp(expect, actual) != 0)                        \
{                                                       \
  char buffer[ASSERT_MESSAGE_BUFFER_SIZE];              \
  snprintf(buffer,                                      \
           ASSERT_MESSAGE_BUFFER_SIZE - 1,              \
           "%s\n expected: <%s>\n  but was: <%s>",      \
           message, expect, actual);                    \
  UT_INFO->logerror(UT_INFO,__LINE__,__FILE__,buffer);  \
  return FALSE;                                         \
}


#endif /* CUTTER_ASSERSIONS_H */
