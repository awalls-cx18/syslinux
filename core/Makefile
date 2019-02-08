## -----------------------------------------------------------------------
##
##   Copyright 1998-2009 H. Peter Anvin - All Rights Reserved
##   Copyright 2009-2016 Intel Corporation; author: H. Peter Anvin
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as published by
##   the Free Software Foundation, Inc., 53 Temple Place Ste 330,
##   Boston MA 02111-1307, USA; either version 2 of the License, or
##   (at your option) any later version; incorporated herein by reference.
##
## -----------------------------------------------------------------------

#
# Makefile for the SYSLINUX core
#

VPATH = $(SRC)

# No builtin rules
MAKEFLAGS += -r
MAKE      += -r

include $(MAKEDIR)/embedded.mk
-include $(objdir)/version.mk

OPTFLAGS =
INCLUDES = -I$(SRC)/include -I$(com32)/include -I$(com32)/include/sys \
	   -I$(com32)/lib \
	   -I$(SRC)/bios/lwip/src/include \
	   -I$(SRC)/bios/lwip/src/include/ipv4 -I$(SRC)/fs/pxe

# This is very similar to cp437; technically it's for Norway and Denmark,
# but it's unlikely the characters that are different will be used in
# filenames by other users.
CODEPAGE = cp865

# The targets to build in this directory...
BTARGET  = kwdhash.gen ldlinux.bss ldlinux.sys \
	   isolinux.bin isolinux-debug.bin pxelinux.0 lpxelinux.0

# All primary source files for the main syslinux files
NASMSRC	 := $(sort $(shell find $(SRC) -name '*.asm' -print))
NASMHDR	 := $(sort $(shell find $(SRC) -name '*.inc' -print))
CSRC	 := $(sort $(shell find $(SRC) -name '*.c' -print))
SSRC	 := $(sort $(shell find $(SRC) -name '*.S' -print))
CHDR	 := $(sort $(shell find $(SRC) -name '*.h' -print))
OTHERSRC := keywords
ALLSRC    = $(NASMSRC) $(NASMHDR) $(CSRC) $(SSRC) $(CHDR) $(OTHERSRC)

COBJ	 := $(subst $(SRC)/,,$(patsubst %.c,%.o,$(CSRC)))
SOBJ	 := $(subst $(SRC)/,,$(patsubst %.S,%.o,$(SSRC)))

# To make this compatible with the following $(filter-out), make sure
# we prefix everything with $(SRC)
CORE_PXE_DIRS  = $(SRC)/bios/pxe
LPXELINUX_DIRS = $(SRC)/bios/lwip
PXELINUX_DIRS  = $(SRC)/bios/legacynet

CORE_PXE_CSRC  = $(sort $(shell find $(CORE_PXE_DIRS)  -name '*.c' -print))
CORE_PXE_SSRC  = $(sort $(shell find $(CORE_PXE_DIRS)  -name '*.S' -print))
LPXELINUX_CSRC = $(sort $(shell find $(LPXELINUX_DIRS) -name '*.c' -print))
LPXELINUX_SSRC = $(sort $(shell find $(LPXELINUX_DIRS) -name '*.S' -print))
PXELINUX_CSRC  = $(sort $(shell find $(PXELINUX_DIRS)  -name '*.c' -print))
PXELINUX_SSRC  = $(sort $(shell find $(PXELINUX_DIRS)  -name '*.S' -print))

CORE_PXE_OBJS  = $(subst $(SRC)/,,$(CORE_PXE_CSRC:%.c=%.o)  $(CORE_PXE_SSRC:%.S=%.o))
LPXELINUX_OBJS = $(subst $(SRC)/,,$(LPXELINUX_CSRC:%.c=%.o) $(LPXELINUX_SSRC:%.S=%.o))
PXELINUX_OBJS  = $(subst $(SRC)/,,$(PXELINUX_CSRC:%.c=%.o)  $(PXELINUX_SSRC:%.S=%.o))

UNITTEST_DIRS = $(sort $(shell find $(SRC) -type d -name tests))

# Don't include network stack specific objects or unit tests
FILTER_DIRS = $(UNITTEST_DIRS) $(CORE_PXE_DIRS) \
	      $(PXELINUX_DIRS) $(LPXELINUX_DIRS)
ifeq ($(FWCLASS),EFI)
FILTER_DIRS += $(SRC)/bios
endif
FILTER_OBJS = $(patsubst $(SRC)/%,%/%,$(FILTER_DIRS))

COBJS	 = $(filter-out $(FILTER_OBJS),$(COBJ))
SOBJS	 = $(filter-out $(FILTER_OBJS),$(SOBJ))

LIB	 = bios/libsyslinux.a
LIBS	 = $(LIB) --whole-archive $(objdir)/com32/lib/libcom32core.a
LIBDEP   = $(filter-out -% %start%,$(LIBS))
LIBOBJS	 = $(COBJS) $(SOBJS)

NASMDEBUG = -g -F dwarf
NASMOPT  += $(NASMDEBUG)

PREPCORE = $(OBJ)/../lzo/prepcore

CFLAGS += -D__SYSLINUX_CORE__ -D__FIRMWARE_$(FIRMWARE)__ \
	  -I$(objdir) -DLDLINUX=\"$(LDLINUX)\"

# The DATE is set on the make command line when building binaries for
# official release.  Otherwise, substitute a hex string that is pretty much
# guaranteed to be unique to be unique from build to build.
ifndef HEXDATE
HEXDATE := $(shell $(PERL) $(SRC)/../now.pl $(SRCS))
endif
ifndef DATE
DATE    := $(shell sh $(SRC)/../gen-id.sh $(VERSION) $(HEXDATE))
endif

ifeq ($(FWCLASS),EFI)
all: makeoutputdirs $(filter-out %bios.o,$(COBJS) $(SOBJS)) codepage.o
else
all: makeoutputdirs $(BTARGET)
endif

makeoutputdirs:
	@mkdir -p $(sort $(dir $(COBJ) $(SOBJ)))

kwdhash.gen: keywords genhash.pl
	$(PERL) $(SRC)/genhash.pl < $(SRC)/keywords > $(OBJ)/kwdhash.gen

ifeq ($(FWCLASS),BIOS)

.PRECIOUS: %.elf
%.raw: %.elf
	$(OBJCOPY) -O binary -S $< $(@:.bin=.raw)

# GNU make 3.82 gets confused by plain %.raw; make 4.0 requires it
.PRECIOUS: $(OBJ)/%.raw %.raw
%.bin: %.raw $(PREPCORE)
	$(PREPCORE) $< $@

%.o: %.asm kwdhash.gen $(OBJ)/../version.gen
	$(NASM) -f $(NASM_ELF) $(NASMOPT) -DDATE_STR="'$(DATE)'" \
		-DHEXDATE="$(HEXDATE)" \
		-D$(ARCH) \
		-I$(SRC)/ -I$(SRC)/bios/ \
		-l $(@:.o=.lsr) -o $@ -MP -MD $(@D)/.$(@F).d $<

AUXLIBS = bios/libpxelinux.a bios/liblpxelinux.a
LDSCRIPT = $(SRC)/bios/syslinux.ld

NASM_ELF = elf

%.elf: %.o %-c.o lib%.a $(LIBDEP) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -pie -Bsymbolic \
		-T $(LDSCRIPT) \
		--unresolved-symbols=report-all \
		-E --hash-style=gnu -M -o $@ $< $*-c.o \
		--start-group $(*D)/lib$(*F).a $(LIBS) --end-group \
		> $*.map
	if [ `$(NM) -D -u $@ | wc -l` -ne 0 ]; then \
		$(NM) -D -u $@ 1>&2; rm -f $@; false; fi
	$(OBJDUMP) -h $@ > $*.sec
	$(PERL) $(SRC)/lstadjust.pl $*.lsr $*.sec $*.lst

# Legacy network stack
bios/libpxelinux.a: $(CORE_PXE_OBJS) $(PXELINUX_OBJS)

# LwIP network stack
bios/liblpxelinux.a: $(CORE_PXE_OBJS) $(LPXELINUX_OBJS)

# Otherwise, no special library needed, a null library will be created

bios/lib%.a:
	rm -f $@
	$(AR) cq $@ $^
	$(RANLIB) $@

%.0: bios/%.bin
	cp -f $< $@

%.bin: bios/%.bin
	cp -f $< $@

ldlinux.bss: bios/ldlinux.bin
	dd if=$< of=$@ bs=512 count=1

ldlinux.sys: bios/ldlinux.bin
	dd if=$< of=$@ bs=512 skip=2

endif # BIOS

$(LIB): $(LIBOBJS)
	rm -f $@
	$(AR) cq $@ $^
	$(RANLIB) $@

codepage.cp: $(OBJ)/../codepage/$(CODEPAGE).cp
	cp -f $< $@

codepage.o: codepage.S codepage.cp

install: installer

install-lib: installer

install-all: install install-lib

netinstall: installer

tidy dist:
	for d in $(find . -type d -print) ; do \
	( cd "$$d" ; \
	  rm -f *.o *.a .*.d *.lst \
	        codepage.cp *.elf stupid.* patch.offset .depend \
	        *.elf.tmp *.sym \
	        *.lsr *.map *.sec *.raw \
	); done
	rm -f $(OBSOLETE) $(LIB)

clean: tidy

spotless: clean
	rm -f $(BTARGET) *.bin *_bin.c

# Include dependencies file
-include $(shell find $(OBJ) -name '.*.d' -print)
