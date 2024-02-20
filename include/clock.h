#ifndef __CLOCK__H
#define __CLOCK__H
#include "stddef.h"
typedef long time_t;

typedef struct
{
    int h;
    int m;
    int s;
    // Expanded time
    int y;
    int mo;
    int d;
} TIME;

struct tm {
    int tm_sec;    // seconds after the minute (0-59)
    int tm_min;    // minutes after the hour (0-59)
    int tm_hour;   // hours since midnight (0-23)
    int tm_mday;   // day of the month (1-31)
    int tm_mon;    // months since January (0-11)
    int tm_year;   // years since 1900
    int tm_wday;   // days since Sunday (0-6)
    int tm_yday;   // days since January 1 (0-365)
    int tm_isdst;  // Daylight Saving Time flag
};

// Function prototypes
time_t time(time_t *t);
struct tm *localtime(const time_t *timer);

#endif
