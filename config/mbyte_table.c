#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "mbyte_table.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "types.h"

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

static void destroy_mbchartbl(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct MbCharTable **m = (struct MbCharTable **) var;
  if (!*m)
    return;

  free_mbchartbl(m);
}

static bool set_mbchartbl(const struct ConfigSet *cs, void *var,
                          const struct VariableDef *vdef, const char *value,
                          struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct MbCharTable *table = parse_mbchar_table(value);

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
    return false; /* LCOV_EXCL_LINE */

  struct MbCharTable *table = *(struct MbCharTable **) var;
  if (!table)
    return true; /* empty string */

  if (!table->orig_str)
    return true; /* empty string */

  mutt_buffer_addstr(result, table->orig_str);
  return true;
}

static struct MbCharTable *dup_mbchartbl(struct MbCharTable *table)
{
  if (!table)
    return NULL; /* LCOV_EXCL_LINE */

  struct MbCharTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(table->orig_str);
  return m;
}

static bool set_native_mbchartbl(const struct ConfigSet *cs, void *var,
                                 const struct VariableDef *vdef, intptr_t value,
                                 struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  free_mbchartbl(var);

  struct MbCharTable *table = dup_mbchartbl((struct MbCharTable *) value);

  *(struct MbCharTable **) var = table;
  return true;
}

static intptr_t get_native_mbchartbl(const struct ConfigSet *cs, void *var,
                                     const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct MbCharTable *table = *(struct MbCharTable **) var;

  return (intptr_t) table;
}

static bool reset_mbchartbl(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  destroy_mbchartbl(var, vdef);

  struct MbCharTable *table = parse_mbchar_table((const char *) vdef->initial);
  if (!table)
    return false;

  *(struct MbCharTable **) var = table;
  return true;
}

void free_mbchartbl(struct MbCharTable **table)
{
  if (!table || !*table)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*table)->orig_str);
  FREE(&(*table)->chars);
  FREE(&(*table)->segmented_str);
  FREE(table);
}

struct MbCharTable *mb_create(const char *str)
{
  struct MbCharTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(str);
  return m;
}

void init_mbyte_table(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_mbchartbl = {
    "mbtable", set_mbchartbl, get_mbchartbl, set_native_mbchartbl, get_native_mbchartbl, reset_mbchartbl, destroy_mbchartbl,
  };
  cs_register_type(cs, DT_MBCHARTBL, &cst_mbchartbl);
}
