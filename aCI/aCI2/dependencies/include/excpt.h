/*
 * WCRT  -  Win32API CRT
 *
 * excpt.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_EXCPT_H_INCLUDED
#define WCRT_EXCPT_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* Win SDK 7.1 windows.h includes crtdefs.h, this file is here
       to prevent the compiler from complaining about it */

    /* Exception disposition return values */
    typedef enum _EXCEPTION_DISPOSITION {
        ExceptionContinueExecution,
        ExceptionContinueSearch,
        ExceptionNestedException,
        ExceptionCollidedUnwind
    } EXCEPTION_DISPOSITION;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_EXCPT_H_INCLUDED */
