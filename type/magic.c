#include <stdbool.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

static bool set_magic(struct ConfigSet *set, struct HashElem *e,
                      const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(err, "Variable is not a mailbox type");
    return false;
  }

  return false;
}

static bool get_magic(struct HashElem *e, struct Buffer *result)
{
  const char *text[] = { NULL, "mbox", "MMDF", "MH", "Maildir" };

  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(result, "Variable is not a mailbox type");
    return false;
  }

  intptr_t index = (intptr_t) e->data;
  if ((index < 1) || (index > mutt_array_size(text)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, text[index]);
  return true;
}

static bool reset_magic(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(err, "Variable is not a mailbox magic");
    return false;
  }

  return false;
}


void init_magic(void)
{
  struct ConfigSetType cst_magic = { "magic", set_magic, get_magic, reset_magic, NULL };
  cs_register_type(DT_MAGIC, &cst_magic);
}
