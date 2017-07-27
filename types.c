#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "buffer.h"
#include "config_set.h"
#include "data.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"
#include "type/address.h"
#include "type/bool.h"
#include "type/magic.h"
#include "type/mbyte_table.h"
#include "type/mutt_regex.h"
#include "type/number.h"
#include "type/path.h"
#include "type/quad.h"
#include "type/sort.h"
#include "type/string.h"

int Percentage;
char *PrintCommand;
#define UL (intptr_t)

bool percentage_validator(struct ConfigSet *set, const char *name, int type, intptr_t value, struct Buffer *result)
{
  return false;
}

struct VariableDef vars[] =
{
  { "percentage",    DT_NUM, &Percentage,   10,       percentage_validator },
  { "print_command", DT_STR, &PrintCommand, UL "lpr", NULL },
  { NULL },
};

bool init_types(struct ConfigSet *set)
{
  init_addr();
  init_bool();
  init_magic();
  init_mbyte_table();
  init_regex();
  init_num();
  init_path();
  init_quad();
  init_sorts();
  init_string();

  cs_register_variables(set, MuttVars);

  return true;
}

