#include "clock.h"
TIME get_time()
{
    int second;
    int minute;
    int hour;
    int day;
    int month;
    int year;
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);
    second = (second & 0x0F) + ((second / 16) * 10);
    minute = (minute & 0x0F) + ((minute / 16) * 10);
    hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
    day = (day & 0x0F) + ((day / 16) * 10);
    month = (month & 0x0F) + ((month / 16) * 10);
    year = (year & 0x0F) + ((year / 16) * 10);
    TIME time;
    time.h = hour+2;
    time.m = minute;
    time.s = second;
    time.y = year;
    time.mo = month;
    time.d = day;
    // printf("time: %d/%d/%d\n",day,month,year);
    // printf("time: %d.%d:%d\n",hour+2,minute,second);
    return time;
}

time_t time(time_t *t)
{
    TIME current_time = get_time();
    // Convert the current time to seconds since the Epoch
    // Adjusting for hours, minutes, and seconds
    time_t seconds = (current_time.y - 1970) * 31536000 + (current_time.mo - 1) * 2592000 + (current_time.d - 1) * 86400 +
                     current_time.h * 3600 + current_time.m * 60 + current_time.s;
    
    if (t != NULL) {
        *t = seconds;
    }
    
    return seconds;
}
struct tm *localtime(const time_t *timer)
{
    static struct tm local_time;
    time_t current_time = *timer;

    // Perform conversion to local time
    // This part may vary depending on your system's implementation
    // For simplicity, assuming no timezone adjustments
    local_time.tm_sec = current_time % 60;
    local_time.tm_min = (current_time / 60) % 60;
    local_time.tm_hour = (current_time / 3600) % 24;
    local_time.tm_mday = (current_time / 86400) % 31 + 1; // Assuming a 31-day month
    local_time.tm_mon = ((current_time / 2592000) % 12) + 1; // Assuming 30 days per month
    local_time.tm_year = 1970 + (current_time / 31536000); // Assuming a non-leap year
    local_time.tm_wday = 0; // Not calculating the day of the week
    local_time.tm_yday = 0; // Not calculating the day of the year
    local_time.tm_isdst = 0; // Not considering Daylight Saving Time
    
    return &local_time;
}