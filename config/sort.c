#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sort.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/string2.h"
#include "mapping.h"
#include "mutt_options.h"

const struct Mapping SortAliasMethods[] = {
  { "alias", SORT_ALIAS }, { "address", SORT_ADDRESS }, { "unsorted", SORT_ORDER }, { NULL, 0 },
};

const struct Mapping SortAuxMethods[] = {
  { "date", SORT_DATE },
  { "date-sent", SORT_DATE },
  { "date-received", SORT_RECEIVED },
  { "mailbox-order", SORT_ORDER },
  { "subject", SORT_SUBJECT },
  { "from", SORT_FROM },
  { "size", SORT_SIZE },
  { "threads", SORT_DATE },
  { "to", SORT_TO },
  { "score", SORT_SCORE },
  { "spam", SORT_SPAM },
  { "label", SORT_LABEL },
  { NULL, 0 },
};

const struct Mapping SortBrowserMethods[] = {
  { "alpha", SORT_SUBJECT },  { "count", SORT_COUNT },
  { "date", SORT_DATE },      { "desc", SORT_DESC },
  { "new", SORT_UNREAD },     { "size", SORT_SIZE },
  { "unsorted", SORT_ORDER }, { NULL, 0 },
};

const struct Mapping SortKeyMethods[] = {
  { "address", SORT_ADDRESS }, { "date", SORT_DATE }, { "keyid", SORT_KEYID },
  { "trust", SORT_TRUST },     { NULL, 0 },
};

const struct Mapping SortMethods[] = {
  { "date", SORT_DATE },
  { "date-sent", SORT_DATE },
  { "date-received", SORT_RECEIVED },
  { "mailbox-order", SORT_ORDER },
  { "subject", SORT_SUBJECT },
  { "from", SORT_FROM },
  { "size", SORT_SIZE },
  { "threads", SORT_THREADS },
  { "to", SORT_TO },
  { "score", SORT_SCORE },
  { "spam", SORT_SPAM },
  { "label", SORT_LABEL },
  { NULL, 0 },
};

const struct Mapping SortSidebarMethods[] = {
  { "alpha", SORT_PATH },
  { "count", SORT_COUNT },
  { "desc", SORT_DESC },
  { "flagged", SORT_FLAGGED },
  { "mailbox-order", SORT_ORDER },
  { "name", SORT_PATH },
  { "new", SORT_UNREAD },
  { "path", SORT_PATH },
  { "unread", SORT_UNREAD },
  { "unsorted", SORT_ORDER },
  { NULL, 0 },
};

static const char *find_string(const struct Mapping *map, int type)
{
  if (!map)
    return NULL; /* LCOV_EXCL_LINE */

  for (int i = 0; map[i].name; i++)
    if (map[i].value == type)
      return map[i].name;

  return NULL;
}

static int find_id(const struct Mapping *map, const char *str)
{
  if (!map || !str)
    return -1; /* LCOV_EXCL_LINE */

  for (int i = 0; map[i].name; i++)
    if (mutt_strcasecmp(map[i].name, str) == 0)
      return map[i].value;

  return -1;
}

static bool set_sort(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false; /* LCOV_EXCL_LINE */

  intptr_t id = -1;

  switch (vdef->type & DT_SUBTYPE_MASK)
  {
    case DT_SORT_INDEX:
      id = find_id(SortMethods, value);
      break;
    case DT_SORT_ALIAS:
      id = find_id(SortAliasMethods, value);
      break;
    case DT_SORT_AUX:
      id = find_id(SortAuxMethods, value);
      break;
    case DT_SORT_BROWSER:
      id = find_id(SortBrowserMethods, value);
      break;
    case DT_SORT_KEYS:
      id = find_id(SortKeyMethods, value);
      break;
    case DT_SORT_SIDEBAR:
      id = find_id(SortSidebarMethods, value);
      break;
    default:
      mutt_buffer_printf(err, "Invalid sort type: %ld", vdef->type & DT_SUBTYPE_MASK);
      return false;
      break;
  }

  if (id < 0)
  {
    mutt_buffer_printf(err, "Invalid sort name: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) id, err))
    return false;

  *(short *) var = id;
  return true;
}

static bool get_sort(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  int sort = *(short *) var;

  const char *str = NULL;

  switch (vdef->type & DT_SUBTYPE_MASK)
  {
    case DT_SORT_INDEX:
      str = find_string(SortMethods, sort);
      break;
    case DT_SORT_ALIAS:
      str = find_string(SortAliasMethods, sort);
      break;
    case DT_SORT_AUX:
      str = find_string(SortAuxMethods, sort);
      break;
    case DT_SORT_BROWSER:
      str = find_string(SortBrowserMethods, sort);
      break;
    case DT_SORT_KEYS:
      str = find_string(SortKeyMethods, sort);
      break;
    case DT_SORT_SIDEBAR:
      str = find_string(SortSidebarMethods, sort);
      break;
    default:
      mutt_buffer_printf(result, "Invalid sort type: %ld", vdef->type & DT_SUBTYPE_MASK);
      return false;
      break;
  }

  if (!str)
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, str);
  return true;
}

static bool set_native_sort(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, intptr_t value,
                            struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = NULL;

  switch (vdef->type & DT_SUBTYPE_MASK)
  {
    case DT_SORT_INDEX:
      str = find_string(SortMethods, value);
      break;
    case DT_SORT_ALIAS:
      str = find_string(SortAliasMethods, value);
      break;
    case DT_SORT_AUX:
      str = find_string(SortAuxMethods, value);
      break;
    case DT_SORT_BROWSER:
      str = find_string(SortBrowserMethods, value);
      break;
    case DT_SORT_KEYS:
      str = find_string(SortKeyMethods, value);
      break;
    case DT_SORT_SIDEBAR:
      str = find_string(SortSidebarMethods, value);
      break;
    default:
      mutt_buffer_printf(err, "Invalid sort type: %ld", vdef->type & DT_SUBTYPE_MASK);
      return false;
      break;
  }

  if (!str)
  {
    mutt_buffer_printf(err, "Invalid sort type: %ld", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  *(short *) var = value;
  return true;
}

static intptr_t get_native_sort(const struct ConfigSet *cs, void *var,
                                const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

static bool reset_sort(const struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  *(short *) var = vdef->initial;
  return true;
}

void init_sorts(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_sort = {
    "sort",          set_sort,   get_sort, set_native_sort,
    get_native_sort, reset_sort, NULL,
  };
  cs_register_type(cs, DT_SORT, &cst_sort);
}