#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "debug.h"

const char *line = "--------------------------------------------------------------------------------";

bool validator_fail(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *result)
{
  if (value > 1000000)
    mutt_buffer_printf(result, "%s: %s, (ptr)", __func__, vdef->name);
  else
    mutt_buffer_printf(result, "%s: %s, %ld", __func__, vdef->name, value);
  return false;
}

bool validator_succeed(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *result)
{
  if (value > 1000000)
    mutt_buffer_printf(result, "%s: %s, (ptr)", __func__, vdef->name);
  else
    mutt_buffer_printf(result, "%s: %s, %ld", __func__, vdef->name, value);
  return true;
}

void log_line(const char *fn)
{
  int len = 54 - mutt_strlen(fn);
  printf("---- %s %.*s\n", fn, len, line);
}

bool log_listener(struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev)
{
  const char *events[] = { "set", "reset" };
  printf("Event: %s has been %s\n", name, events[ev]);
  return true;
}

void set_list(struct ConfigSet *cs)
{
  log_line(__func__);
  cs_dump_set(cs);
}

