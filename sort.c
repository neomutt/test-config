#include "buffer.h"
#include "mutt_options.h"
#include "mapping.h"
#include "config_set.h"
#include "sort.h"
#include "hash.h"

const struct Mapping SortMethods[] = {
  { "date",             SORT_DATE },
  { "date-sent",        SORT_DATE },
  { "date-received",    SORT_RECEIVED },
  { "mailbox-order",    SORT_ORDER },
  { "subject",          SORT_SUBJECT },
  { "from",             SORT_FROM },
  { "size",             SORT_SIZE },
  { "threads",          SORT_THREADS },
  { "to",               SORT_TO },
  { "score",            SORT_SCORE },
  { "spam",             SORT_SPAM },
  { "label",            SORT_LABEL },
  { NULL,               0 },
};

const struct Mapping SortAuxMethods[] = {
  { "date",             SORT_DATE },
  { "date-sent",        SORT_DATE },
  { "date-received",    SORT_RECEIVED },
  { "mailbox-order",    SORT_ORDER },
  { "subject",          SORT_SUBJECT },
  { "from",             SORT_FROM },
  { "size",             SORT_SIZE },
  { "threads",          SORT_DATE },    
  { "to",               SORT_TO },
  { "score",            SORT_SCORE },
  { "spam",             SORT_SPAM },
  { "label",            SORT_LABEL },
  { NULL,               0 },
};

const struct Mapping SortBrowserMethods[] = {
  { "alpha",    SORT_SUBJECT },
  { "count",    SORT_COUNT },
  { "date",     SORT_DATE },
  { "desc",     SORT_DESC },
  { "new",      SORT_UNREAD },
  { "size",     SORT_SIZE },
  { "unsorted", SORT_ORDER },
  { NULL,       0 },
};

const struct Mapping SortAliasMethods[] = {
  { "alias",    SORT_ALIAS },
  { "address",  SORT_ADDRESS },
  { "unsorted", SORT_ORDER },
  { NULL,       0 },
};

const struct Mapping SortKeyMethods[] = {
  { "address",  SORT_ADDRESS },
  { "date",     SORT_DATE },
  { "keyid",    SORT_KEYID },
  { "trust",    SORT_TRUST },
  { NULL,       0 },
};

const struct Mapping SortSidebarMethods[] = {
  { "alpha",            SORT_PATH },
  { "count",            SORT_COUNT },
  { "desc",             SORT_DESC },
  { "flagged",          SORT_FLAGGED },
  { "mailbox-order",    SORT_ORDER },
  { "name",             SORT_PATH },
  { "new",              SORT_UNREAD },  
  { "path",             SORT_PATH },
  { "unread",           SORT_UNREAD },
  { "unsorted",         SORT_ORDER },
  { NULL,               0 },
};


static bool sort_set_string(struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_SORT)
  {
    mutt_buffer_printf(err, "Variable is not a sort");
    return false;
  }

  return false;
}

static bool sort_get_string(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_SORT)
  {
    mutt_buffer_printf(result, "Variable is not a sort");
    return false;
  }

  // pgp_sort_keys       DT_SORT|DT_SORT_KEYS    PgpSortKeys       SORT_ADDRESS
  // sidebar_sort_method DT_SORT|DT_SORT_SIDEBAR SidebarSortMethod SORT_ORDER
  // sort                DT_SORT                 Sort              SORT_DATE
  // sort_alias          DT_SORT|DT_SORT_ALIAS   SortAlias         SORT_ALIAS
  // sort_aux            DT_SORT|DT_SORT_AUX     SortAux           SORT_DATE
  // sort_browser        DT_SORT|DT_SORT_BROWSER BrowserSort       SORT_ALPHA

  return false;
}

bool init_sorts(void)
{
  struct ConfigSetType cst_sort = { sort_set_string, sort_get_string, NULL };

  cs_register_type("sort", DT_SORT, &cst_sort);
  return true;
}


