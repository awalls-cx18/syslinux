SUBDIRS = tools lib libutil gpllib libupload elflink/ldlinux modules mboot \
	  menu samples elflink rosh cmenu lua/src

ifneq ($(FWCLASS),EFI)
# These tools are no applicable to EFI, or need serious porting
SUBDIRS += hdt gfxboot sysdump chain
endif

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	@mkdir -p $(OBJ)/$@
	$(MAKE) -C $(OBJ)/$@ SRC="$(SRC)"/$@ OBJ="$(OBJ)"/$@/ \
		-f $(SRC)/$@/Makefile $(MAKECMDGOALS)

all tidy dist clean spotless install: subdirs

# Parallel dependencies
elflink/ldlinux gpllib libupload libutil: lib
cmenu: lib libutil

chain gfxboot mboot menu modules rosh: lib libutil gpllib
hdt: lib libutil gpllib libupload cmenu
lua/src: lib libutil gpllib cmenu
samples: lib libutil gpllib elflink/ldlinux
sysdump: lib libutil gpllib libupload
