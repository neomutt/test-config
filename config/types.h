#ifndef _CONFIG_TYPES_H
#define _CONFIG_TYPES_H

#define DT_MASK      0x0f
#define DT_BOOL       1   /**< boolean option */
#define DT_NUM        2   /**< a number */
#define DT_STR        3   /**< a string */
#define DT_PATH       4   /**< a pathname */
#define DT_QUAD       5   /**< quad-option (yes/no/ask-yes/ask-no) */
#define DT_SORT       6   /**< sorting methods */
#define DT_REGEX      7   /**< regular expressions */
#define DT_MAGIC      8   /**< mailbox type */
#define DT_SYN        9   /**< synonym for another variable */
#define DT_ADDR      10   /**< e-mail address */
#define DT_MBTABLE   11   /**< multibyte char table */
#define DT_HCACHE    12   /**< header cache backend */

#define DTYPE(x) ((x) & DT_MASK)

/* subtypes */
#define DT_SUBTYPE_MASK 0xff0
#define DT_SORT_INDEX   0x000
#define DT_SORT_ALIAS   0x010
#define DT_SORT_BROWSER 0x020
#define DT_SORT_KEYS    0x040
#define DT_SORT_AUX     0x080
#define DT_SORT_SIDEBAR 0x100

#define DT_INHERITED    0x200
#define DT_INITIAL_SET  0x400

#endif /* _CONFIG_TYPES_H */
