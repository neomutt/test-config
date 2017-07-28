#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

const char *magic_values[] = { NULL, "mbox", "MMDF", "MH", "Maildir" };

static bool set_magic(struct ConfigSet *set, struct HashElem *e,
                      const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(err, "Variable is not a mailbox type");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  for (unsigned int i = 0; i < mutt_array_size(magic_values); i++)
  {
    if (mutt_strcasecmp(magic_values[i], value) == 0)
    {
      *(short *) v->variable = i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid magic value: %s", value);
  return false;
}

static bool get_magic(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(result, "Variable is not a mailbox type");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  unsigned int index = *(short *) v->variable;
  if ((index < 1) || (index >= mutt_array_size(magic_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, magic_values[index]);
  return true;
}

static bool reset_magic(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(err, "Variable is not a mailbox magic");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  *(short *) v->variable = v->initial;
  return true;
}


void init_magic(void)
{
  struct ConfigSetType cst_magic = { "magic", set_magic, get_magic, reset_magic, NULL };
  cs_register_type(DT_MAGIC, &cst_magic);
}
