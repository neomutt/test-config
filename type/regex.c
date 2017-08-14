#include "regex.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_rx(void **obj, struct VariableDef *def)
{
  if (!obj || !*obj)
    return;

  // struct Regex *r = (struct Regex *) obj;
  //XXX FREE(&r->pattern);
  //regfree(r->rx)
  // FREE(r);
}

static bool set_rx(struct ConfigSet *set, void *variable, struct VariableDef *def, const char *value,
                   struct Buffer *err)
{
  struct Regex *r = variable;
  if (!r)
    return false;

  r->rx = NULL; //XXX regenerate r->rx
  //XXX mutt_str_replace(&r->pattern, value);
  r->pattern = safe_strdup(value);
  return true;
}

static bool get_rx(void *variable, struct VariableDef *def, struct Buffer *result)
{
  struct Regex *r = variable;
  if (!r)
    return false;

  mutt_buffer_addstr(result, r->pattern);
  return true;
}

static bool reset_rx(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  struct VariableDef *def = e->data;
  if (!def)
    return false;

  destroy_rx(def->variable, def);

  struct Regex *r = def->variable;
  if (!r)
    return false;

  r->rx = NULL; //XXX regenerate r->rx
  mutt_str_replace(&r->pattern, (const char*) def->initial);
  return true;
}


void init_regex(void)
{
  struct ConfigSetType cst_rx = { "regex", set_rx, get_rx, reset_rx, destroy_rx };
  cs_register_type(DT_RX, &cst_rx);
}
