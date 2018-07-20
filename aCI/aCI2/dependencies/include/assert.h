/*
 * WCRT  -  Win32API CRT
 *
 * assert.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_ASSERT_H_INCLUDED
#define WCRT_ASSERT_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG

#define assert(ignore) ((void)0)

#else /* NDEBUG */

    void wcrt_assert(const char *exp, const char *file, int line, const char *func);

#ifndef __FUNCTION__
# define __FUNCTION__ "<unknown>"
#endif

#define assert(exp) ((exp) ? (void)0 : (wcrt_assert(#exp,__FILE__,__LINE__,__FUNCTION__), (void)0))

#endif /* NDEBUG */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_ASSERT_H_INCLUDED */
