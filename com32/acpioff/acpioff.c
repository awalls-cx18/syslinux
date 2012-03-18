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
#include <stdlib.h>
#include <console.h>
#include <sys/cpu.h>
#include <acpi/acpi.h>
#include "osl.h"

acpi_status dump_tree_cb(acpi_handle obj, u32 depth, void *context, void **ret)
{
	/*
	 * This callback is called when a node is first traversed,
	 * before traversing its children.
	 */
	acpi_status status;
	struct acpi_buffer b;

	b.length = ACPI_ALLOCATE_BUFFER;
	b.pointer = NULL; 
	status = acpi_get_name(obj, ACPI_FULL_PATHNAME, &b);
	acpi_os_process_deferred_work(FALSE);

	if (ACPI_FAILURE(status)) {
		printf("acpioff: dump_tree_cb(): acpi_get_name() failed: %s\n",
		       acpi_format_exception(status));
	} else {
		printf("    %s\n", (char *)(b.pointer));
	}

	if (b.pointer != NULL)
		acpi_os_free(b.pointer);

	return AE_OK;
}

void dump_acpi_tree(void)
{
	printf("ACPI namespace tree dump:\n");
	acpi_walk_namespace(ACPI_TYPE_ANY, ACPI_ROOT_OBJECT, 0xffffffff,
			    dump_tree_cb, NULL, NULL, (void **) NULL);
	acpi_os_process_deferred_work(FALSE);
}

int main(int argc, char *argv[])
{
	acpi_status status;
	int dont_shutdown = 0;

	openconsole(&dev_stdcon_r, &dev_stdcon_w);

	if (argc > 1)
		acpi_dbg_level = strtoul(argv[1], NULL, 0);
	if (argc > 2)
		acpi_dbg_layer = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		dont_shutdown = 1;

	status = acpi_initialize_subsystem();
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_initialize_subsystem() failed: %s\n",
		       acpi_format_exception(status));
		goto err;
	}

	status = acpi_initialize_tables(NULL, 16, FALSE);
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_initialize_tables() failed: %s\n",
		       acpi_format_exception(status));
		goto err;
	}

	status = acpi_load_tables();
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_load_tables() failed: %s\n",
		       acpi_format_exception(status));
		goto err;
	}

	/*
	 * Our OSL can't easily handle the SCI interrupt, without major
	 * changes to SYSLINUX.  Maybe we shouldn't try to init events:
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
		goto err;
	}

	status = acpi_initialize_objects(ACPI_FULL_INITIALIZATION);
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		printf("acpioff: acpi_initialize_objects() failed: %s\n",
		       acpi_format_exception(status));
		goto err;
	}

	if (dont_shutdown)
	{
		dump_acpi_tree();
		goto err;
	}

	status = acpi_enter_sleep_state_prep(ACPI_STATE_S5); /* S5: poweroff */
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		dump_acpi_tree();
		printf("acpioff: acpi_enter_sleep_state_prep() failed: %s\n",
		       acpi_format_exception(status));
		goto err;
	}

	status = acpi_enter_sleep_state(ACPI_STATE_S5); /* S5: poweroff */
	/* We should not return from the above, if we power off */
	acpi_os_process_deferred_work(FALSE);
	if (ACPI_FAILURE(status))
	{
		dump_acpi_tree();
		printf("acpioff: acpi_enter_sleep_state() failed: %s\n",
		       acpi_format_exception(status));
		goto err;
	}

	dump_acpi_tree();
err:
	acpi_terminate();
	printf("acpioff: failed to shut system off. Reboot required.\n");

	while (1)
		hlt();

	return -1;
}
