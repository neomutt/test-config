#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "regex.h"
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_rx(void **obj, const struct VariableDef *vdef)
{
  if (!obj || !*obj)
    return;

  struct Regex **r = (struct Regex **) obj;
  if ((*r)->pattern != (char *) vdef->initial)
    FREE(&(*r)->pattern);
  //regfree(r->rx)
  FREE(r);
}

static bool set_rx(struct ConfigSet *cs, void *variable, const struct VariableDef *vdef,
                   const char *value, struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  destroy_rx(variable, vdef);

  struct Regex *r = safe_calloc(1, sizeof(*r));

  r->pattern = safe_strdup(value);
  r->rx = NULL; //XXX regenerate r->rx

  *(struct Regex **) variable = r;
  return true;
}

static bool get_rx(void *variable, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  struct Regex *r = *(struct Regex **) variable;
  if (!r)
    return false;

  mutt_buffer_addstr(result, r->pattern);
  return true;
}

static bool reset_rx(struct ConfigSet *cs, void *variable,
                     const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  destroy_rx(variable, vdef);

  struct Regex *r = safe_calloc(1, sizeof(*r));

  r->pattern = (char *) vdef->initial;
  r->rx = NULL; //XXX regenerate r->rx

  *(struct Regex **) variable = r;
  return true;
}


void init_regex(void)
{
  const struct ConfigSetType cst_rx = { "regex", set_rx, get_rx, reset_rx, destroy_rx };
  cs_register_type(DT_RX, &cst_rx);
}
