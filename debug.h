#ifndef _MUTT_DEBUG_H
#define _MUTT_DEBUG_H

struct Hash;
struct ConfigSet;

void hash_dump(struct Hash *table);
void cs_dump_set(const struct ConfigSet *cs);

#endif /* _MUTT_DEBUG_H */

