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

static bool addr_set_string(struct HashElem *e, const char *value, struct Buffer *err)
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

static bool bool_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(err, "Variable is not an address");
    return false;
  }

  return false;
}

static bool magic_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MAGIC)
  {
    mutt_buffer_printf(err, "Variable is not a mailbox type");
    return false;
  }

  return false;
}

static bool mbchartbl_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MBCHARTBL)
  {
    mutt_buffer_printf(err, "Variable is not a multibyte string");
    return false;
  }

  return false;
}

static bool num_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_NUM)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  return false;
}

static bool path_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(err, "Variable is not a path");
    return false;
  }

  return false;
}

static bool quad_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_QUAD)
  {
    mutt_buffer_printf(err, "Variable is not a quad");
    return false;
  }

  return false;
}

static bool rx_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  return false;
}

static bool sort_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_SORT)
  {
    mutt_buffer_printf(err, "Variable is not a sort");
    return false;
  }

  return false;
}

static bool str_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  return false;
}


static const char *addr_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *bool_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *magic_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *mbchartbl_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *num_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *path_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *quad_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *rx_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *sort_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}

static const char *str_get_string(struct HashElem *e, struct Buffer *err)
{
  return NULL;
}


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
  FREE(&a);

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
  FREE(&m);
  return true;
}

static bool path_destructor(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(err, "Variable is not a path");
    return false;
  }

  const char *str = (const char *) e->data;
  FREE(&str);

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
  FREE(&r);

  return true;
}

static bool str_destructor(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  const char *str = (const char *) e->data;
  FREE(&str);

  return true;
}


bool init_variables(void)
{
  struct ConfigSetType cst_addr      = { addr_set_string,      addr_get_string,      addr_destructor      };
  struct ConfigSetType cst_bool      = { bool_set_string,      bool_get_string,      NULL                 };
  struct ConfigSetType cst_magic     = { magic_set_string,     magic_get_string,     NULL                 };
  struct ConfigSetType cst_mbchartbl = { mbchartbl_set_string, mbchartbl_get_string, mbchartbl_destructor };
  struct ConfigSetType cst_num       = { num_set_string,       num_get_string,       NULL                 };
  struct ConfigSetType cst_path      = { path_set_string,      path_get_string,      path_destructor      };
  struct ConfigSetType cst_quad      = { quad_set_string,      quad_get_string,      NULL                 };
  struct ConfigSetType cst_rx        = { rx_set_string,        rx_get_string,        rx_destructor        };
  struct ConfigSetType cst_sort      = { sort_set_string,      sort_get_string,      NULL                 };
  struct ConfigSetType cst_str       = { str_set_string,       str_get_string,       str_destructor       };

  cs_register_type("address", DT_ADDR,      &cst_addr);
  cs_register_type("boolean", DT_BOOL,      &cst_bool);
  cs_register_type("magic",   DT_MAGIC,     &cst_magic);
  cs_register_type("mbtable", DT_MBCHARTBL, &cst_mbchartbl);
  cs_register_type("number",  DT_NUM,       &cst_num);
  cs_register_type("path",    DT_PATH,      &cst_path);
  cs_register_type("quad",    DT_QUAD,      &cst_quad);
  cs_register_type("regex",   DT_RX,        &cst_rx);
  cs_register_type("sort",    DT_SORT,      &cst_sort);
  cs_register_type("string",  DT_STR,       &cst_str);

  return true;
}

#if 0
static bool hc_string_set(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_HCACHE)
  {
    mutt_buffer_printf(err, "Variable is not an hcache");
    return false;
  }

  intptr_t index = -1;

  for (int i = 0; i < mutt_array_size(hcache_backends); i++)
  {
    if (strcmp(value, hcache_backends[i]) == 0)
    {
      index = i;
      break;
    }
  }

  if (index < 0)
  {
    mutt_buffer_printf(err, "Not a valid hcache backend: %s", value);
    return false;
  }

  e->data = (void *) index;
  return true;
}

static const char *hc_string_get(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_HCACHE)
  {
    mutt_buffer_printf(err, "Variable is not an hcache");
    return NULL;
  }

  int index = (intptr_t) e->data;
  if ((index < 0) || (index >= mutt_array_size(hcache_backends)))
  {
    mutt_buffer_printf(err, "Invalid hcache value: %d", index);
    return NULL;
  }

  return hcache_backends[index];
}



#endif
