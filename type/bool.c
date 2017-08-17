#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"
#include "account.h"

const char *bool_values[] = {
  "no", "yes", "false", "true", "0", "1", "off", "on",
};

static bool set_bool(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  int num = -1;
  for (unsigned int i = 0; i < mutt_array_size(bool_values); i++)
  {
    if (mutt_strcasecmp(bool_values[i], value) == 0)
    {
      num = i % 2;
      break;
    }
  }

  if (num < 0)
  {
    mutt_buffer_printf(err, "Invalid boolean value: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) num, err))
    return false;

  *(bool *) var = num;
  return true;
}

static bool get_bool(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  unsigned int index = *(bool *) var;
  if (index > 1)
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, bool_values[index]);
  return true;
}

static bool set_native_bool(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  if ((value < 0) || (value > 1))
  {
    mutt_buffer_printf(err, "Invalid boolean value: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  *(bool *) var = value;
  return true;
}

static intptr_t get_native_bool(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  return *(bool *) var;
}

static bool reset_bool(struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  *(bool *) var = vdef->initial;
  return true;
}

bool set_he_bool_err(struct Account *ac, int vid, bool value, struct Buffer *err)
{
  intptr_t copy = value;
  return account_set_value(ac, vid, copy, err);
}

bool set_he_bool(struct Account *ac, int vid, bool value)
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  mutt_buffer_reset(&err);

  bool result = set_he_bool_err(ac, vid, value, &err);
  if (!result)
    printf("%s\n", err.data);

  FREE(&err.data);
  return result;
}

bool get_he_bool_err(struct Account *ac, int vid, struct Buffer *err)
{
  return account_get_value(ac, vid, err);
}

bool get_he_bool(struct Account *ac, int vid)
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  mutt_buffer_reset(&err);

  bool result = get_he_bool_err(ac, vid, &err);
  // if (!result)
  //   printf("%s\n", err.data);

  FREE(&err.data);
  return result;
}

void init_bool(void)
{
  const struct ConfigSetType cst_bool = { "boolean", set_bool, get_bool, set_native_bool, get_native_bool, reset_bool, NULL, };
  cs_register_type(DT_BOOL, &cst_bool);
}

