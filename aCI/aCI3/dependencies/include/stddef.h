/*
 * WCRT  -  Win32API CRT
 *
 * stddef.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_STDDEF_H_INCLUDED
#define WCRT_STDDEF_H_INCLUDED

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
# define NULL 0
#else
# define NULL ((void *)0)
#endif
#endif

#ifndef  __cplusplus

#ifdef _WIN64
# define WCRT_INT_T __int64
#else
# define WCRT_INT_T int
#endif

#ifndef WCRT_SIZE_T_DEFINED
# define WCRT_SIZE_T_DEFINED
    typedef unsigned WCRT_INT_T size_t;
#endif

#ifndef WCRT_PTRDIFF_T_DEFINED
# define WCRT_PTRDIFF_T_DEFINED
    typedef WCRT_INT_T ptrdiff_t;
#endif

#ifndef WCRT_WCHAR_T_DEFINED
# define WCRT_WCHAR_T_DEFINED
    typedef unsigned short wchar_t;
#endif

#endif /* __cplusplus */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_STDDEF_H_INCLUDED */
