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
 * | Data                  | Description
 * | :-------------------- | :--------------------------------------------------
 * | #Months               | Months of the year (abbreviated)
 * | #TimeZones            | Lookup table of Time Zones
 * | #Weekdays             | Day of the week (abbreviated)
 *
 * | Function              | Description
 * | :-------------------- | :--------------------------------------------------
 * | is_day_name()         | Is the string a valid day name
 * | mutt_check_month()    | Is the string a valid month name
 * | mutt_local_tz()       | Calculate the local timezone in seconds east of UTC
 * | mutt_make_date()      | Write a date in RFC822 format to a buffer
 * | mutt_mktime()         | Convert `struct tm` to `time_t`
 * | mutt_normalize_time() | Fix the contents of a struct tm
 * | mutt_parse_date()     | Parse a date string in RFC822 format
 */

#include "config.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "date.h"
#include "debug.h"
#include "string2.h"

/* theoretically time_t can be float but it is integer on most (if not all) systems */
#define TIME_T_MAX ((((time_t) 1 << (sizeof(time_t) * 8 - 2)) - 1) * 2 + 1)
#define TM_YEAR_MAX                                                            \
  (1970 + (((((TIME_T_MAX - 59) / 60) - 59) / 60) - 23) / 24 / 366)

/**
 * Weekdays - Day of the week (abbreviated)
 */
const char *const Weekdays[] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

/**
 * Months - Months of the year (abbreviated)
 */
const char *const Months[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
  "Aug", "Sep", "Oct", "Nov", "Dec", "ERR",
};

// clang-format off
/**
 * TimeZones - Lookup table of Time Zones
 *
 * @note Keep in alphabetical order
 */
const struct Tz TimeZones[] = {
  { "aat",   1,  0, true  }, /* Atlantic Africa Time */
  { "adt",   4,  0, false }, /* Arabia DST */
  { "ast",   3,  0, false }, /* Arabia */
//{ "ast",   4,  0, true  }, /* Atlantic */
  { "bst",   1,  0, false }, /* British DST */
  { "cat",   1,  0, false }, /* Central Africa */
  { "cdt",   5,  0, true  },
  { "cest",  2,  0, false }, /* Central Europe DST */
  { "cet",   1,  0, false }, /* Central Europe */
  { "cst",   6,  0, true  },
//{ "cst",   8,  0, false }, /* China */
//{ "cst",   9, 30, false }, /* Australian Central Standard Time */
  { "eat",   3,  0, false }, /* East Africa */
  { "edt",   4,  0, true  },
  { "eest",  3,  0, false }, /* Eastern Europe DST */
  { "eet",   2,  0, false }, /* Eastern Europe */
  { "egst",  0,  0, false }, /* Eastern Greenland DST */
  { "egt",   1,  0, true  }, /* Eastern Greenland */
  { "est",   5,  0, true  },
  { "gmt",   0,  0, false },
  { "gst",   4,  0, false }, /* Presian Gulf */
  { "hkt",   8,  0, false }, /* Hong Kong */
  { "ict",   7,  0, false }, /* Indochina */
  { "idt",   3,  0, false }, /* Israel DST */
  { "ist",   2,  0, false }, /* Israel */
//{ "ist",   5, 30, false }, /* India */
  { "jst",   9,  0, false }, /* Japan */
  { "kst",   9,  0, false }, /* Korea */
  { "mdt",   6,  0, true  },
  { "met",   1,  0, false }, /* This is now officially CET */
  { "msd",   4,  0, false }, /* Moscow DST */
  { "msk",   3,  0, false }, /* Moscow */
  { "mst",   7,  0, true  },
  { "nzdt", 13,  0, false }, /* New Zealand DST */
  { "nzst", 12,  0, false }, /* New Zealand */
  { "pdt",   7,  0, true  },
  { "pst",   8,  0, true  },
  { "sat",   2,  0, false }, /* South Africa */
  { "smt",   4,  0, false }, /* Seychelles */
  { "sst",  11,  0, true  }, /* Samoa */
//{ "sst",   8,  0, false }, /* Singapore */
  { "utc",   0,  0, false },
  { "wat",   0,  0, false }, /* West Africa */
  { "west",  1,  0, false }, /* Western Europe DST */
  { "wet",   0,  0, false }, /* Western Europe */
  { "wgst",  2,  0, true  }, /* Western Greenland DST */
  { "wgt",   3,  0, true  }, /* Western Greenland */
  { "wst",   8,  0, false }, /* Western Australia */
};
// clang-format on

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
 * uncomment_timezone - Strip ()s from a timezone string
 * @param buf    Buffer to store the results
 * @param buflen Length of buffer
 * @param tz     Timezone string to copy
 * @retval ptr Results buffer
 *
 * Some time formats have the timezone in ()s, e.g. (MST) or (-0700)
 *
 * @note If the timezone doesn't have any ()s the function will return a
 *       pointer to the original string.
 */
static const char *uncomment_timezone(char *buf, size_t buflen, const char *tz)
{
  char *p = NULL;
  size_t len;

  if (*tz != '(')
    return tz; /* no need to do anything */
  tz = skip_email_wsp(tz + 1);
  p = strpbrk(tz, " )");
  if (!p)
    return tz;
  len = p - tz;
  if (len > (buflen - 1))
    len = buflen - 1;
  memcpy(buf, tz, len);
  buf[len] = 0;
  return buf;
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

/**
 * mutt_check_month - Is the string a valid month name
 * @param s - String to check
 * @retval num Index into Months array (0-based)
 * @retval -1  Error
 *
 * @note Only the first three characters are checked
 * @note The comparison is case insensitive
 */
int mutt_check_month(const char *s)
{
  for (int i = 0; i < 12; i++)
    if (mutt_strncasecmp(s, Months[i], 3) == 0)
      return i;

  return -1; /* error */
}

/**
 * is_day_name - Is the string a valid day name
 * @param s - String to check
 * @retval boolean
 *
 * @note Only the first three characters are checked
 * @note The comparison is case insensitive
 */
bool is_day_name(const char *s)
{
  if ((strlen(s) < 3) || !*(s + 3) || !ISSPACE(*(s + 3)))
    return false;

  for (int i = 0; i < 7; i++)
    if (mutt_strncasecmp(s, Weekdays[i], 3) == 0)
      return true;

  return false;
}

/**
 * mutt_parse_date - Parse a date string in RFC822 format
 * @param[in]  s      String to parse
 * @param[out] tz_out Pointer to timezone (optional)
 * @retval num Unix time in seconds
 *
 * Parse a date of the form:
 * `[ weekday , ] day-of-month month year hour:minute:second [ timezone ]`
 *
 * The 'timezone' field is optional; it defaults to +0000 if missing.
 */
time_t mutt_parse_date(const char *s, const struct Tz **tz_out)
{
  int count = 0;
  char *t = NULL;
  int hour, min, sec;
  struct tm tm;
  int i;
  int tz_offset = 0;
  int zhours = 0;
  int zminutes = 0;
  bool zoccident = false;
  const char *ptz = NULL;
  char tzstr[SHORT_STRING];
  char scratch[SHORT_STRING];

  /* Don't modify our argument. Fixed-size buffer is ok here since
   * the date format imposes a natural limit.
   */

  strfcpy(scratch, s, sizeof(scratch));

  /* kill the day of the week, if it exists. */
  if ((t = strchr(scratch, ',')))
    t++;
  else
    t = scratch;
  t = skip_email_wsp(t);

  memset(&tm, 0, sizeof(tm));

  while ((t = strtok(t, " \t")) != NULL)
  {
    switch (count)
    {
      case 0: /* day of the month */
        if ((mutt_atoi(t, &tm.tm_mday) < 0) || (tm.tm_mday < 0))
          return -1;
        if (tm.tm_mday > 31)
          return -1;
        break;

      case 1: /* month of the year */
        i = mutt_check_month(t);
        if (i < 0)
          return -1;
        tm.tm_mon = i;
        break;

      case 2: /* year */
        if ((mutt_atoi(t, &tm.tm_year) < 0) || (tm.tm_year < 0))
          return -1;
        if (tm.tm_year < 50)
          tm.tm_year += 100;
        else if (tm.tm_year >= 1900)
          tm.tm_year -= 1900;
        break;

      case 3: /* time of day */
        if (sscanf(t, "%d:%d:%d", &hour, &min, &sec) == 3)
          ;
        else if (sscanf(t, "%d:%d", &hour, &min) == 2)
          sec = 0;
        else
        {
          mutt_debug(1, "parse_date: could not process time format: %s\n", t);
          return -1;
        }
        tm.tm_hour = hour;
        tm.tm_min = min;
        tm.tm_sec = sec;
        break;

      case 4: /* timezone */
        /* sometimes we see things like (MST) or (-0700) so attempt to
         * compensate by uncommenting the string if non-RFC822 compliant
         */
        ptz = uncomment_timezone(tzstr, sizeof(tzstr), t);

        if (*ptz == '+' || *ptz == '-')
        {
          if (ptz[1] && ptz[2] && ptz[3] && ptz[4] &&
              isdigit((unsigned char) ptz[1]) && isdigit((unsigned char) ptz[2]) &&
              isdigit((unsigned char) ptz[3]) && isdigit((unsigned char) ptz[4]))
          {
            zhours = (ptz[1] - '0') * 10 + (ptz[2] - '0');
            zminutes = (ptz[3] - '0') * 10 + (ptz[4] - '0');

            if (ptz[0] == '-')
              zoccident = true;
          }
        }
        else
        {
          struct Tz *tz = NULL;

          /* This is safe to do: A pointer to a struct equals a pointer to its first element */
          tz = bsearch(ptz, TimeZones, sizeof(TimeZones) / sizeof(struct Tz),
                       sizeof(struct Tz),
                       (int (*)(const void *, const void *)) mutt_strcasecmp);

          if (tz)
          {
            zhours = tz->zhours;
            zminutes = tz->zminutes;
            zoccident = tz->zoccident;

            if (tz_out)
              *tz_out = tz;
          }

          /* ad hoc support for the European MET (now officially CET) TZ */
          if (mutt_strcasecmp(t, "MET") == 0)
          {
            if ((t = strtok(NULL, " \t")) != NULL)
            {
              if (mutt_strcasecmp(t, "DST") == 0)
                zhours++;
            }
          }
        }
        tz_offset = (zhours * 3600) + (zminutes * 60);
        if (!zoccident)
          tz_offset = -tz_offset;
        break;
    }
    count++;
    t = 0;
  }

  if (count < 4) /* don't check for missing timezone */
  {
    mutt_debug(
        1, "parse_date(): error parsing date format, using received time\n");
    return -1;
  }

  return (mutt_mktime(&tm, 0) + tz_offset);
}
