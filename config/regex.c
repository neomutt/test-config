#include "config.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "regex2.h"
#include "set.h"
#include "types.h"

static void regex_destroy(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct Regex **r = (struct Regex **) var;
  if (!*r)
    return;

  regex_free(r);
}

static bool regex_string_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, const char *value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Regex *r = NULL;
  if (value)
  {
    r = safe_calloc(1, sizeof(*r));
    r->pattern = safe_strdup(value);
    r->regex = NULL; //XXX regenerate r->regex
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) r, err))
  {
    regex_free(&r);
    return false;
  }

  regex_destroy(var, vdef);

  *(struct Regex **) var = r;
  return true;
}

static bool regex_string_get(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Regex *r = *(struct Regex **) var;
  if (!r)
    return false;

  mutt_buffer_addstr(result, r->pattern);
  return true;
}

static struct Regex *regex_dup(struct Regex *r)
{
  if (!r)
    return NULL; /* LCOV_EXCL_LINE */

  struct Regex *copy = safe_calloc(1, sizeof(*copy));
  copy->pattern = safe_strdup(r->pattern);
  return copy;
}

static bool regex_native_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, intptr_t value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  regex_free(var);

  struct Regex *r = regex_dup((struct Regex *) value);

  *(struct Regex **) var = r;
  return true;
}

static intptr_t regex_native_get(const struct ConfigSet *cs, void *var,
                                 const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Regex *r = *(struct Regex **) var;

  return (intptr_t) r;
}

static bool regex_reset(const struct ConfigSet *cs, void *var,
                        const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  regex_destroy(var, vdef);

  struct Regex *r = safe_calloc(1, sizeof(*r));

  r->pattern = safe_strdup((char *) vdef->initial);
  r->regex = NULL; //XXX regenerate r->regex

  *(struct Regex **) var = r;
  return true;
}

void regex_free(struct Regex **r)
{
  if (!r || !*r)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*r)->pattern);
  //regfree(r->regex)
  FREE(r);
}

void regex_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_regex = {
    "regex",          regex_string_set, regex_string_get, regex_native_set,
    regex_native_get, regex_reset,      regex_destroy,
  };
  cs_register_type(cs, DT_REGEX, &cst_regex);
}

struct Regex *regex_create(const char *str)
{
  struct Regex *r = safe_calloc(1, sizeof(*r));
  r->pattern = safe_strdup(str);
  return r;
}
