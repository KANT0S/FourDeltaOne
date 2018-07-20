/*
 * WCRT  -  Win32API CRT
 *
 * ctype.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_CTYPE_H_INCLUDED
#define WCRT_CTYPE_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    int isalnum(int c);
    int isalpha(int c);
    int iscntrl(int c);
    int ispunct(int c);
    int isspace(int c);
    int isxdigit(int c);

#define isdigit(c) (((c) - '0') < 10U)
#define isgraph(c) (((c) - '!') < 94U)
#define islower(c) (((c) - 'a') < 26U)
#define isprint(c) (((c) - ' ') < 95U)
#define isupper(c) (((c) - 'A') < 26U)

    int tolower(int c);
    int toupper(int c);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_CTYPE_H_INCLUDED */
