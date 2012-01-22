/*
    acpioff.c - COM32 program to turn off the system via ACPI

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
 */

#include <stdio.h>
#include <console.h>
#include <acpi/acpi.h>
#include "osl.h"

int main(int argc, char *argv[])
{
	acpi_status status;

	openconsole(&dev_stdcon_r, &dev_stdcon_w);

	status = acpi_initialize_subsystem();
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_initialize_subsystem() failed: %s\n",
		       acpi_format_exception(status));
		acpi_terminate();
		return status;
	}

	status = acpi_initialize_tables(NULL, 16, FALSE);
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_initialize_tables() failed: %s\n",
		       acpi_format_exception(status));
		acpi_terminate();
		return status;
	}

	/*
	 * Our OSL can't easily handle the SCI interrupt, without major
	 * changes to SYSLINUX
	 *
	 * status =
	 *       acpi_enable_subsystem(ACPI_NO_EVENT_INIT|ACPI_NO_HANDLER_INIT);
	 */
	status = acpi_enable_subsystem(ACPI_FULL_INITIALIZATION);
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_enable_subsystem() failed: %s\n",
		       acpi_format_exception(status));
		acpi_terminate();
		return status;
	}

	status = acpi_initialize_objects(ACPI_FULL_INITIALIZATION);
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_initialize_objects() failed: %s\n",
		       acpi_format_exception(status));
		acpi_terminate();
		return status;
	}

	status = acpi_enter_sleep_state_prep(ACPI_STATE_S5); /* S5: poweroff */
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_enter_sleep_state_prep() failed: %s\n",
		       acpi_format_exception(status));
		acpi_terminate();
		return status;
	}

	status = acpi_enter_sleep_state(ACPI_STATE_S5); /* S5: poweroff */
	/* We should return from the above, if we power off */
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_enter_sleep_state() failed: %s\n",
		       acpi_format_exception(status));
		acpi_terminate();
		return status;
	}

	acpi_terminate();
	return 0;
}
