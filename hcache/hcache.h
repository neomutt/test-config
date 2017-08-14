#ifndef _MUTT_HCACHE_H
#define _MUTT_HCACHE_H

struct ConfigSet;

void init_hcache(struct ConfigSet *cs);

extern char *HeaderCachePageSize;

#endif /* _MUTT_HCACHE_H */
