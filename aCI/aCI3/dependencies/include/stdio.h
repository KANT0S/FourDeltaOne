/*
 * WCRT  -  Win32API CRT
 *
 * stdio.h
 *
 * Copyright (c) 2003-2011 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 */

#ifndef WCRT_STDIO_H_INCLUDED
#define WCRT_STDIO_H_INCLUDED

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* TODO:
       FILE *tmpfile(void);

       int setvbuf(FILE *stream, char *buf, int mode, size_t size);
       void setbuf(FILE *stream, char *buf);

       void clearerr(FILE *stream);
       int feof(FILE *stream);
       int ferror(FILE *stream);
       void perror(const char *s);
    */

    typedef struct {
        void *hFile;
        int unget;
    } FILE;

    typedef struct {
        unsigned int sizel;
        unsigned int sizeh;
    } fpos_t;

#define EOF (-1)

#define FILENAME_MAX 260

    extern FILE wcrt_stdin;
    extern FILE wcrt_stdout;
    extern FILE wcrt_stderr;

#define stdin  (&wcrt_stdin)
#define stdout (&wcrt_stdout)
#define stderr (&wcrt_stderr)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define L_tmpnam 16
#define TMP_MAX  32767

    FILE *freopen(const char *filename, const char *mode, FILE *stream);
    FILE *fopen(const char *filename, const char *mode);
    int fflush(FILE *stream);
    int fclose(FILE *stream);

    int remove(const char *filename);
    int rename(const char *oldname, const char *newname);

    char *tmpnam(char *s);

    int fprintf(FILE *stream, const char *format, ...);
    int printf(const char *format, ...);
    int sprintf(char *s, const char *format, ...);
    int snprintf(char *s, size_t n, const char *format, ...);

    int vfprintf(FILE *stream, const char *format, va_list arg);
    int vprintf(const char *format, va_list arg);
    int vsprintf(char *s, const char *format, va_list arg);
    int vsnprintf(char *s, size_t n, const char *format, va_list arg);

    int fscanf(FILE *stream, const char *format, ...);
    int scanf(const char *format, ...);
    int sscanf(char *s, const char *format, ...);

    int fgetc(FILE *stream);
    char *fgets(char *s, int n, FILE *stream);

    int fputc(int c, FILE *stream);
    int fputs(const char *s, FILE *stream);

#define getc(fp)   fgetc(fp)
#define getchar()  getc(stdin)

    char *gets(char *s);

#define putc(c,fp) fputc(c,fp)
#define putchar(c) putc(c,stdout)

    int puts(const char *s);

    int ungetc(int c, FILE *stream);

    size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream);
    size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream);

    int fseek(FILE *stream, long offset, int origin);
    long ftell(FILE *stream);
    void rewind(FILE *stream);

    int fgetpos(FILE *stream, fpos_t *ptr);
    int fsetpos(FILE *stream, const fpos_t *ptr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WCRT_STDIO_H_INCLUDED */
