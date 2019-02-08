/*
 * -----------------------------------------------------------------------
 *
 *   Copyright 1994-2008 H. Peter Anvin - All Rights Reserved
 *   Copyright 2009-2014 Intel Corporation; author: H. Peter Anvin
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 53 Temple Place Ste 330,
 *   Boston MA 02111-1307, USA; either version 2 of the License, or
 *   (at your option) any later version; incorporated herein by reference.
 *
 * -----------------------------------------------------------------------
 *
 *
 * conio.c
 *
 * Console I/O code, except:
 *   writechr, writestr_early	- module-dependent
 *   writestr, crlf		- writestr.inc
 *   writehex*			- writehex.inc
 */
#include <sys/io.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fs.h>
#include <com32.h>
#include <x86/cpu.h>
#include <syslinux/firmware.h>

__export int pollchar(void)
{
	return firmware->i_ops->pollchar();
}

__export uint8_t kbd_shiftflags(void)
{
	if (firmware->i_ops->shiftflags)
		return firmware->i_ops->shiftflags();
	else
		return 0;	/* Unavailable on this firmware */
}

/*
 * getchar: Read a character from keyboard or serial port
 */
__export char getchar(char *hi)
{
	return firmware->i_ops->getchar(hi);
}
