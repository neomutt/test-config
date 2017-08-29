#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "config/set.h"
#include "debug.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"

struct HashElem;

const char *line = "----------------------------------------"
                   "----------------------------------------";

bool validator_fail(const struct ConfigSet *cs, const struct VariableDef *vdef,
                    intptr_t value, struct Buffer *result)
{
  if (value > 1000000)
    mutt_buffer_printf(result, "%s: %s, (ptr)", __func__, vdef->name);
  else
    mutt_buffer_printf(result, "%s: %s, %ld", __func__, vdef->name, value);
  return false;
}

bool validator_succeed(const struct ConfigSet *cs, const struct VariableDef *vdef,
                       intptr_t value, struct Buffer *result)
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

bool log_listener(const struct ConfigSet *cs, struct HashElem *he,
                  const char *name, enum ConfigEvent ev)
{
  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = safe_calloc(1, STRING);
  result.dsize = STRING;

  const char *events[] = { "set", "reset" };

  mutt_buffer_reset(&result);
  cs_get_variable2(cs, he, &result);

  printf("Event: %s has been %s to '%s'\n", name, events[ev], result.data);

  FREE(&result.data);
  return true;
}

void set_list(const struct ConfigSet *cs)
{
  log_line(__func__);
  cs_dump_set(cs);
  // hash_dump(cs->hash);
}
