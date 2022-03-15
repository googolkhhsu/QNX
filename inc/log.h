#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#define FMT "%02d-%02d %02d:%02d:%02d.%03d"
#define INFO __FILE__, __func__, __LINE__
#define MSGFMT "%s [%s.%s#%d] %s\n"
#define dlog(msg)                                                   \
    {                                                               \
        time_t p;                                                   \
        struct tm *T;                                               \
        struct timeval t;                                           \
        time(&p);                                                   \
        gettimeofday(&t, NULL);                                     \
        T = localtime(&t.tv_sec);                                   \
        char time[30];                                              \
        int msec = t.tv_usec / 1000;                                \
        snprintf(time, sizeof time, FMT, T->tm_mon + 1, T->tm_mday, \
                 T->tm_hour, T->tm_min, T->tm_sec, msec);           \
        printf(MSGFMT, time, INFO, msg);                            \
    }
#define DLOG(fmt, msg)
#define DLOG2(c, ...)
#endif