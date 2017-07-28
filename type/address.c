#include "address.h"
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

static void addr_destructor(void **obj)
{
  if (!obj || !*obj)
    return;

  struct Address *a = *(struct Address **) obj;
  FREE(&a->personal);
  FREE(&a->mailbox);
  FREE(&a);
}

static bool set_addr(struct ConfigSet *set, struct HashElem *e,
                     const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_ADDR)
  {
    mutt_buffer_printf(err, "Variable is not an address");
    return false;
  }

  // enum ConfigEvent e = CE_CHANGED;
  // intptr_t copy = (intptr_t) value;

  // if (elem)
  // {
  //   if (set->destructor && !set->destructor(set, DT_ADDR, (intptr_t) elem->data))
  //     FREE(&elem->data);
  //   elem->data = (void *) value;
  // }
  // else
  // {
  //   e = CE_SET;
  //   elem = hash_typed_insert(set->hash, name, DT_ADDR, (void *) value);
  // }

  // notify_listeners(set, name, e);
  // return elem;
  return false;
}

static bool get_addr(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_ADDR)
  {
    mutt_buffer_printf(result, "Variable is not an address");
    return false;
  }

  mutt_buffer_addstr(result, (const char *) e->data);
  return true;
}

static bool reset_addr(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_ADDR)
  {
    mutt_buffer_printf(err, "Variable is not an address");
    return false;
  }

  return false;
}


void init_addr(void)
{
  struct ConfigSetType cst_addr = { "address", set_addr, get_addr, reset_addr, addr_destructor };
  cs_register_type(DT_ADDR, &cst_addr);
}
