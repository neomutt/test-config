#ifndef _ACUTEST_H
#define _ACUTEST_H

#include <stdbool.h>
#include <stdio.h>

static inline bool TEST_CHECK(bool result)
{
  return result;
}

#define TEST_MSG printf
#define TEST_CHECK_(NUM,REST) printf("%s\n", REST);

#endif /* _ACUTEST_H */
