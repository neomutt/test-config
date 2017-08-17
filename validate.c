#include <string.h>
#include <strings.h>
#include "validate.h"
#include "lib/lib.h"
#include "type/sort.h"
#include "type/regex.h"
#include "type/mbyte_table.h"

bool val_path(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  const char *str = (const char *) value;

  if (mutt_strcmp(str, "dir") == 0)
    return true;

  mutt_buffer_addstr(err, "val_path failed");
  return false;
}

bool val_str(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  const char *str = (const char *) value;

  if (mutt_strcmp(str, "alias") == 0)
    return true;

  mutt_buffer_addstr(err, "val_str failed");
  return false;
}

bool val_addr(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  const char *str = (const char *) value;

  if (strchr(str, '@'))
    return true;

  mutt_buffer_addstr(err, "val_addr failed");
  return false;
}

bool val_magic(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (value == 4)
    return true;

  mutt_buffer_addstr(err, "val_magic failed");
  return false;
}

bool val_num(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (value < 50)
    return true;

  mutt_buffer_addstr(err, "val_num failed");
  return false;
}

bool val_quad(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if ((value == 1) || (value == 3))
    return true;

  mutt_buffer_addstr(err, "val_quad failed");
  return false;
}

bool val_rx(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  struct Regex *r = (struct Regex *) value;

  if (strstr(r->pattern, "ment"))
    return true;

  mutt_buffer_addstr(err, "val_rx failed");
  return false;
}

bool val_bool(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  static bool test = false;
  if (test)
    return true;

  test = true;
  mutt_buffer_addstr(err, "val_bool failed");
  return false;
}

bool val_sort(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (value == SORT_SPAM)
    return true;

  mutt_buffer_addstr(err, "val_sort failed");
  return false;
}

bool val_mbchartbl(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  struct MbCharTable *table = (struct MbCharTable *) value;

  if (strcasecmp(table->orig_str, "pqrs") == 0)
    return true;

  mutt_buffer_addstr(err, "val_mbchartbl failed");
  return false;
}

