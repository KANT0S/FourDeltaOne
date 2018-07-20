/*
 * WCRT  -  Win32API CRT
 *
 * math.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_MATH_H_INCLUDED
#define WCRT_MATH_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* TODO:
       double frexp(double x, int *exp);
    */

    double sin(double x);
    double cos(double x);
    double tan(double x);
    double asin(double x);
    double acos(double x);
    double atan(double x);
    double atan2(double y, double x);
    double sinh(double x);
    double cosh(double x);
    double tanh(double x);
    double exp(double x);
    double exp2(double x);
    double log(double x);
    double log10(double x);
    double log2(double x);
    double pow(double x, double y);

    double sqrt(double x);
    double ceil(double x);
    double floor(double x);
    double round(double x);
    double trunc(double x);
    double fabs(double x);
    double ldexp(double x, int n);
    double modf(double x, double *ip);
    double fmod(double x, double y);

    float sinf(float x);
    float cosf(float x);
    float tanf(float x);
    float asinf(float x);
    float acosf(float x);
    float atanf(float x);
    float atan2f(float y, float x);
    float sinhf(float x);
    float coshf(float x);
    float tanhf(float x);
    float expf(float x);
    float exp2f(float x);
    float logf(float x);
    float log10f(float x);
    float log2f(float x);
    float powf(float x, float y);

    float sqrtf(float x);
    float ceilf(float x);
    float floorf(float x);
    float roundf(float x);
    float truncf(float x);
    float fabsf(float x);
    float ldexpf(float x, int n);
    float modff(float x, float *ip);
    float fmodf(float x, float y);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_MATH_H_INCLUDED */
