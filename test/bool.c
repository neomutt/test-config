#include <stdio.h>
#include <stdbool.h>
#include "type/bool.h"
#include "config_set.h"

bool test_bool(void)
{
  struct ConfigSet *cs = NULL;
  init_bool(cs);
  return false;
}

