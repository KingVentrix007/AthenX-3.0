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