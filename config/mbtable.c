#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "mbtable.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "types.h"

static struct MbTable *parse_mbtable(const char *s)
{
  struct MbTable *t = NULL;
  size_t slen, k;
  mbstate_t mbstate;
  char *d = NULL;

  t = safe_calloc(1, sizeof(struct MbTable));
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
      mutt_debug(1, "parse_mbtable: mbrtowc returned %d converting %s in %s\n",
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

static void destroy_mbtable(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct MbTable **m = (struct MbTable **) var;
  if (!*m)
    return;

  free_mbtable(m);
}

static bool set_mbtable(const struct ConfigSet *cs, void *var,
                        const struct VariableDef *vdef, const char *value,
                        struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct MbTable *table = parse_mbtable(value);

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) table, err))
  {
    free_mbtable(&table);
    return false;
  }

  destroy_mbtable(var, vdef);

  *(struct MbTable **) var = table;
  return true;
}

static bool get_mbtable(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct MbTable *table = *(struct MbTable **) var;
  if (!table)
    return true; /* empty string */

  if (!table->orig_str)
    return true; /* empty string */

  mutt_buffer_addstr(result, table->orig_str);
  return true;
}

static struct MbTable *dup_mbtable(struct MbTable *table)
{
  if (!table)
    return NULL; /* LCOV_EXCL_LINE */

  struct MbTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(table->orig_str);
  return m;
}

static bool set_native_mbtable(const struct ConfigSet *cs, void *var,
                               const struct VariableDef *vdef, intptr_t value,
                               struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  free_mbtable(var);

  struct MbTable *table = dup_mbtable((struct MbTable *) value);

  *(struct MbTable **) var = table;
  return true;
}

static intptr_t get_native_mbtable(const struct ConfigSet *cs, void *var,
                                   const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct MbTable *table = *(struct MbTable **) var;

  return (intptr_t) table;
}

static bool reset_mbtable(const struct ConfigSet *cs, void *var,
                          const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  destroy_mbtable(var, vdef);

  struct MbTable *table = parse_mbtable((const char *) vdef->initial);
  if (!table)
    return false;

  *(struct MbTable **) var = table;
  return true;
}

void free_mbtable(struct MbTable **table)
{
  if (!table || !*table)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*table)->orig_str);
  FREE(&(*table)->chars);
  FREE(&(*table)->segmented_str);
  FREE(table);
}

struct MbTable *mbtable_create(const char *str)
{
  struct MbTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(str);
  return m;
}

void init_mbtable(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_mbtable = {
    "mbtable",          set_mbtable,   get_mbtable,     set_native_mbtable,
    get_native_mbtable, reset_mbtable, destroy_mbtable,
  };
  cs_register_type(cs, DT_MBTABLE, &cst_mbtable);
}
