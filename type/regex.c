#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "regex.h"
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void free_rx(struct Regex **r)
{
  if (!r || !*r)
    return;

  FREE(&(*r)->pattern);
  //regfree(r->rx)
  FREE(r);
}

static void destroy_rx(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return;

  struct Regex **r = (struct Regex **) var;
  if (!*r)
    return;

  free_rx(r);
}

static bool set_rx(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                   const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  struct Regex *r = safe_calloc(1, sizeof(*r));

  r->pattern = safe_strdup(value);
  r->rx = NULL; //XXX regenerate r->rx

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) r, err))
  {
    free_rx(&r);
    return false;
  }

  destroy_rx(var, vdef);

  *(struct Regex **) var = r;
  return true;
}

static bool get_rx(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  struct Regex *r = *(struct Regex **) var;
  if (!r)
    return false;

  mutt_buffer_addstr(result, r->pattern);
  return true;
}

static struct Regex *dup_regex(struct Regex *addr)
{
  struct Regex *rx = safe_calloc(1, sizeof(*rx));
  rx->pattern = safe_strdup(addr->pattern);
  return rx;
}

static bool set_native_rx(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  regex_free(var);

  struct Regex *rx = dup_regex((struct Regex *) value);

  *(struct Regex **) var = rx;
  return true;
}

static intptr_t get_native_rx(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  struct Regex *rx = *(struct Regex **) var;

  return (intptr_t) rx;
}

static bool reset_rx(struct ConfigSet *cs, void *var,
                     const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  destroy_rx(var, vdef);

  struct Regex *r = safe_calloc(1, sizeof(*r));

  r->pattern = safe_strdup((char *) vdef->initial);
  r->rx = NULL; //XXX regenerate r->rx

  *(struct Regex **) var = r;
  return true;
}

void init_regex(void)
{
  struct ConfigSetType cst_rx = { "regex", set_rx, get_rx, set_native_rx, get_native_rx, reset_rx, destroy_rx, };
  cs_register_type(DT_RX, &cst_rx);
}
struct Regex *regex_create(const char *str)
{
  struct Regex *rx = safe_calloc(1, sizeof(*rx));
  rx->pattern = safe_strdup(str);
  return rx;
}

void regex_free(struct Regex **rx)
{
  FREE(&(*rx)->pattern);
  FREE(rx);
}
