#include "regex.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_rx(void **obj, struct VariableDef *vdef)
{
  if (!obj || !*obj)
    return;

  // struct Regex *r = (struct Regex *) obj;
  //XXX FREE(&r->pattern);
  //regfree(r->rx)
  // FREE(r);
}

static bool set_rx(struct ConfigSet *cs, void *variable, struct VariableDef *vdef, const char *value,
                   struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  struct Regex *r = variable;
  if (!r)
    return false;

  r->rx = NULL; //XXX regenerate r->rx
  //XXX mutt_str_replace(&r->pattern, value);
  r->pattern = safe_strdup(value);
  return true;
}

static bool get_rx(void *variable, struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  struct Regex *r = variable;
  if (!r)
    return false;

  mutt_buffer_addstr(result, r->pattern);
  return true;
}

static bool reset_rx(struct ConfigSet *cs, void *variable, struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  destroy_rx(variable, vdef);

  struct Regex *r = variable;
  if (!r)
    return false;

  r->rx = NULL; //XXX regenerate r->rx
  mutt_str_replace(&r->pattern, (const char*) vdef->initial);
  return true;
}


void init_regex(void)
{
  struct ConfigSetType cst_rx = { "regex", set_rx, get_rx, reset_rx, destroy_rx };
  cs_register_type(DT_RX, &cst_rx);
}
