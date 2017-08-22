#include "config.h"
#include <string.h>
#include "lib/lib.h"
#include "config_set.h"
#include "inheritance.h"
#include "mutt_options.h"

void hash_dump(struct Hash *table)
{
  struct HashElem *he = NULL;

  for (int i = 0; i < table->nelem; i++)
  {
    he = table->table[i];
    if (!he)
      continue;

    printf("%03d ", i);
    for (; he; he = he->next)
    {
      if (he->type & DT_INHERITED)
      {
        struct Inheritance *i = he->data;
        printf("\033[1;32m[%s]\033[m ", i->name);
      }
      else
      {
        printf("%s ", *(char**) he->data);
      }
    }
    printf("\n");
  }
}

void cs_dump_set(struct ConfigSet *cs)
{
  struct HashElem *he = NULL;
  struct HashWalkState state;
  memset(&state, 0, sizeof(state));

  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = safe_calloc(1, STRING);
  result.dsize = STRING;

  while ((he = hash_walk(cs->hash, &state)))
  {
    const char *name = NULL;

    if (he->type & DT_INHERITED)
    {
      struct Inheritance *i = he->data;
      he = i->parent;
      name = i->name;
    }
    else
    {
      name = he->key.strkey;
    }

    struct ConfigSetType *cst = cs_get_type_def(cs, he->type);
    if (!cst)
    {
      printf("Unknown type: %d\n", he->type);
      continue;
    }

    mutt_buffer_reset(&result);
    printf("%s %s", cst->name, name);

    struct VariableDef *vdef = he->data;

    if (cst->getter(vdef->var, vdef, &result))
      printf(" = %s\n", result.data);
    else
      printf(": ERROR: %s\n", result.data);
  }

  FREE(&result.data);
}
