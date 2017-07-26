#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "config_set.h"
#include "mutt_options.h"
#include "buffer.h"
#include "hash.h"
#include "lib.h"
#include "address.h"
#include "mbyte_table.h"
#include "mutt_regex.h"

const char *bool_values[] = { "no", "yes" };
const char *quad_values[] = { "no", "yes", "ask-no", "ask-yes" };

static bool addr_destructor(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_ADDR)
  {
    mutt_buffer_printf(err, "Variable is not an address");
    return false;
  }

  struct Address *a = (struct Address *) e->data;
  FREE(&a->personal);
  FREE(&a->mailbox);

  FREE(&e->data);
  return true;
}

static bool mbchartbl_destructor(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MBCHARTBL)
  {
    mutt_buffer_printf(err, "Variable is not a multibyte string");
    return false;
  }

  struct MbCharTable *m = (struct MbCharTable *) e->data;
  FREE(&m->segmented_str);
  FREE(&m->orig_str);

  FREE(&e->data);
  return true;
}

static bool path_destructor(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(err, "Variable is not a path");
    return false;
  }

  FREE(&e->data);
  return true;
}

static bool rx_destructor(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  struct Regex *r = (struct Regex *) e->data;
  FREE(&r->pattern);
  //regfree(r->rx)

  FREE(&e->data);
  return true;
}

static bool str_destructor(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  FREE(&e->data);
  return true;
}


static bool set_addr(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
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

static bool set_bool(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(err, "Variable is not a boolean");
    return false;
  }

  for (intptr_t i = 0; i < mutt_array_size(bool_values); i++)
  {
    if (strcasecmp(bool_values[i], value) == 0)
    {
      e->data = (void *) i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid boolean value: %s", value);
  return false;
}

static bool set_magic(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(err, "Variable is not a mailbox type");
    return false;
  }

  return false;
}

static bool set_mbchartbl(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MBCHARTBL)
  {
    mutt_buffer_printf(err, "Variable is not a multibyte string");
    return false;
  }

  return false;
}

static bool set_num(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_NUM)
  {
    mutt_buffer_printf(err, "Variable is not a number");
    return false;
  }

  int num = 0;
  if (mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return false;
  }

  intptr_t copy = num;
  e->data = (void *) copy;
  return true;
}

static bool set_path(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(err, "Variable is not a path");
    return false;
  }

  return false;
}

static bool set_quad(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_QUAD)
  {
    mutt_buffer_printf(err, "Variable is not a quad");
    return false;
  }

  for (intptr_t i = 0; i < mutt_array_size(quad_values); i++)
  {
    if (strcasecmp(quad_values[i], value) == 0)
    {
      e->data = (void *) i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid quad value: %s", value);
  return false;
}

static bool set_rx(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  return false;
}

static bool set_str(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (e && DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  // if (e)
  // {
  //   if (!str_destructor(e, err))
  //     return false;

  //   e->data = (void *) value;
  // }
  // else
  // {
  //   e = hash_typed_insert(hash, name, DT_STR, (void *) value);
  // }

  return false;
}


static bool get_addr(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_ADDR)
  {
    mutt_buffer_printf(result, "Variable is not an address");
    return false;
  }

  mutt_buffer_addstr(result, (const char*) e->data);
  return true;
}

static bool get_bool(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(result, "Variable is not an address");
    return false;
  }

  intptr_t index = (intptr_t) e->data;
  if ((index < 0) || (index > mutt_array_size(bool_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, bool_values[index]);
  return true;
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

static bool get_mbchartbl(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_MBCHARTBL)
  {
    mutt_buffer_printf(result, "Variable is not a multibyte string");
    return false;
  }

  struct MbCharTable *table = (struct MbCharTable*) e->data;
  mutt_buffer_addstr(result, table->orig_str);
  return true;
}

static bool get_num(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_NUM)
  {
    mutt_buffer_printf(result, "Variable is not a string");
    return false;
  }

  mutt_buffer_printf(result, "%d", (intptr_t) e->data);
  return true;
}

static bool get_path(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(result, "Variable is not a path");
    return false;
  }

  mutt_buffer_addstr(result, (const char*) e->data);
  return true;
}

static bool get_quad(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_QUAD)
  {
    mutt_buffer_printf(result, "Variable is not a quad");
    return false;
  }

  intptr_t index = (intptr_t) e->data;
  if ((index < 0) || (index > mutt_array_size(quad_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, quad_values[index]);
  return true;
}

static bool get_rx(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(result, "Variable is not a regex");
    return false;
  }

  struct Regex *rx = (struct Regex*) e->data;
  mutt_buffer_addstr(result, rx->pattern);
  return true;
}

static bool get_str(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(result, "Variable is not a string");
    return false;
  }

  mutt_buffer_addstr(result, (const char*) e->data);
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

static bool reset_mbchartbl(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MBCHARTBL)
  {
    mutt_buffer_printf(err, "Variable is not a multibyte string");
    return false;
  }

  return false;
}

static bool reset_path(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(err, "Variable is not a path");
    return false;
  }

  return false;
}

static bool reset_rx(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  return false;
}

static bool reset_str(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (e && DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  // if (e)
  // {
  //   if (!str_destructor(e, err))
  //     return false;

  //   e->data = (void *) value;
  // }
  // else
  // {
  //   e = hash_typed_insert(hash, name, DT_STR, (void *) value);
  // }

  return false;
}


int Percentage;
char *PrintCommand;
#define UL (intptr_t)

bool percentage_validator(struct ConfigSet *set, const char *name, int type, intptr_t value, struct Buffer *result)
{
  return false;
}

struct VariableDef vars[] =
{
  { "percentage",    DT_NUM, UL &Percentage,   UL 10,    percentage_validator },
  { "print_command", DT_STR, UL &PrintCommand, UL "lpr", NULL },
  { NULL },
};

bool init_types(struct ConfigSet *set)
{
  struct ConfigSetType cst_addr      = { set_addr,      get_addr,      reset_addr,      addr_destructor      };
  struct ConfigSetType cst_bool      = { set_bool,      get_bool,      NULL,            NULL                 };
  struct ConfigSetType cst_magic     = { set_magic,     get_magic,     NULL,            NULL                 };
  struct ConfigSetType cst_mbchartbl = { set_mbchartbl, get_mbchartbl, reset_mbchartbl, mbchartbl_destructor };
  struct ConfigSetType cst_num       = { set_num,       get_num,       NULL,            NULL                 };
  struct ConfigSetType cst_path      = { set_path,      get_path,      reset_path,      path_destructor      };
  struct ConfigSetType cst_quad      = { set_quad,      get_quad,      NULL,            NULL                 };
  struct ConfigSetType cst_rx        = { set_rx,        get_rx,        reset_rx,        rx_destructor        };
  struct ConfigSetType cst_str       = { set_str,       get_str,       reset_str,       str_destructor       };

  cs_register_type("address", DT_ADDR,      &cst_addr);
  cs_register_type("boolean", DT_BOOL,      &cst_bool);
  cs_register_type("magic",   DT_MAGIC,     &cst_magic);
  cs_register_type("mbtable", DT_MBCHARTBL, &cst_mbchartbl);
  cs_register_type("number",  DT_NUM,       &cst_num);
  cs_register_type("path",    DT_PATH,      &cst_path);
  cs_register_type("quad",    DT_QUAD,      &cst_quad);
  cs_register_type("regex",   DT_RX,        &cst_rx);
  cs_register_type("string",  DT_STR,       &cst_str);

  cs_register_variable("percentage", DT_NUM, "10", NULL);

  cs_register_variables(set, vars);

  return true;
}


