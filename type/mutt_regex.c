#include <stdbool.h>
#include "mutt_regex.h"
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

static void rx_destructor(void **obj)
{
  if (!obj || !*obj)
    return;

  struct Regex *r = (struct Regex *) obj;
  FREE(&r->pattern);
  //regfree(r->rx)
  // FREE(r);
}

static bool set_rx(struct ConfigSet *set, struct HashElem *e, const char *value,
                   struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  struct Regex *r = v->variable;
  if (!r)
    return false;

  r->rx = NULL; //XXX regenerate r->rx
  mutt_str_replace(&r->pattern, value);
  return true;
}

static bool get_rx(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(result, "Variable is not a regex");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  struct Regex *r = v->variable;
  if (!r)
    return false;

  mutt_buffer_addstr(result, r->pattern);
  return true;
}

static bool reset_rx(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  rx_destructor(v->variable);

  struct Regex *r = v->variable;
  if (!r)
    return false;

  r->rx = NULL; //XXX regenerate r->rx
  mutt_str_replace(&r->pattern, (const char*) v->initial);
  return true;
}


void init_regex(void)
{
  struct ConfigSetType cst_rx = { "regex", set_rx, get_rx, reset_rx, rx_destructor };
  cs_register_type(DT_RX, &cst_rx);
}
