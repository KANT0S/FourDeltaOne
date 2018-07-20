/*
 * WCRT  -  Win32API CRT
 *
 * time.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_TIME_H_INCLUDED
#define WCRT_TIME_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

    /* TODO:
       time_t time(time_t *tp);

       double difftime(time_t time2, time_t time1);

       time_t mktime(struct tm *tp);

       char *ctime(const time_t *tp);
       struct tm *gmtime(const time_t *tp);
       struct tm *localtime(const time_t *tp);
       size_t strftime(char *s, size_t smax,
                       const char *fmt, const struct tm *tp));
    */

    typedef unsigned int clock_t;

    typedef unsigned int time_t;

#define CLOCKS_PER_SEC 1000

    struct tm {
        int tm_sec;
        int tm_min;
        int tm_hour;
        int tm_mday;
        int tm_mon;
        int tm_year;
        int tm_wday;
        int tm_yday;
        int tm_isdst;
    };

    clock_t clock(void);

    char *asctime(const struct tm *tp);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_TIME_H_INCLUDED */
