/**
 * @file
 * Time and date handling routines
 *
 * @authors
 * Copyright (C) 1996-2000 Michael R. Elkins <me@mutt.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @page date Time and date handling routines
 *
 * Some commonly used time and date functions.
 *
 * | Function              | Description
 * | :-------------------- | :--------------------------------------------------
 * | mutt_local_tz()       | Calculate the local timezone in seconds east of UTC
 * | mutt_make_date()      | Write a date in RFC822 format to a buffer
 * | mutt_mktime()         | Convert `struct tm` to `time_t`
 * | mutt_normalize_time() | Fix the contents of a struct tm
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* theoretically time_t can be float but it is integer on most (if not all) systems */
#define TIME_T_MAX ((((time_t) 1 << (sizeof(time_t) * 8 - 2)) - 1) * 2 + 1)
#define TM_YEAR_MAX                                                            \
  (1970 + (((((TIME_T_MAX - 59) / 60) - 59) / 60) - 23) / 24 / 366)

const char *const Weekdays[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};
const char *const Months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec", "ERR",
};

/**
 * compute_tz - Calculate the number of seconds east of UTC
 * @param g   Local time
 * @param utc UTC time
 * @retval number Seconds east of UTC
 *
  * returns the seconds east of UTC given 'g' and its corresponding gmtime()
 * representation
 */
static time_t compute_tz(time_t g, struct tm *utc)
{
  struct tm *lt = localtime(&g);
  time_t t;
  int yday;

  t = (((lt->tm_hour - utc->tm_hour) * 60) + (lt->tm_min - utc->tm_min)) * 60;

  if ((yday = (lt->tm_yday - utc->tm_yday)))
  {
    /* This code is optimized to negative timezones (West of Greenwich) */
    if ((yday == -1) || /* UTC passed midnight before localtime */
        (yday > 1))     /* UTC passed new year before localtime */
      t -= (24 * 60 * 60);
    else
      t += (24 * 60 * 60);
  }

  return t;
}

/**
 * is_leap_year_feb - Is a given February in a leap year
 * @param tm Date to be tested
 * @retval true if it's a leap year
 */
static int is_leap_year_feb(struct tm *tm)
{
  if (tm->tm_mon == 1)
  {
    int y = tm->tm_year + 1900;
    return (((y & 3) == 0) && (((y % 100) != 0) || ((y % 400) == 0)));
  }
  return 0;
}

/**
 * mutt_local_tz - Calculate the local timezone in seconds east of UTC
 * @param t Time to examine
 * @retval num Seconds east of UTC
 *
 * Returns the local timezone in seconds east of UTC for the time t,
 * or for the current time if t is zero.
 */
time_t mutt_local_tz(time_t t)
{
  struct tm *ptm = NULL;
  struct tm utc;

  if (!t)
    t = time(NULL);
  ptm = gmtime(&t);
  /* need to make a copy because gmtime/localtime return a pointer to
     static memory (grr!) */
  memcpy(&utc, ptm, sizeof(utc));
  return (compute_tz(t, &utc));
}

/**
 * mutt_mktime - Convert `struct tm` to `time_t`
 * @param t     Time to convert
 * @param local Should the local timezone be considered
 * @retval num Time in Unix format
 *
 * Convert a struct tm to time_t, but don't take the local timezone into
 * account unless ``local'' is nonzero
 */
time_t mutt_mktime(struct tm *t, int local)
{
  time_t g;

  static const int AccumDaysPerMonth[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
  };

  /* Prevent an integer overflow.
   * The time_t cast is an attempt to silence a clang range warning. */
  if ((time_t) t->tm_year > TM_YEAR_MAX)
    return TIME_T_MAX;

  /* Compute the number of days since January 1 in the same year */
  g = AccumDaysPerMonth[t->tm_mon % 12];

  /* The leap years are 1972 and every 4. year until 2096,
   * but this algorithm will fail after year 2099 */
  g += t->tm_mday;
  if ((t->tm_year % 4) || t->tm_mon < 2)
    g--;
  t->tm_yday = g;

  /* Compute the number of days since January 1, 1970 */
  g += (t->tm_year - 70) * (time_t) 365;
  g += (t->tm_year - 69) / 4;

  /* Compute the number of hours */
  g *= 24;
  g += t->tm_hour;

  /* Compute the number of minutes */
  g *= 60;
  g += t->tm_min;

  /* Compute the number of seconds */
  g *= 60;
  g += t->tm_sec;

  if (local)
    g -= compute_tz(g, t);

  return g;
}

/**
 * mutt_normalize_time - Fix the contents of a struct tm
 * @param tm Time to correct
 *
 * If values have been added/subtracted from a struct tm, it can lead to
 * invalid dates, e.g.  Adding 10 days to the 25th of a month.
 *
 * This function will correct any over/under-flow.
 */
void mutt_normalize_time(struct tm *tm)
{
  static const char DaysPerMonth[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
  };
  int nLeap;

  while (tm->tm_sec < 0)
  {
    tm->tm_sec += 60;
    tm->tm_min--;
  }
  while (tm->tm_sec >= 60)
  {
    tm->tm_sec -= 60;
    tm->tm_min++;
  }
  while (tm->tm_min < 0)
  {
    tm->tm_min += 60;
    tm->tm_hour--;
  }
  while (tm->tm_min >= 60)
  {
    tm->tm_min -= 60;
    tm->tm_hour++;
  }
  while (tm->tm_hour < 0)
  {
    tm->tm_hour += 24;
    tm->tm_mday--;
  }
  while (tm->tm_hour >= 24)
  {
    tm->tm_hour -= 24;
    tm->tm_mday++;
  }
  /* use loops on NNNdwmy user input values? */
  while (tm->tm_mon < 0)
  {
    tm->tm_mon += 12;
    tm->tm_year--;
  }
  while (tm->tm_mon >= 12)
  {
    tm->tm_mon -= 12;
    tm->tm_year++;
  }
  while (tm->tm_mday <= 0)
  {
    if (tm->tm_mon)
      tm->tm_mon--;
    else
    {
      tm->tm_mon = 11;
      tm->tm_year--;
    }
    tm->tm_mday += DaysPerMonth[tm->tm_mon] + is_leap_year_feb(tm);
  }
  while (tm->tm_mday > (DaysPerMonth[tm->tm_mon] + (nLeap = is_leap_year_feb(tm))))
  {
    tm->tm_mday -= DaysPerMonth[tm->tm_mon] + nLeap;
    if (tm->tm_mon < 11)
      tm->tm_mon++;
    else
    {
      tm->tm_mon = 0;
      tm->tm_year++;
    }
  }
}

/**
 * mutt_make_date - Write a date in RFC822 format to a buffer
 * @param buf    Buffer for result
 * @param buflen Length of buffer
 * @retval ptr Buffer containing result
 */
char *mutt_make_date(char *buf, size_t buflen)
{
  time_t t = time(NULL);
  struct tm *l = localtime(&t);
  time_t tz = mutt_local_tz(t);

  tz /= 60;

  snprintf(buf, buflen, "Date: %s, %d %s %d %02d:%02d:%02d %+03d%02d\n",
           Weekdays[l->tm_wday], l->tm_mday, Months[l->tm_mon], l->tm_year + 1900,
           l->tm_hour, l->tm_min, l->tm_sec, (int) tz / 60, (int) abs((int) tz) % 60);
  return buf;
}

