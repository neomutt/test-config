#include <stdbool.h>
#include "buffer.h"
#include "lib.h"
#include "mutt_options.h"
#include "hash.h"
#include "mbyte_table.h"
#include "config_set.h"

static void mbchartbl_destructor(void **obj)
{
  if (!obj || !*obj)
    return;

  struct MbCharTable *m = *(struct MbCharTable **) obj;
  FREE(&m->segmented_str);
  FREE(&m->orig_str);
  FREE(&m);
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

static bool reset_mbchartbl(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_MBCHARTBL)
  {
    mutt_buffer_printf(err, "Variable is not a multibyte string");
    return false;
  }

  return false;
}


void init_mbyte_table(void)
{
  struct ConfigSetType cst_mbchartbl = { set_mbchartbl, get_mbchartbl, reset_mbchartbl, mbchartbl_destructor };
  cs_register_type("mbtable", DT_MBCHARTBL, &cst_mbchartbl);
}
