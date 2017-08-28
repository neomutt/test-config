#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "test/account2.h"
#include "test/address.h"
#include "test/bool.h"
#include "test/configset.h"
#include "test/initial.h"
#include "test/magic.h"
#include "test/mbyte_table.h"
#include "test/number.h"
#include "test/path.h"
#include "test/quad.h"
#include "test/regex.h"
#include "test/sort.h"
#include "test/string.h"
#include "test/synonym.h"

typedef bool (*test_fn)(void);

// clang-format off
struct Test
{
  const char *name;
  test_fn function;
} test[] = {
  { "configset",   configset_test  },
  { "account",     account_test    },
  { "address",     address_test    },
  { "bool",        bool_test       },
  { "initial",     initial_test    },
  { "magic",       magic_test      },
  { "mbyte_table", mbytetable_test },
  { "number",      number_test     },
  { "path",        path_test       },
  { "quad",        quad_test       },
  { "regex",       regex_test      },
  { "sort",        sort_test       },
  { "string",      string_test     },
  { "synonym",     synonym_test    },
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
