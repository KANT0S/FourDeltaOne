/*
 * WCRT  -  Win32API CRT
 *
 * wchar.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_WCHAR_H_INCLUDED
#define WCRT_WCHAR_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* TODO:
       int fwprintf(FILE *stream, const wchar_t *format, ...);
       int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...);
       int vfwprintf(FILE *stream, const wchar_t *format, va_list arg);
       int vswprintf(wchar_t *s, size_t n, const wchar_t *format, va_list arg);
       int vwprintf(const wchar_t *format, va_list arg);
       int wprintf(const wchar_t *format, ...);

       int fwscanf(FILE *stream, const wchar_t *format, ...);
       int swscanf(const wchar_t *s, const wchar_t *format, ...);
       int vfwscanf(FILE *stream, const wchar_t *format, va_list arg);
       int vswscanf(const wchar_t *s, const wchar_t *format, va_list arg);
       int vwscanf(const wchar_t *format, va_list arg);
       int wscanf(const wchar_t *format, ...);

       wint_t fgetwc(FILE *stream);
       wchar_t *fgetws(wchar_t *s, int n, FILE *stream);
       wint_t fputwc(wchar_t c, FILE *stream);
       int fputws(const wchar_t *s, FILE *stream);
       int fwide(FILE *stream, int mode);
       wint_t getwc(FILE *stream);
       wint_t getwchar(void);
       wint_t putwc(wchar_t c, FILE *stream);
       wint_t putwchar(wchar_t c);
       wint_t ungetwc(wint_t c, FILE *stream);

       double wcstod(const wchar_t *s, wchar_t **endp);
       float wcstof(const wchar_t *s, wchar_t **endp);
       long double wcstold(const wchar_t *s, wchar_t **endp);
       long int wcstol(const wchar_t *s, wchar_t **endp, int base);
       long long int wcstoll(const wchar_t *s, wchar_t **endp, int base);
       unsigned long int wcstoul(const wchar_t *s, wchar_t **endp, int base);
       unsigned long long int wcstoull(const wchar_t *s, wchar_t **endp, int base);

       size_t wcsxfrm(wchar_t *s, const wchar_t *ct, size_t n);

       size_t wcsftime(wchar_t *s, size_t smax, const wchar_t *fmt, const struct tm *tp);

       wint_t btowc(int c);
       int wctob(wint_t c);
       int mbsinit(const mbstate_t *ps);
       size_t mbrlen(const char *s, size_t n, mbstate_t *ps);
       size_t mbrtowc(wchar_t *pwc, const char *cs, size_t n, mbstate_t *ps);
       size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps);
       size_t mbsrtowcs(wchar_t *dst, const char **src, size_t len, mbstate_t *ps);
       size_t wcsrtombs(char *dst, const wchar_t **src, size_t len, mbstate_t *ps);
    */

    typedef unsigned short wchar_t;

    typedef unsigned short wint_t;

#define WCHAR_MIN (0)
#define WCHAR_MAX ((wchar_t)-1)

#define WEOF ((wint_t)-1)

    wchar_t *wcscpy(wchar_t *s, const wchar_t *ct);
    wchar_t *wcsncpy(wchar_t *s, const wchar_t *ct, size_t n);
    wchar_t *wcscat(wchar_t *s, const wchar_t *ct);
    wchar_t *wcsncat(wchar_t *s, const wchar_t *ct, size_t n);
    int wcscmp(const wchar_t *cs, const wchar_t *ct);
    int wcsncmp(const wchar_t *cs, const wchar_t *ct, size_t n);
    wchar_t *wcschr(const wchar_t *cs, wchar_t c);
    size_t wcscspn(const wchar_t *cs, const wchar_t *ct);
    wchar_t *wcspbrk(const wchar_t *cs, const wchar_t *ct);
    wchar_t *wcsrchr(const wchar_t *cs, wchar_t c);
    size_t wcsspn(const wchar_t *cs, const wchar_t *ct);
    wchar_t *wcsstr(const wchar_t *cs, const wchar_t *ct);
    wchar_t *wcstok(wchar_t *s, const wchar_t *ct, wchar_t **t);
    size_t wcslen(const wchar_t *cs);

    wchar_t *wmemcpy(wchar_t *s, const wchar_t *ct, size_t n);
    wchar_t *wmemmove(wchar_t *s, const wchar_t *ct, size_t n);
    int wmemcmp(const wchar_t *cs, const wchar_t *ct, size_t n);
    wchar_t *wmemchr(const wchar_t *cs, wchar_t c, size_t n);
    wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_WCHAR_H_INCLUDED */
