#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "regex2.h"

static void free_rx(struct Regex **r)
{
  if (!r || !*r)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*r)->pattern);
  //regfree(r->rx)
  FREE(r);
}

static void destroy_rx(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct Regex **r = (struct Regex **) var;
  if (!*r)
    return;

  free_rx(r);
}

static bool set_rx(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                   const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Regex *r = NULL;
  if (value)
  {
    r = safe_calloc(1, sizeof(*r));
    r->pattern = safe_strdup(value);
    r->rx = NULL; //XXX regenerate r->rx
  }

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
    return false; /* LCOV_EXCL_LINE */

  struct Regex *r = *(struct Regex **) var;
  if (!r)
    return false;

  mutt_buffer_addstr(result, r->pattern);
  return true;
}

static struct Regex *dup_regex(struct Regex *regex)
{
  if (!regex)
    return NULL; /* LCOV_EXCL_LINE */

  struct Regex *rx = safe_calloc(1, sizeof(*rx));
  rx->pattern = safe_strdup(regex->pattern);
  return rx;
}

static bool set_native_rx(const struct ConfigSet *cs, void *var,
                          const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  regex_free(var);

  struct Regex *rx = dup_regex((struct Regex *) value);

  *(struct Regex **) var = rx;
  return true;
}

static intptr_t get_native_rx(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Regex *rx = *(struct Regex **) var;

  return (intptr_t) rx;
}

static bool reset_rx(const struct ConfigSet *cs, void *var,
                     const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  destroy_rx(var, vdef);

  struct Regex *r = safe_calloc(1, sizeof(*r));

  r->pattern = safe_strdup((char *) vdef->initial);
  r->rx = NULL; //XXX regenerate r->rx

  *(struct Regex **) var = r;
  return true;
}

void init_regex(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_rx = {
    "regex", set_rx, get_rx, set_native_rx, get_native_rx, reset_rx, destroy_rx,
  };
  cs_register_type(cs, DT_RX, &cst_rx);
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