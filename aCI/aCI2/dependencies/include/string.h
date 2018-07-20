/*
 * WCRT  -  Win32API CRT
 *
 * string.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_STRING_H_INCLUDED
#define WCRT_STRING_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* TODO:
       char *strerror(unsigned int n);
    */

    char *strcpy(char *s, const char *ct);
    char *strncpy(char *s, const char *ct, size_t n);
    char *strcat(char *s, const char *ct);
    char *strncat(char *s, const char *ct, size_t n);
    int strcmp(const char *cs, const char *ct);
    int strncmp(const char *cs, const char *ct, size_t n);
    int strcoll(const char *cs, const char *ct);
    char *strchr(const char *cs, int c);
    char *strrchr(const char *cs, int c);
    size_t strspn(const char *cs, const char *ct);
    size_t strcspn(const char *cs, const char *ct);
    char *strpbrk(const char *cs, const char *ct);
    char *strstr(const char *cs, const char *ct);
    size_t strlen(const char *cs);

    char *strtok(char *s, const char *ct);

    void *memcpy(void *s, const void *ct, size_t n);
    void *memmove(void *s, const void *ct, size_t n);
    int memcmp(const void *cs, const void *ct, size_t n);
    void *memchr(const void *cs, int c, size_t n);
    void *memset(void *s, int c, size_t n);

    /* non-standard */

    char *strlwr(char *s);
    char *strupr(char *s);
    int stricmp(const char *cs, const char *ct);

    char *strtok_r(char *s, const char *ct, char **t);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_STRING_H_INCLUDED */
