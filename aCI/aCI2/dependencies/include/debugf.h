/*
 * WCRT  -  Win32API CRT
 *
 * debugf.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_DEBUGF_H_INCLUDED
#define WCRT_DEBUGF_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUGF

#define debugf(ignore) ((void)0)

#else /* NDEBUGF */

    char * __cdecl wcrt_debugf_format(const char *format, ...);

    void __cdecl wcrt_debugf_out(const char *file, int line, const char *func, char *s);

#ifndef __FUNCTION__
# define __FUNCTION__ "<unknown>"
#endif

#define debugf(args) wcrt_debugf_out(__FILE__, __LINE__, __FUNCTION__, wcrt_debugf_format args)

#endif /* NDEBUGF */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_DEBUGF_H_INCLUDED */
