## -----------------------------------------------------------------------
##
##   Copyright 2001-2008 H. Peter Anvin - All Rights Reserved
##   Copyright 2012 Andy Walls <awalls@md.metrocast.net>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as published by
##   the Free Software Foundation, Inc., 53 Temple Place Ste 330,
##   Boston MA 02111-1307, USA; either version 2 of the License, or
##   (at your option) any later version; incorporated herein by reference.
##
## -----------------------------------------------------------------------

# Avoid <acpi/acpi.h> header file conflicts with syslinux/com32/gplinclude
NOGPL = 1

LIBS += $(CURDIR)/libacpica.a

topdir = ../..
MAKEDIR = $(topdir)/mk
include $(MAKEDIR)/com32.mk

LIBACPICA_SRC_DIRS = acpica_linuxized/drivers/acpi/acpica \
	acpica_linuxized/drivers/acpi

CFLAGS += -I$(CURDIR)/acpica_linuxized/include \
	-I$(CURDIR)/acpica_linuxized/include/acpi \
	-I$(CURDIR)/acpica_linuxized/drivers/acpi/acpica

# The ACPICA can have a number of unused parameters due to macros and
# configuration options
GCCWARN += -Wno-unused-parameter

# Uncomment to have the ACPICA spew debugging output
#CFLAGS += -DACPI_DEBUG_OUTPUT

# Uncomment when ACPI_DEBUG_OUTPUT is defined for debug output, as the ACPICA
# debug output code generates many warnings
#GCCWARN += -Wno-error

LIBACPICA_OBJS := $(foreach dir,$(LIBACPICA_SRC_DIRS),$(patsubst %.c,%.o,$(wildcard $(dir)/*.c)))

all: acpioff.c32

acpioff.elf: acpioff.o $(LIBS) $(C_LIBS) $(COM32LD)
	$(LD) $(LDFLAGS) -o $@ $(filter-out $(COM32LD),$^)

$(CURDIR)/libacpica.a: $(LIBACPICA_OBJS)
	rm -f $@
	$(AR) cq $@ $^
	$(RANLIB) $@

tidy dist:
	find . \( -name \*.o   -o \
	          -name \*.lo  -o \
	          -name \*.a   -o \
	          -name \*.lst -o \
	          -name \*.elf -o \
	          -name .\*.d  -o \
	          -name \*.tmp \) -print0 | xargs -0r rm -f

clean: tidy
	find . -name \*.lnx -print0 | xargs -0r rm -f

spotless: clean
	find . \( -name \*.lss -o \
	          -name \*.c32 -o \
	          -name \*.com -o \
	          -name "*~"   -o \
	          -name "#*" \) -print0 | xargs -0r rm -f

install:	

-include .*.d */.*.d */*/.*.d */*/*/.*.d
