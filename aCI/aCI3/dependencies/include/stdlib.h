/*
 * WCRT  -  Win32API CRT
 *
 * stdlib.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_STDLIB_H_INCLUDED
#define WCRT_STDLIB_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int quot;
        int rem;
    } div_t;

    typedef struct {
        long quot;
        long rem;
    } ldiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX 0x7fff

    double atof(const char *s);
    int atoi(const char *s);
    long atol(const char *s);

    double strtod(const char *s, char **endp);
    long strtol(const char *s, char **endp, int base);
    unsigned long strtoul(const char *s, char **endp, int base);
    __int64 strtoll(const char *s, char **endp, int base);
    unsigned __int64 strtoull(const char *s, char **endp, int base);

    int rand(void);
    void srand(unsigned int seed);

    void *calloc(size_t nobj, size_t size);
    void *malloc(size_t size);
    void *realloc(void *p, size_t size);
    void free(void *p);

    void abort(void);
    void exit(int status);
    int atexit(void (*fcn)(void));

    char *getenv(const char *name);

    int system(const char *s);

    void *bsearch(const void *key, const void *base,
                  size_t n, size_t size,
                  int (*cmp)(const void *keyval, const void *datum));

    void qsort(void *base, size_t n, size_t size,
               int (*cmp)(const void *, const void *));

    int abs(int n);
    long labs(long n);

    div_t div(int num, int denom);
    ldiv_t ldiv(long num, long denom);

    /* non-standard */

    __int64 _strtoi64(const char *s, char **endp, int base);
    unsigned __int64 _strtoui64(const char *s, char **endp, int base);

    int rand_r(unsigned int *seed);
    int rand_s(void);

    unsigned int _rotl(unsigned int val, int shift);
    unsigned int _rotr(unsigned int val, int shift);
    unsigned long _lrotl(unsigned long val, int shift);
    unsigned long _lrotr(unsigned long val, int shift);
    unsigned __int64 _rotl64(unsigned __int64 val, int shift);
    unsigned __int64 _rotr64(unsigned __int64 val, int shift);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_STDLIB_H_INCLUDED */
