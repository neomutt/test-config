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
#include "type/mbyte_table.h"
#include "type/mutt_regex.h"

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

  cs_register_variable("percentage", DT_NUM, "10", NULL);

  // cs_register_variables(set, vars);
  cs_register_variables(set, MuttVars);

  return true;
}

