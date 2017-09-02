#include "config.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "test/account2.h"
#include "test/address.h"
#include "test/bool.h"
#include "test/initial.h"
#include "test/magic.h"
#include "test/mbtable.h"
#include "test/number.h"
#include "test/path.h"
#include "test/quad.h"
#include "test/regex3.h"
#include "test/set.h"
#include "test/sort.h"
#include "test/string4.h"
#include "test/synonym.h"

typedef bool (*test_fn)(void);

/* stdout override */
void mutt_debug(int level, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  va_end(ap);
}

// clang-format off
struct Test
{
  const char *name;
  test_fn function;
} test[] = {
  { "set",       set_test       },
  { "account",   account_test   },
  { "initial",   initial_test   },
  { "synonym",   synonym_test   },
  { "address",   address_test   },
  { "bool",      bool_test      },
  { "magic",     magic_test     },
  { "mbtable",   mbtable_test   },
  { "number",    number_test    },
  { "path",      path_test      },
  { "quad",      quad_test      },
  { "regex",     regex_test     },
  { "sort",      sort_test      },
  { "string",    string_test    },
  { NULL },
};
// clang-format on

int main(int argc, char *argv[])
{
  int result = 0;

  if (argc < 2)
  {
    printf("Usage: %s TEST ...\n", argv[0]);
    for (int i = 0; test[i].name; i++)
      printf("    %s\n", test[i].name);
    return 1;
  }

  for (; --argc > 0; argv++)
  {
    struct Test *t = NULL;

    for (int i = 0; test[i].name; i++)
    {
      if (strcmp(argv[1], test[i].name) == 0)
      {
        t = &test[i];
        break;
      }
    }
    if (!t)
    {
      printf("Unknown test '%s'\n", argv[1]);
      result = 1;
      continue;
    }

    if (!t->function())
    {
      printf("%s_test() failed\n", t->name);
      result = 1;
    }
  }

  return result;
}
