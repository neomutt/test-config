#include "mbyte_table.h"
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static struct MbCharTable *parse_mbchar_table(const char *s)
{
  struct MbCharTable *t = NULL;
  size_t slen, k;
  mbstate_t mbstate;
  char *d = NULL;

  t = safe_calloc(1, sizeof(struct MbCharTable));
  slen = mutt_strlen(s);
  if (!slen)
    return t;

  t->orig_str = safe_strdup(s);
  /* This could be more space efficient.  However, being used on tiny
   * strings (Tochars and StChars), the overhead is not great. */
  t->chars = safe_calloc(slen, sizeof(char *));
  d = t->segmented_str = safe_calloc(slen * 2, sizeof(char));

  memset(&mbstate, 0, sizeof(mbstate));
  while (slen && (k = mbrtowc(NULL, s, slen, &mbstate)))
  {
    if (k == (size_t)(-1) || k == (size_t)(-2))
    {
      mutt_debug(
          1, "parse_mbchar_table: mbrtowc returned %d converting %s in %s\n",
          (k == (size_t)(-1)) ? -1 : -2, s, t->orig_str);
      if (k == (size_t)(-1))
        memset(&mbstate, 0, sizeof(mbstate));
      k = (k == (size_t)(-1)) ? 1 : slen;
    }

    slen -= k;
    t->chars[t->len++] = d;
    while (k--)
      *d++ = *s++;
    *d++ = '\0';
  }

  return t;
}

static void destroy_mbchartbl(void **obj, struct VariableDef *def)
{
  if (!obj || !*obj)
    return;

  // struct MbCharTable *m = *(struct MbCharTable **) obj;
  //XXX FREE(&m->chars);
  //XXX FREE(&m->segmented_str);
  //XXX FREE(&m->orig_str);
  //XXX FREE(&m);
}

static bool set_mbchartbl(struct ConfigSet *set, void *variable, struct VariableDef *def,
                          const char *value, struct Buffer *err)
{
  // if (DTYPE(e->type) != DT_MBCHARTBL)
  // {
  //   mutt_buffer_printf(err, "Variable is not a multibyte string");
  //   return false;
  // }

  // struct VariableDef *def = e->data;
  // if (!def)
  //   return false;

  struct MbCharTable *table = parse_mbchar_table(value);
  if (!table)
    return false;

  // destroy_mbchartbl(def->variable);
  // *(struct MbCharTable **) def->variable = table;
  *(struct MbCharTable **) variable = table;
  return true;
}

static bool get_mbchartbl(struct HashElem *e, struct Buffer *result)
{
  // if (DTYPE(e->type) != DT_MBCHARTBL)
  // {
  //   mutt_buffer_printf(result, "Variable is not a multibyte string");
  //   return false;
  // }

  // struct VariableDef *v = e->data;
  // if (!v)
  //   return false;

  void *variable = e;

  // struct MbCharTable *table = *(struct MbCharTable **) v->variable;
  struct MbCharTable *table = *(struct MbCharTable **) variable;
  if (!table)
    return true; /* empty string */

  if (!table->orig_str)
    return true; /* empty string */

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

  struct VariableDef *def = e->data;
  if (!def)
    return false;

  struct MbCharTable *table = parse_mbchar_table((const char*) def->initial);
  if (!table)
    return false;

  destroy_mbchartbl(def->variable, def);
  *(struct MbCharTable **) def->variable = table;
  return true;
}


void init_mbyte_table(void)
{
  struct ConfigSetType cst_mbchartbl = { "mbtable", set_mbchartbl, get_mbchartbl,
                                         reset_mbchartbl, destroy_mbchartbl };
  cs_register_type(DT_MBCHARTBL, &cst_mbchartbl);
}
