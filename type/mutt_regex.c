#include <stdbool.h>
#include "mutt_regex.h"
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

struct Regex Mask;
struct Regex QuoteRegexp;
struct Regex ReplyRegexp;
struct Regex Smileys;
struct Regex GecosMask;

static void rx_destructor(void **obj)
{
  if (!obj || !*obj)
    return;

  struct Regex *r = *(struct Regex **) obj;
  FREE(&r->pattern);
  //regfree(r->rx)
  FREE(&r);
}

static bool set_rx(struct ConfigSet *set, struct HashElem *e, const char *value,
                   struct Buffer *err)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(err, "Variable is not a regex");
    return false;
  }

  return false;
}

static bool get_rx(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_RX)
  {
    mutt_buffer_printf(result, "Variable is not a regex");
    return false;
  }

  struct Regex *rx = (struct Regex *) e->data;
  mutt_buffer_addstr(result, rx->pattern);
  return true;
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


void init_regex(void)
{
  struct ConfigSetType cst_rx = { set_rx, get_rx, reset_rx, rx_destructor };
  cs_register_type("regex", DT_RX, &cst_rx);
}
