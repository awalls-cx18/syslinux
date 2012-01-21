/*
    osl.h - ACPI Component Architecture OS Services Layer custom exported
            functions for use with acpioff.c32 and the SYSLINUX variants

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
 * Chapter 9: OS Services Layer - External Interface Definitnion
 */

#ifndef _OSL_H_
#define _OSL_H_

/*
 * This ACPICA/OSL is single threaded.  After calling ACPICA API functions,
 * a caller should call
 *
 * 	acpi_os_process_deferred_work(FALSE);
 *
 * to finish up the deferred work the ACPICA has scheduled.
 */
int acpi_os_process_deferred_work(int noexec);

#endif /* _OSL_H_ */
