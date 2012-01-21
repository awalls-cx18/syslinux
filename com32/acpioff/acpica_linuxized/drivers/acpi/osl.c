
/*
    osl.c - ACPI Component Architecture required OS Services Layer functions
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
 * Chapter 9: OS Services Layer - External Interface Definitnion
 */

#include <acpi/acpi.h>
#include <sys/pci.h>
#include <unistd.h>
#include <sys/times.h>
#include <stdio.h>

#define _COMPONENT		ACPI_OS_SERVICES
ACPI_MODULE_NAME("osl");

/*
 * Doubly-linked List
 */
struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

static void _acpi_os_list_add(struct list_head *new,
                              struct list_head *prev, struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static void acpi_os_list_add_tail(struct list_head *new, struct list_head *head)
{
	_acpi_os_list_add(new, head->prev, head);
}


static void _acpi_os_list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static void acpi_os_list_del(struct list_head *entry)
{
	_acpi_os_list_del(entry->prev, entry->next);
}

static int acpi_os_list_empty(struct list_head *head)
{
	return (head->next == head);
}

struct acpi_os_deferred_work_e {
	struct list_head	list; /* must be first element */
	acpi_execute_type       type;
	acpi_osd_exec_callback  fn;
	void                   *priv;
};

static struct list_head acpi_os_deferred_work = { &acpi_os_deferred_work,
                                                  &acpi_os_deferred_work };

struct acpi_os_irq {
	u32               irqn;
	acpi_osd_handler  fn;
	void             *priv;
};

static struct acpi_os_irq acpi_os_sci_irq = { 0xffffffff, NULL, NULL };

static enum pci_config_type acpi_os_pci_config_type = PCI_CFG_NONE;

static FILE *acpi_osl_printf_dest = stdout;

int acpi_os_process_deferred_work(int noexec)
{
	int i = 0;
	struct acpi_os_deferred_work_e *work;
	struct list_head *p;

	while (!acpi_os_list_empty(&acpi_os_deferred_work)) {

		p = acpi_os_deferred_work.next;
		acpi_os_list_del(p);

		work = (struct acpi_os_deferred_work_e *) p;
		if (!noexec && work->fn != NULL)
			work->fn(work->priv);
		free(work);
		i++;
	}
	ACPI_INFO((AE_INFO, "%s %u deferred work item(s)",
	           noexec ? "Deallocated" : "Processed", i));
	return i;
}



/*
 * 9.1 Environmental and ACPI Tables
 */
acpi_status acpi_os_initialize(void)
{
	/*
	 * OSL initializes itself here.
	 * Called during initialization of the ACPICA subsystem.
	 */
	acpi_os_deferred_work.next = &acpi_os_deferred_work;
	acpi_os_deferred_work.prev = &acpi_os_deferred_work;

	acpi_os_sci_irq.irqn = 0xffffffff;
	acpi_os_sci_irq.fn   = NULL;
	acpi_os_sci_irq.priv = NULL;

        acpi_os_pci_config_type = pci_set_config_type(PCI_CFG_AUTO);

	acpi_osl_printf_dest = stdout;

	return AE_OK;
}

acpi_status acpi_os_terminate(void)
{
	/*
	 * OSL cleans itself up and terminates here.
	 * Called during termination of the ACPICA subsystem.
	 */

	/* Walk deferred work list and deallocate things at least */
	acpi_os_process_deferred_work(1);

	/* FIXME: unhook the IRQ handler if not done already */
	/*
	 * FIXME: do magic to halt actual interrupt firing and dereg.
	 */
	acpi_os_sci_irq.irqn = 0xffffffff;
	acpi_os_sci_irq.fn   = NULL;
	acpi_os_sci_irq.priv = NULL;

        acpi_os_pci_config_type = PCI_CFG_NONE;
	return AE_OK;
}

acpi_physical_address acpi_os_get_root_pointer(void)
{
	acpi_physical_address rsdp = 0;
	acpi_status status;
	acpi_size table_addr = 0;
	
	/*
	 * TODO: EFI firmwares should give us the RSDP location without
	 * having to search.
	 */
	status = acpi_find_root_pointer(&table_addr);
	switch (status) {
	case AE_OK:
		rsdp = ACPI_PTR_TO_PHYSADDR(ACPI_TO_POINTER(table_addr));
		ACPI_INFO((AE_INFO,
		           "RSDP found at physical addr 0x%8.8x", rsdp));
		break;
	case AE_NOT_FOUND:
		ACPI_ERROR((AE_INFO, "Unable to find RSDP in either "
		                     "Extended BIOS Data Area or BIOS ROM"));
		break;
	case AE_NO_MEMORY:
		ACPI_ERROR((AE_INFO, "Not enough memory to search for RSDP in "
		                     "Extended BIOS Data Area or BIOS ROM"));
		break;
	default:
		ACPI_EXCEPTION((AE_INFO, status, "Unexpected failure searching "
		            "for RSDP in Extended BIOS Data Area or BIOS ROM"));
		break;
	}
	return rsdp;
}

acpi_status acpi_os_predefined_override(
				   const struct acpi_predefined_names *init_val,
				   acpi_string * new_val)
{
	if (init_val == NULL || new_val == NULL)
		return AE_BAD_PARAMETER;

	/* We do not override any predefined objects in the ACPI namespace */
	*new_val = NULL;
	return AE_OK;
}

acpi_status acpi_os_table_override(struct acpi_table_header *existing_table,
				   struct acpi_table_header **new_table)
{
	if (existing_table == NULL || new_table == NULL)
		return AE_BAD_PARAMETER;

	/* We do not override any firmware ACPI tables */
	*new_table = NULL;
	return AE_OK;
}

/*
 * 9.2 Memory Management
 */

#ifndef ACPI_USE_LOCAL_CACHE

/* Fix these if ACPICA is configured to not use its own cache fn's */

acpi_status acpi_os_create_cache(char *cache_name, u16 object_size,
                                 u16 max_depth, acpi_cache_t **return_cache)
{
	return AE_NO_MEMORY;
}

acpi_status acpi_os_delete_cache(acpi_cache_t * cache)
{
	return AE_BAD_PARAMETER;
}

acpi_status acpi_os_purge_cache(acpi_cache_t * cache)
{
	return AE_BAD_PARAMETER;
}

void *acpi_os_acquire_object(acpi_cache_t * cache)
{
	return NULL;
}

acpi_status acpi_os_release_object(acpi_cache_t * cache, void *object)
{
	return AE_BAD_PARAMETER;
}

#endif


void *acpi_os_map_memory(acpi_physical_address where, acpi_size length)
{
	/* AFAICT COM32 doesn't use the MMU and sets up linear 4G segments */
	if (((u64) where + (u64) length) > 0xffffffff)
		return NULL;

	/* Physical to Logical mapping is trivial */
	return (void *) where;
}

void acpi_os_unmap_memory(void *logical_address, acpi_size size)
{
	/* AFAICT COM32 doesn't use the MMU and sets up linear 4G segments */
	/* Unmapping is trivial */
	return;
}

acpi_status acpi_os_get_physical_address(void *logical_address,
                                        acpi_physical_address *physical_address)
{
	if (logical_address == NULL || physical_address == NULL)
		return AE_BAD_PARAMETER;

	/* AFAICT COM32 doesn't use the MMU and sets up linear 4G segments */
	/* Translation is trivial*/
	if (logical_address > (void *) 0xffffffff)
		return AE_ERROR;

	*physical_address = (acpi_physical_address) logical_address;
	return AE_OK;
}

void *acpi_os_allocate(acpi_size size)
{
	return malloc((size_t) size);
}

void acpi_os_free(void *memory)
{
	free(memory);
}

u8 acpi_os_readable(void *pointer, acpi_size length)
{
	/* We could perform a real check, but the ACPICA never calls this */
	return TRUE;
}

u8 acpi_os_writable(void *pointer, acpi_size length)
{
	/* We could perform a real check, but the ACPICA never calls this */
	return TRUE;
}


/*
 * 9.3 Multithreading and Scheduling Services
 */
acpi_thread_id acpi_os_get_thread_id(void)
{
	/* We are single threaded, so return a constant thread id */
	return 0xC0B32AC91051; /* "COM32 ACPI OSL" */
}

acpi_status acpi_os_execute(acpi_execute_type type,
                            acpi_osd_exec_callback function, void *context)
{
	struct acpi_os_deferred_work_e *work;

	if (function == NULL)
		return AE_BAD_PARAMETER;

	switch (type) {
	case OSL_GLOBAL_LOCK_HANDLER:
	case OSL_NOTIFY_HANDLER:
	case OSL_GPE_HANDLER:
	case OSL_DEBUGGER_THREAD:
	case OSL_EC_POLL_HANDLER:
	case OSL_EC_BURST_HANDLER:
		break;
	default:
		return AE_BAD_PARAMETER;
	}

	work = malloc(sizeof(struct acpi_os_deferred_work_e));
	if (work == NULL)
		return AE_NO_MEMORY;

	work->type = type;
	work->fn   = function;
	work->priv = context;

	acpi_os_list_add_tail(&work->list, &acpi_os_deferred_work);
	return AE_OK;
}

void acpi_os_wait_events_complete(void *context)
{
	/* No callers in the ACPICA and no documentation => easy */
	return;
}

void acpi_os_sleep(u64 milliseconds)
{
	/* Wait, suspending the running thread */
	/* The ACPI CA never lets this get called with a value > 2000 msec */
	/* COM32's msleep() calls the idle_hook_func to do periodic work */
	msleep((unsigned int) milliseconds);
}

void acpi_os_stall(u32 microseconds)
{
	clock_t start;
	clock_t msecs;

	/* Busy wait, without suspending the running thread */
	/* COM32 has a granularity of ~54.9 msec, so no usecs are available */
	msecs = (clock_t) (microseconds/1000);
	start = times(NULL);
	while((times(NULL) - start) < msecs)
		;
}


/*
 * 9.4 Mutual Exclusion and Synchronization
 */
#if (ACPI_MUTEX_TYPE != ACPI_BINARY_SEMAPHORE)

acpi_status acpi_os_create_mutex(acpi_mutex *out_handle)
{
	if (out_handle == NULL)
		return AE_BAD_PARAMETER;

	/* We are single threaded, so return a constant handle */
	*out_handle = (acpi_mutex) 0xb007ec5; /* "Mutecs" - sort of */
	return AE_OK;
}

void acpi_os_delete_mutex(acpi_mutex handle)
{
	/* We are single threaded, so no action for the constant handle */
	return;
}

acpi_status acpi_os_acquire_mutex(acpi_mutex handle, u16 timeout)
{
	if (handle != (acpi_mutex) 0xb007ec5)
		return AE_BAD_PARAMETER;

	/* We are single threaded, so no action for the constant handle */
	return AE_OK;
}

void acpi_os_release_mutex(acpi_mutex handle)
{
	/* We are single threaded, so no action for the constant handle */
	return;
}

#endif

acpi_status acpi_os_create_semaphore(u32 max_units, u32 initial_units,
                                     acpi_semaphore *out_handle)
{
	if (out_handle == NULL || (initial_units > max_units))
		return AE_BAD_PARAMETER;

	/* We are single threaded, so return a constant handle */
	*out_handle = (acpi_semaphore) 0x5eba4; /* "Semaphore" - sort of */
	return AE_OK;
}

acpi_status acpi_os_delete_semaphore(acpi_semaphore handle)
{
	if (handle != (acpi_semaphore) 0x5eba4)
		return AE_BAD_PARAMETER;

	/* We are single threaded, so no action for the constant handle */
	return AE_OK;
}

acpi_status acpi_os_wait_semaphore(acpi_semaphore handle, u32 units,
                                   u16 timeout)
{
	if (handle != (acpi_semaphore) 0x5eba4)
		return AE_BAD_PARAMETER;

	/* We are single threaded, so no action for the constant handle */
	return AE_OK;
}

acpi_status acpi_os_signal_semaphore(acpi_semaphore handle, u32 units)
{
	if (handle != (acpi_semaphore) 0x5eba4)
		return AE_BAD_PARAMETER;

	/* We are single threaded, so no action for the constant handle */
	return AE_OK;
}

acpi_status acpi_os_create_lock(acpi_spinlock *out_handle)
{
	if (out_handle == NULL)
		return AE_BAD_PARAMETER;

	/* We are single threaded, so return a constant handle */
	*out_handle = (acpi_spinlock) 0x591d10c; /* "Spinlock" - sort of */
	return AE_OK;
}

void acpi_os_delete_lock(acpi_spinlock handle)
{
	/* We are single threaded, so no action for the constant handle */
	return;
}

acpi_cpu_flags acpi_os_acquire_lock(acpi_spinlock handle)
{
	/* We are single threaded, so return a constant flags value */
	return (acpi_cpu_flags) 0xf1a85; /* "Flags" - sort of */
}

void acpi_os_release_lock(acpi_spinlock handle, acpi_cpu_flags flags)
{
	/* We are single threaded, so no action for the constant handle */
	return;
}



/*
 * 9.5 Interrupt Handling
 */
acpi_status acpi_os_install_interrupt_handler(u32 interrupt_number,
                                              acpi_osd_handler service_routine,
                                              void *context)
{
	if (service_routine == NULL)
		return AE_BAD_PARAMETER;

	/* Ignore ACPI interrupts other than the SCI */
	if (interrupt_number != acpi_gbl_FADT.sci_interrupt)
		return AE_BAD_PARAMETER;

	if (acpi_os_sci_irq.fn != NULL)
		return AE_ALREADY_EXISTS;

	acpi_os_sci_irq.irqn = interrupt_number;
	acpi_os_sci_irq.fn   = service_routine;
	acpi_os_sci_irq.priv = context;

	/*
	 * FIXME: do magic to set up actual interrupt to fire and handler
	 * to be called
	 */
	return AE_OK;
}

acpi_status acpi_os_remove_interrupt_handler(u32 interrupt_number,
                                             acpi_osd_handler service_routine)
{
	/* Ignore ACPI interrupts other than the SCI */
	if (interrupt_number != acpi_gbl_FADT.sci_interrupt)
		return AE_BAD_PARAMETER;

	if (service_routine == NULL || service_routine != acpi_os_sci_irq.fn)
		return AE_BAD_PARAMETER;

	if (acpi_os_sci_irq.fn == NULL)
		return AE_NOT_EXIST;

	/*
	 * FIXME: do magic to halt actual interrupt firing and dereg.
	 */

	acpi_os_sci_irq.irqn = 0xffffffff;
	acpi_os_sci_irq.fn   = NULL;
	acpi_os_sci_irq.priv = NULL;

	return AE_OK;
}



/*
 * 9.6 Memory Access and Memory Mapped I/O
 */
acpi_status acpi_os_read_memory(acpi_physical_address address,
                                u32 *value, u32 width)
{
	if (value == NULL || address == (acpi_physical_address) NULL)
		return AE_BAD_PARAMETER;

	/* AFAICT COM32 doesn't use the MMU and sets up linear 4G segments */
	/* Logical <-> Physcial mapping is trivial */

	switch (width) {
	case 8:
		*value = (u32) *(( u8 *) address);
		break;
	case 16:
		*value = (u32) *((u16 *) address);
		break;
	case 32:
		*value = (u32) *((u32 *) address);
		break;
	default:
		return AE_BAD_PARAMETER;
	}
	return AE_OK;
}

acpi_status acpi_os_write_memory(acpi_physical_address address,
                                 u32 value, u32 width)
{
	if (address == (acpi_physical_address) NULL)
		return AE_BAD_PARAMETER;

	/* AFAICT COM32 doesn't use the MMU and sets up linear 4G segments */
	/* Logical <-> Physcial mapping is trivial */
	switch (width) {
	case 8:
		*(( u8 *) address) = ( u8) (value & 0x000000ff);
		break;
	case 16:
		*((u16 *) address) = (u16) (value & 0x0000ffff);
		break;
	case 32:
		*((u32 *) address) = value;
		break;
	default:
		return AE_BAD_PARAMETER;
	}
	return AE_OK;
}



/*
 * 9.7 Port Input/Output
 */
acpi_status acpi_os_read_port(acpi_io_address address, u32 *value, u32 width)
{
	if (value == NULL)
		return AE_BAD_PARAMETER;

	switch (width) {
	case 8:
		*value = (u32) inb((uint16_t) address);
		break;
	case 16:
		*value = (u32) inw((uint16_t) address);
		break;
	case 32:
		*value = (u32) inl((uint16_t) address);
		break;
	default:
		return AE_BAD_PARAMETER;
	}
	return AE_OK;
}

acpi_status acpi_os_write_port(acpi_io_address address, u32 value, u32 width)
{
	switch (width) {
	case 8:
		outb(( uint8_t) (value & 0x000000ff), (uint16_t) address);
		break;
	case 16:
		outw((uint16_t) (value & 0x0000ffff), (uint16_t) address);
		break;
	case 32:
		outl((uint32_t) value, (uint16_t) address);
		break;
	default:
		return AE_BAD_PARAMETER;
	}
	return AE_OK;
}



/*
 * 9.8 PCI Configuration Space Access
 */
acpi_status acpi_os_read_pci_configuration(struct acpi_pci_id *pci_id,
                                           u32 reg, u64 *value, u32 width)
{
	pciaddr_t addr;

        if (acpi_os_pci_config_type == PCI_CFG_NONE)
		return AE_NOT_EXIST;

	if (value == NULL || pci_id == NULL)
		return AE_BAD_PARAMETER;

	/* Due to some library limits in pci_mkaddr() */
	if (pci_id->segment != 0 || pci_id->bus > 0xff ||
	    pci_id->device > 0x1f || pci_id->function > 0x07)
		return AE_LIMIT;

	addr = pci_mkaddr((uint32_t) pci_id->bus,
	                  (uint32_t) pci_id->device,
                          (uint32_t) pci_id->function,
                          (uint32_t) reg);

	switch(width) {
	case 8:
		*value = (u64) pci_readb(addr);
		break;
	case 16:
		*value = (u64) pci_readw(addr);
		break;
	case 32:
		*value = (u64) pci_readl(addr);
		break;
	case 64:
		return AE_LIMIT;
	default:
		return AE_BAD_PARAMETER;
	}
	return AE_OK;
}

acpi_status acpi_os_write_pci_configuration(struct acpi_pci_id *pci_id,
                                            u32 reg, u64 value, u32 width)
{
	pciaddr_t addr;

        if (acpi_os_pci_config_type == PCI_CFG_NONE)
		return AE_NOT_EXIST;

	if (pci_id == NULL)
		return AE_BAD_PARAMETER;

	/* Due to some library limits in pci_mkaddr() */
	if (pci_id->segment != 0 || pci_id->bus > 0xff ||
	    pci_id->device > 0x1f || pci_id->function > 0x07)
		return AE_LIMIT;

	addr = pci_mkaddr((uint32_t) pci_id->bus,
	                  (uint32_t) pci_id->device,
                          (uint32_t) pci_id->function,
                          (uint32_t) reg);

	switch(width) {
	case 8:
		pci_writeb(( uint8_t) (value & 0x000000ff), addr);
		break;
	case 16:
		pci_writew((uint16_t) (value & 0x0000ffff), addr);
		break;
	case 32:
		pci_writel((uint32_t) (value & 0xffffffff), addr);
		break;
	case 64:
		return AE_LIMIT;
	default:
		return AE_BAD_PARAMETER;
	}
	return AE_OK;
}



/*
 * 9.9 Fomatted Output
 */
void ACPI_INTERNAL_VAR_XFACE acpi_os_printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	acpi_os_vprintf(format, args);
	va_end(args);
}

void acpi_os_vprintf(const char *format, va_list args)
{
	if (acpi_osl_printf_dest == NULL)
		acpi_osl_printf_dest = stdout;
	vfprintf(acpi_osl_printf_dest, format, args);
}

void acpi_os_redirect_output(void *destination)
{
	if (destination != NULL)
		acpi_osl_printf_dest = destination;
}



/*
 * 9.10 Miscellaneous
 */
u64 acpi_os_get_timer(void)
{
	/* times(NULL) returns milliseconds, we return 100's of nanoseconds */
	return ((u64) times(NULL)) * 10000;
}

acpi_status acpi_os_signal(u32 function, void *info)
{
	struct acpi_signal_fatal_info *finfo;

	switch (function) {
	case ACPI_SIGNAL_FATAL:
		ACPI_ERROR((AE_INFO, "Received AML FATAL opcode signal,"));
		if (info != NULL) {
			finfo = info;
			ACPI_ERROR((AE_INFO, "with type 0x%8.8X, code 0x%8.8X, "
				   "arg 0x%8.8X,", finfo->type, finfo->code,
			           finfo->argument));
		}
		ACPI_ERROR((AE_INFO, "trying to proceed anyway."));
		break;
	case ACPI_SIGNAL_BREAKPOINT:
		ACPI_WARNING((AE_INFO, "No ACPI debugger support; ignoring "
		             "AML BREAKPOINT opcode signal with msg: %s",
		             (char *) info));
		break;
	default:
		return AE_BAD_PARAMETER;
	}
	return AE_OK;
}

acpi_status acpi_os_get_line(char *buffer, u32 buffer_length, u32 *bytes_read)
{
	if (bytes_read == NULL || buffer == NULL || buffer_length == 0)
		return AE_BAD_PARAMETER;

	/* The ACPICA doesn't call this, when there is no debugger thread */
	buffer[0] = '\n';
	*bytes_read = 1;
	return AE_OK;
}



/*
 * Undocumented and not called by the ACPICA
 */
void *acpi_os_open_directory(char *pathname,
                             char *wildcard_spec, char requested_file_type)
{
	return NULL;
}

char *acpi_os_get_next_filename(void *dir_handle)
{
	return NULL;
}

void acpi_os_close_directory(void *dir_handle)
{
	return;
}
