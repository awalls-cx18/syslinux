/* ----------------------------------------------------------------------- *
 *   
 *   Copyright 2010-2019 Intel Corporation; author: H. Peter Anvin
 *
 *   Permission is hereby granted, free of charge, to any person
 *   obtaining a copy of this software and associated documentation
 *   files (the "Software"), to deal in the Software without
 *   restriction, including without limitation the rights to use,
 *   copy, modify, merge, publish, distribute, sublicense, and/or
 *   sell copies of the Software, and to permit persons to whom
 *   the Software is furnished to do so, subject to the following
 *   conditions:
 *   
 *   The above copyright notice and this permission notice shall
 *   be included in all copies or substantial portions of the Software.
 *   
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------------------------------------------------------- */

/*
 * bitops.h
 *
 * Simple bitwise operations
 */

#ifndef _BITOPS_H
#define _BITOPS_H

#include <klibc/compiler.h>

#if defined(__i386__) || defined(__x86_64__)
# include <x86/bitops.h>
#else
static inline void set_bit(long __bit, void *__bitmap)
{
    char *__cp = (char *)__bitmap;

    __cp[__bit >> 3] |= 1 << (__bit & 7);
}

static inline void clr_bit(long __bit, void *__bitmap)
{
    char *__cp = (char *)__bitmap;

    __cp[__bit >> 3] &= ~(1 << (__bit & 7));
}

static inline _Bool __purefunc test_bit(long __bit, const void *__bitmap)
{
    const char *__cp = (char *)__bitmap;

    return (__cp[__bit >> 3] >> (__bit & 7)) & 1;
}

#endif

#endif /* _BITOPS_H */
