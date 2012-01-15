/*
    accom32.h - ACPI Component Architecture COM32 environment configuration
            for use with acpioff.c32 and the SYSLINUX bootloader variants

    Copyright (C) 2012 Andy Walls <awalls@md.metrocast.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Reference:
 * ACPI Component Architecture
 * User Guide and Programmer [sic] Reference
 * Revision 5.01
 * December 29, 2011
 * Chapter 7: Subsystem Configuration
 */

#ifndef __ACCOM32_H__
#define __ACCOM32_H__

/*
 * Datatypes not defined by the ACPICA or COM32 headers, but unfortuinately
 * introduced by Linuxizing the ACPICA source code.
 */

#include <stdint.h>
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int32_t     s32;
typedef int64_t     s64;



/*
 * Component Selection
 */

/* #define ACPI_DISASSEMBLER */
/* #define ACPI_DEBUGGER */



/*
 * Configurable Data Types
 */

/* We don't have semaphores and spinlocks in COM32, use default of void * */
/* #define acpi_spinlock    void * */
/* #define acpi_semaphore   void * */

/*
 * We define the mutex type to be the same as a binary semaphore to avoid
 * writing the OS layer mutex functions. See ACPI_MUTEX_TYPE below.
 */
/* #define acpi_mutex       acpi_semaphore */

/* Without spinlocks, this type doesn't really matter. Use the default. */
/* #define acpi_cpu_flags   acpi_size */

/* We don't actually get to tune this type. In actype.h, it is u64. */
/* #define acpi_thread_id   u64 */

/*
 * If we don't use the ACPI local cache functions, and we don't want this to
 * be a void *, we need to define it to something else here.
 */
/* #define acpi_cache_t     void *           */
/* #define acpi_cache_t     acpi_memory_list */

/* Since COM32 defines a uintptr_t, use it here instead of void * */
#define acpi_uintptr_t    uintptr_t



/*
 * Subsystem Compile-Time Options
 */

/* COM32 provides a C library; so use it. */
#define ACPI_USE_SYSTEM_CLIBRARY
#define ACPI_USE_STANDARD_HEADERS

/* This enables ALL debug facilities within ACPICA. */
/* #define ACPI_DEBUG_OUTPUT */

/* Enable the ACPICA cache manager code.  We don't have one for ACPICA to use */
#define ACPI_USE_LOCAL_CACHE
/* Collect statistics on the cache, so the debugger can display them */
/* #define ACPI_DBG_TRACK_ALLOCATIONS */

/*
 * Even though we don't have mutexes, still indicate to implement mutexes
 * using semaphores, to avoid writing mutex related OSL functions.
 */
#define ACPI_MUTEX_TYPE    ACPI_BINARY_SEMAPHORE
/* #define ACPI_MUTEX_DEBUG */

/* Using this invites possible double evaluation of macro arguments: bad idea */
/* #define ACPI_SIMPLE_RETURN_MACROS */

/* Wrap return macros in a do { } while(0) to prevent warnings: good idea */
#define ACPI_USE_DO_WHILE_0



/*
 * Per-Compiler Configuration
 */
#define COMPILER_DEPENDENT_INT64    int64_t
#define COMPILER_DEPENDENT_UINT64   uint64_t

/* COM32 doesn't appear to provide a native 64 bit divide */
/* #define ACPI_USE_NATIVE_DIVIDE */

#define ACPI_DIV_64_BY_32(n_hi, n_lo, d32, q32, r32) \
	asm("divl %[divisor]" \
	    : [quotient] "=a" (q32), [remainder] "=d" (r32) \
	    : [divisor] "r" (d32), \
	      [dividend_lo] "0" (n_lo), [dividend_hi] "1" (n_hi))

#define ACPI_SHIFT_RIGHT_64(n_hi, n_lo) \
	asm("shrl $1, %[input_high];" \
	    "rcrl $1, %[input_low]" \
	    : [result_high] "=r" (n_hi), [result_low] "=r" (n_lo) \
	    : [input_high] "0" (n_hi), [input_low] "1" (n_lo))

/* We don't use any special annotations for functions */
/* #define ACPI_EXPORT_SYMBOL(symbol) */
/* #define ACPI_EXTERNAL_XFACE */
/* #define ACPI_INTERNAL_XFACE */
/* #define ACPI_INTERNAL_VAR_XFACE */
/* #define ACPI_SYSTEM_XFACE */

/* acgcc.h defines these for us */
/* #define ACPI_INLINE */
/* #define ACPI_PRINTF_LIKE */
/* #define ACPI_UNUSED_VAR */
#include "acgcc.h"



/*
 * Per-Machine Configuration
 */
#define ACPI_MACHINE_WIDTH    32
#define ACPI_FLUSH_CPU_CACHE() asm volatile("wbinvd": : :"memory")

/* The OS name is an obsolete legacy value that will break things if changed */
/* #define ACPI_OS_NAME   "Microsoft Windows NT" */

/* We don't have mutliple threads and locks, so let these define to default */
/* #define ACPI_ACQUIRE_GLOBAL_LOCK(facs_ptr, acquired) acquired = 1 */
/* #define ACPI_RELEASE_GLOBAL_LOCK(facs_ptr, pending) pending = 0 */



/*
 * Subsystem Runtime Configuration
 */
/* Defaults for the globals are set in acglobal.h using ACPI_INIT_GLOBAL() */



/*
 * Subsystem Configuration Constants
 */
/*
 * The constants may be modified at compile time by changing acconfig.h,
 * or at runtime by changing the contents of the global variables where
 * the constants are stored.
 */

#endif  /* __ACCOM32_H__ */
