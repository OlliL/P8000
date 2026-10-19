/*
 * wctime - ctime(3) as in the WEGA libc (src/lib/libc/gen/ctime.c, 2.3):
 * the time zone comes from TZ ("ESTn" or "ESTnEDT"), default EST5EDT with the
 * US daylight saving rule of that time (last Sunday in April to last Sunday in
 * October, special days in 1974 and 1975). Not the host's time zone.
 */
#ifndef WCTIME_H
#define WCTIME_H

#include <stdint.h>

/* "Thu Jan  1 00:00:00 1970\n" for t seconds since the epoch; static buffer */
char *wega_ctime(int32_t t);

#endif
