#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include "mbyte_table.h"
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

static void free_mbchartbl(struct MbCharTable **table)
{
  FREE(&(*table)->orig_str);
  FREE(&(*table)->chars);
  FREE(&(*table)->segmented_str);
  FREE(table);
}

static void destroy_mbchartbl(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return;

  struct MbCharTable **m = (struct MbCharTable **) var;
  if (!*m)
    return;

  free_mbchartbl(m);
}

static bool set_mbchartbl(struct ConfigSet *cs, void *var,
                          const struct VariableDef *vdef, const char *value,
                          struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  struct MbCharTable *table = parse_mbchar_table(value);
  if (!table)
  {
    //XXX error message
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) table, err))
  {
    free_mbchartbl(&table);
    return false;
  }

  destroy_mbchartbl(var, vdef);

  *(struct MbCharTable **) var = table;
  return true;
}

static bool get_mbchartbl(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  struct MbCharTable *table = *(struct MbCharTable **) var;
  if (!table)
    return true; /* empty string */

  if (!table->orig_str)
    return true; /* empty string */

  mutt_buffer_addstr(result, table->orig_str);
  return true;
}

static bool set_native_mbchartbl(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  return false;
}

static intptr_t get_native_mbchartbl(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  return -1;
}

static bool reset_mbchartbl(struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  destroy_mbchartbl(var, vdef);

  struct MbCharTable *table = parse_mbchar_table((const char *) vdef->initial);
  if (!table)
    return false;

  *(struct MbCharTable **) var = table;
  return true;
}

void init_mbyte_table(void)
{
  const struct ConfigSetType cst_mbchartbl = { "mbtable", set_mbchartbl, get_mbchartbl, set_native_mbchartbl, get_native_mbchartbl, reset_mbchartbl, destroy_mbchartbl, };
  cs_register_type(DT_MBCHARTBL, &cst_mbchartbl);
}
