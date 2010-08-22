#include <com32.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <syslinux/config.h>
#include "common.h"
#include "chain.h"
#include "utility.h"
#include "partiter.h"
#include "mangle.h"

static const char cmldr_signature[8] = "cmdcons";

/* Create boot info table: needed when you want to chainload
 * another version of ISOLINUX (or another bootlaoder that needs
 * the -boot-info-table switch of mkisofs)
 * (will only work when run from ISOLINUX)
 */
int manglef_isolinux(struct data_area *data)
{
    const union syslinux_derivative_info *sdi;
    unsigned char *isolinux_bin;
    uint32_t *checksum, *chkhead, *chktail;
    uint32_t file_lba = 0;

    sdi = syslinux_derivative_info();

    if (sdi->c.filesystem != SYSLINUX_FS_ISOLINUX) {
	error ("The isolinux= option is only valid when run from ISOLINUX.\n");
	goto bail;
    }

    /* Boot info table info (integers in little endian format)

       Offset Name         Size      Meaning
       8      bi_pvd       4 bytes   LBA of primary volume descriptor
       12     bi_file      4 bytes   LBA of boot file
       16     bi_length    4 bytes   Boot file length in bytes
       20     bi_csum      4 bytes   32-bit checksum
       24     bi_reserved  40 bytes  Reserved

       The 32-bit checksum is the sum of all the 32-bit words in the
       boot file starting at byte offset 64. All linear block
       addresses (LBAs) are given in CD sectors (normally 2048 bytes).

       LBA of primary volume descriptor should already be set to 16.
       */

    isolinux_bin = (unsigned char *)data->data;

    /* Get LBA address of bootfile */
    file_lba = get_file_lba(opt.file);

    if (file_lba == 0) {
	error("Failed to find LBA offset of the boot file\n");
	goto bail;
    }
    /* Set it */
    *((uint32_t *) & isolinux_bin[12]) = file_lba;

    /* Set boot file length */
    *((uint32_t *) & isolinux_bin[16]) = data->size;

    /* Calculate checksum */
    checksum = (uint32_t *) & isolinux_bin[20];
    chkhead = (uint32_t *) & isolinux_bin[64];
    chktail = (uint32_t *) & isolinux_bin[data->size & ~3u];
    *checksum = 0;
    while (chkhead < chktail)
	*checksum += *chkhead++;

    /*
     * Deal with possible fractional dword at the end;
     * this *should* never happen...
     */
    if (data->size & 3) {
	uint32_t xword = 0;
	memcpy(&xword, chkhead, data->size & 3);
	*checksum += xword;
    }
    return 0;
bail:
    return -1;
}

/*
 * GRLDR of GRUB4DOS wants the partition number in DH:
 * -1:   whole drive (default)
 * 0-3:  primary partitions
 * 4-*:  logical partitions
 */
int manglef_grldr(const struct part_iter *iter)
{
    opt.regs.edx.b[1] = (uint8_t)(iter->index - 1);
    return 0;
}

/*
 * Legacy grub's stage2 chainloading
 */
int manglef_grub(const struct part_iter *iter, struct data_area *data)
{
    /* Layout of stage2 file (from byte 0x0 to 0x270) */
    struct grub_stage2_patch_area {
	/* 0x0 to 0x205 */
	char unknown[0x206];
	/* 0x206: compatibility version number major */
	uint8_t compat_version_major;
	/* 0x207: compatibility version number minor */
	uint8_t compat_version_minor;

	/* 0x208: install_partition variable */
	struct {
	    /* 0x208: sub-partition in sub-partition part2 */
	    uint8_t part3;
	    /* 0x209: sub-partition in top-level partition */
	    uint8_t part2;
	    /* 0x20a: top-level partiton number */
	    uint8_t part1;
	    /* 0x20b: BIOS drive number (must be 0) */
	    uint8_t drive;
	} __attribute__ ((packed)) install_partition;

	/* 0x20c: deprecated (historical reason only) */
	uint32_t saved_entryno;
	/* 0x210: stage2_ID: will always be STAGE2_ID_STAGE2 = 0 in stage2 */
	uint8_t stage2_id;
	/* 0x211: force LBA */
	uint8_t force_lba;
	/* 0x212: version string (will probably be 0.97) */
	char version_string[5];
	/* 0x217: config filename */
	char config_file[89];
	/* 0x270: start of code (after jump from 0x200) */
	char codestart[1];
    } __attribute__ ((packed)) *stage2;

    if (data->size < sizeof(struct grub_stage2_patch_area)) {
	error("The file specified by grub=<loader> is too small to be stage2 of GRUB Legacy.\n");
	goto bail;
    }
    stage2 = data->data;

    /*
     * Check the compatibility version number to see if we loaded a real
     * stage2 file or a stage2 file that we support.
     */
    if (stage2->compat_version_major != 3
	    || stage2->compat_version_minor != 2) {
	error("The file specified by grub=<loader> is not a supported stage2 GRUB Legacy binary.\n");
	goto bail;
    }

    /*
     * GRUB Legacy wants the partition number in the install_partition
     * variable, located at offset 0x208 of stage2.
     * When GRUB Legacy is loaded, it is located at memory address 0x8208.
     *
     * It looks very similar to the "boot information format" of the
     * Multiboot specification:
     *   http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
     *
     *   0x208 = part3: sub-partition in sub-partition part2
     *   0x209 = part2: sub-partition in top-level partition
     *   0x20a = part1: top-level partition number
     *   0x20b = drive: BIOS drive number (must be 0)
     *
     * GRUB Legacy doesn't store the BIOS drive number at 0x20b, but at
     * another location.
     *
     * Partition numbers always start from zero.
     * Unused partition bytes must be set to 0xFF.
     *
     * We only care about top-level partition, so we only need to change
     * "part1" to the appropriate value:
     *   -1:   whole drive (default) (-1 = 0xFF)
     *   0-3:  primary partitions
     *   4-*:  logical partitions
     */
    stage2->install_partition.part1 = (uint8_t)(iter->index - 1);

    /*
     * Grub Legacy reserves 89 bytes (from 0x8217 to 0x826f) for the
     * config filename. The filename passed via grubcfg= will overwrite
     * the default config filename "/boot/grub/menu.lst".
     */
    if (opt.grubcfg) {
	if (strlen(opt.grubcfg) > sizeof(stage2->config_file) - 1) {
	    error ("The config filename length can't exceed 88 characters.\n");
	    goto bail;
	}

	strcpy((char *)stage2->config_file, opt.grubcfg);
    }

    return 0;
bail:
    return -1;
}

/*
 * Adjust BPB of a BPB-compatible file
 */
int manglef_bpb(const struct part_iter *iter, struct data_area *data)
{
    /* BPB: hidden sectors */
    if (opt.sethid) {
	if (iter->start_lba < ~0u)
	    *(uint32_t *) ((char *)data->data + 0x1c) = (uint32_t)iter->start_lba;
	else
	    /* won't really help much, but ... */
	    *(uint32_t *) ((char *)data->data + 0x1c) = ~0u;
    }
    /* BPB: legacy geometry */
    if (opt.setgeo) {
	if (iter->di.cbios)
	    *(uint32_t *)((char *)data->data + 0x18) = (uint32_t)((iter->di.head << 16) | iter->di.sect);
	else {
	    if (iter->di.disk & 0x80)
		*(uint32_t *)((char *)data->data + 0x18) = 0x00FF003F;
	    else
		*(uint32_t *)((char *)data->data + 0x18) = 0x00020012;
	}
    }

    /* BPB: drive */
    if (opt.setdrv)
	*(uint8_t *)((char *)data->data + opt.drvoff) = (uint8_t)
	    (opt.swap ? iter->di.disk & 0x80 : iter->di.disk);

    return 0;
}

/*
 * Adjust BPB of a sector
 */
int try_mangles_bpb(const struct part_iter *iter, struct data_area *data)
{
    void *cmp_buf = NULL;

    if (!(opt.setdrv || opt.setgeo || opt.sethid))
	return 0;

#if 0
    /* Turn this off for now. It's hard to find a reason to
     * BPB-mangle sector 0 of whatever there is, but it's
     * "potentially" useful (fixing fdd's sector ?).
     */
    if (!iter->index)
	return 0;
#endif

    if (!(cmp_buf = malloc(data->size))) {
	error("Could not allocate sector-compare buffer.\n");
	goto bail;
    }

    memcpy(cmp_buf, data->data, data->size);

    manglef_bpb(iter, data);

    if (opt.save && memcmp(cmp_buf, data->data, data->size)) {
	if (disk_write_verify_sector(&iter->di, iter->start_lba, data->data)) {
	    error("Cannot write updated boot sector.\n");
	    goto bail;
	}
    }

    free(cmp_buf);
    return 0;

bail:
    return -1;
}

/*
 * To boot the Recovery Console of Windows NT/2K/XP we need to write
 * the string "cmdcons\0" to memory location 0000:7C03.
 * Memory location 0000:7C00 contains the bootsector of the partition.
 */
int mangles_cmldr(struct data_area *data)
{
    memcpy((char *)data->data + 3, cmldr_signature, sizeof(cmldr_signature));
    return 0;
}

#if 0
/*
 * Dell's DRMK chainloading.
 */
int manglef_drmk(struct data_area *data)
{
    /*
     * DRMK entry is different than MS-DOS/PC-DOS
     * A new size, aligned to 16 bytes to ease use of ds:[bp+28].
     * We only really need 4 new, usable bytes at the end.
     */

    uint32_t tsize = (data->size + 19) & 0xfffffff0;
    opt.regs.ss = opt.regs.fs = opt.regs.gs = 0;	/* Used before initialized */
    if (!realloc(data->data, tsize)) {
	error("Failed to realloc for DRMK.\n");
	goto bail;
    }
    data->size = tsize;
    /* ds:[bp+28] must be 0x0000003f */
    opt.regs.ds = (uint16_t)((tsize >> 4) + (opt.fseg - 2));
    /* "Patch" into tail of the new space */
    *(uint32_t *)((char*)data->data + tsize - 4) = 0x0000003f;

    return 0;
bail:
    return -1;
}
#endif

/* vim: set ts=8 sts=4 sw=4 noet: */
