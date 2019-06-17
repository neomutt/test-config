#include "config.h"
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mutt/logging.h"
#include "dump/dump.h"
#include "test/account2.h"
#include "test/address.h"
#include "test/bool.h"
#include "test/deep.h"
#include "test/enum.h"
#include "test/inherit.h"
#include "test/initial.h"
#include "test/long.h"
#include "test/mbtable.h"
#include "test/number.h"
#include "test/quad.h"
#include "test/regex3.h"
#include "test/set.h"
#include "test/slist.h"
#include "test/sort.h"
#include "test/string4.h"
#include "test/synonym.h"

typedef void (*test_fn)(void);

int log_disp_stdout(time_t stamp, const char *file, int line,
                    const char *function, int level, ...)
{
  int err = errno;

  va_list ap;
  va_start(ap, level);
  const char *fmt = va_arg(ap, const char *);
  int ret = vprintf(fmt, ap);
  va_end(ap);

  if (level == LL_PERROR)
    ret += printf("%s", strerror(err));

  return ret;
}

struct Test
{
  const char *name;
  test_fn function;
};

// clang-format off
struct Test test[] = {
  { "set",       config_set       },
  { "account",   config_account   },
  { "initial",   config_initial   },
  { "synonym",   config_synonym   },
  { "address",   config_address   },
  { "bool",      config_bool      },
  { "enum",      config_enum      },
  { "long",      config_long      },
  { "mbtable",   config_mbtable   },
  { "number",    config_number    },
  { "quad",      config_quad      },
  { "regex",     config_regex     },
  { "slist",     config_slist     },
  { "sort",      config_sort      },
  { "string",    config_string    },
  { "deep",      config_deep      },
  { "dump",      config_dump      },
  { "inherit",   config_inherit   },
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

  MuttLogger = log_disp_stdout;

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

    t->function();
  }

  return result;
}
