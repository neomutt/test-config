#ifndef _MUTT_SORTS_H
#define _MUTT_SORTS_H

#include <stdbool.h>

#define SORT_DATE     1 /* the date the mail was sent. */
#define SORT_SIZE     2
#define SORT_SUBJECT  3
#define SORT_ALPHA    3 /* makedoc.c requires this */
#define SORT_FROM     4
#define SORT_ORDER    5 /* the order the messages appear in the mailbox. */
#define SORT_THREADS  6
#define SORT_RECEIVED 7 /* when the message were delivered locally */
#define SORT_TO       8
#define SORT_SCORE    9
#define SORT_ALIAS    10
#define SORT_ADDRESS  11
#define SORT_KEYID    12
#define SORT_TRUST    13
#define SORT_SPAM     14
#define SORT_COUNT    15
#define SORT_UNREAD   16
#define SORT_FLAGGED  17
#define SORT_PATH     18
#define SORT_LABEL    19
#define SORT_DESC     20

bool init_sorts(void);

#endif /* _MUTT_SORTS_H */
