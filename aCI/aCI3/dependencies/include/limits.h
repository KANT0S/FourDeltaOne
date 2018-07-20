/*
 * WCRT  -  Win32API CRT
 *
 * limits.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_LIMITS_H_INCLUDED
#define WCRT_LIMITS_H_INCLUDED

#define CHAR_BIT    8
#define SCHAR_MIN   (-128)
#define SCHAR_MAX   127
#define UCHAR_MAX   0xff

#ifdef _CHAR_UNSIGNED
# define CHAR_MIN   0
# define CHAR_MAX   UCHAR_MAX
#else
# define CHAR_MIN   SCHAR_MIN
# define CHAR_MAX   SCHAR_MAX
#endif

#define MB_LEN_MAX 5

#define SHRT_MIN    (-32768)
#define SHRT_MAX    32767
#define USHRT_MAX   0xffff
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX     2147483647
#define UINT_MAX    0xffffffff
#define LONG_MIN    (-2147483647L - 1)
#define LONG_MAX    2147483647L
#define ULONG_MAX   0xffffffffUL
#define LLONG_MAX   9223372036854775807i64
#define LLONG_MIN   (-9223372036854775807i64 - 1)
#define ULLONG_MAX  0xffffffffffffffffui64

#define _I64_MIN    LLONG_MIN
#define _I64_MAX    LLONG_MAX
#define _UI64_MIN   ULLONG_MIN

#endif /* WCRT_LIMITS_H_INCLUDED */
