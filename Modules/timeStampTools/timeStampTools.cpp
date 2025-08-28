#include "timeStampTools.h"

time_t timestampToEpoch(const char* timestamp) {
    int dd, mm, yy, hh, min, ss;

    sscanf(timestamp, "%2d%2d%2d%2d%2d%2d", &dd, &mm, &yy, &hh, &min, &ss);

    struct tm t = {0};
    t.tm_mday = dd;
    t.tm_mon  = mm - 1;
    t.tm_year = 2000 + yy - 1900;
    t.tm_hour = hh;
    t.tm_min  = min;
    t.tm_sec  = ss;

    return mktime(&t); 
}


void epochToTimestamp(time_t seconds, char* timestampOut, size_t len) {
    struct tm *t = localtime(&seconds);

    snprintf(timestampOut, len, "%02d%02d%02d%02d%02d%02d",
             t->tm_mday,
             t->tm_mon + 1,        // tm_mon: 0-11 - real: 1-12
             t->tm_year % 100,     // tm_year: years since 1900
             t->tm_hour,
             t->tm_min,
             t->tm_sec);
}