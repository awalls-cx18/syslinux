/*
 * vsprintf.c
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int vsprintf(char *buffer, const char *format, va_list ap)
{
    return vsnprintf(buffer, INT_MAX, format, ap);
}
