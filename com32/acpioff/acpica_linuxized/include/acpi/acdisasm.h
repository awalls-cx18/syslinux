/******************************************************************************
 *
 * Name: acdisasm.h - AML disassembler
 *
 *****************************************************************************/

/*
 * Copyright (C) 2000 - 2012, Intel Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    substantially similar to the "NO WARRANTY" disclaimer below
 *    ("Disclaimer") and any redistribution must be conditioned upon
 *    including a substantially similar Disclaimer requirement for further
 *    binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef __ACDISASM_H__
#define __ACDISASM_H__

#include "amlresrc.h"

#define BLOCK_NONE              0
#define BLOCK_PAREN             1
#define BLOCK_BRACE             2
#define BLOCK_COMMA_LIST        4
#define ACPI_DEFAULT_RESNAME    *(u32 *) "__RD"

/*
 * Raw table data header. Used by disassembler and data table compiler.
 * Do not change.
 */
#define ACPI_RAW_TABLE_DATA_HEADER      "Raw Table Data"

typedef const struct acpi_dmtable_info {
	u8 opcode;
	u16 offset;
	char *name;
	u8 flags;

} acpi_dmtable_info;

#define DT_LENGTH                       0x01	/* Field is a subtable length */
#define DT_FLAG                         0x02	/* Field is a flag value */
#define DT_NON_ZERO                     0x04	/* Field must be non-zero */

/* TBD: Not used at this time */

#define DT_OPTIONAL                     0x08
#define DT_COUNT                        0x10

/*
 * Values for Opcode above.
 * Note: 0-7 must not change, they are used as a flag shift value. Other
 * than those, new values can be added wherever appropriate.
 */
typedef enum {
	/* Simple Data Types */

	ACPI_DMT_FLAG0 = 0,
	ACPI_DMT_FLAG1 = 1,
	ACPI_DMT_FLAG2 = 2,
	ACPI_DMT_FLAG3 = 3,
	ACPI_DMT_FLAG4 = 4,
	ACPI_DMT_FLAG5 = 5,
	ACPI_DMT_FLAG6 = 6,
	ACPI_DMT_FLAG7 = 7,
	ACPI_DMT_FLAGS0,
	ACPI_DMT_FLAGS1,
	ACPI_DMT_FLAGS2,
	ACPI_DMT_FLAGS4,
	ACPI_DMT_UINT8,
	ACPI_DMT_UINT16,
	ACPI_DMT_UINT24,
	ACPI_DMT_UINT32,
	ACPI_DMT_UINT40,
	ACPI_DMT_UINT48,
	ACPI_DMT_UINT56,
	ACPI_DMT_UINT64,
	ACPI_DMT_BUF7,
	ACPI_DMT_BUF16,
	ACPI_DMT_BUF128,
	ACPI_DMT_SIG,
	ACPI_DMT_STRING,
	ACPI_DMT_NAME4,
	ACPI_DMT_NAME6,
	ACPI_DMT_NAME8,

	/* Types that are decoded to strings and miscellaneous */

	ACPI_DMT_ACCWIDTH,
	ACPI_DMT_CHKSUM,
	ACPI_DMT_GAS,
	ACPI_DMT_SPACEID,
	ACPI_DMT_UNICODE,
	ACPI_DMT_UUID,

	/* Types used only for the Data Table Compiler */

	ACPI_DMT_BUFFER,
	ACPI_DMT_DEVICE_PATH,
	ACPI_DMT_LABEL,
	ACPI_DMT_PCI_PATH,

	/* Types that are specific to particular ACPI tables */

	ACPI_DMT_ASF,
	ACPI_DMT_DMAR,
	ACPI_DMT_EINJACT,
	ACPI_DMT_EINJINST,
	ACPI_DMT_ERSTACT,
	ACPI_DMT_ERSTINST,
	ACPI_DMT_FADTPM,
	ACPI_DMT_HEST,
	ACPI_DMT_HESTNTFY,
	ACPI_DMT_HESTNTYP,
	ACPI_DMT_IVRS,
	ACPI_DMT_MADT,
	ACPI_DMT_PMTT,
	ACPI_DMT_SLIC,
	ACPI_DMT_SRAT,

	/* Special opcodes */

	ACPI_DMT_EXTRA_TEXT,
	ACPI_DMT_EXIT
} ACPI_ENTRY_TYPES;

typedef
void (*acpi_dmtable_handler) (struct acpi_table_header * table);

typedef acpi_status(*acpi_cmtable_handler) (void **pfield_list);

struct acpi_dmtable_data {
	char *signature;
	struct acpi_dmtable_info *table_info;
	acpi_dmtable_handler table_handler;
	acpi_cmtable_handler cm_table_handler;
	const unsigned char *template;
	char *name;
};

struct acpi_op_walk_info {
	u32 level;
	u32 last_level;
	u32 count;
	u32 bit_offset;
	u32 flags;
	struct acpi_walk_state *walk_state;
};

/*
 * TBD - another copy of this is in asltypes.h, fix
 */
#ifndef ASL_WALK_CALLBACK_DEFINED
typedef
acpi_status(*asl_walk_callback) (union acpi_parse_object * op,
				 u32 level, void *context);
#define ASL_WALK_CALLBACK_DEFINED
#endif

typedef
void (*acpi_resource_handler) (union aml_resource * resource,
			       u32 length, u32 level);

struct acpi_resource_tag {
	u32 bit_index;
	char *tag;
};

/* Strings used for decoding flags to ASL keywords */

extern const char *acpi_gbl_word_decode[];
extern const char *acpi_gbl_irq_decode[];
extern const char *acpi_gbl_lock_rule[];
extern const char *acpi_gbl_access_types[];
extern const char *acpi_gbl_update_rules[];
extern const char *acpi_gbl_match_ops[];

extern struct acpi_dmtable_info acpi_dm_table_info_asf0[];
extern struct acpi_dmtable_info acpi_dm_table_info_asf1[];
extern struct acpi_dmtable_info acpi_dm_table_info_asf1a[];
extern struct acpi_dmtable_info acpi_dm_table_info_asf2[];
extern struct acpi_dmtable_info acpi_dm_table_info_asf2a[];
extern struct acpi_dmtable_info acpi_dm_table_info_asf3[];
extern struct acpi_dmtable_info acpi_dm_table_info_asf4[];
extern struct acpi_dmtable_info acpi_dm_table_info_asf_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_boot[];
extern struct acpi_dmtable_info acpi_dm_table_info_bert[];
extern struct acpi_dmtable_info acpi_dm_table_info_bgrt[];
extern struct acpi_dmtable_info acpi_dm_table_info_cpep[];
extern struct acpi_dmtable_info acpi_dm_table_info_cpep0[];
extern struct acpi_dmtable_info acpi_dm_table_info_dbgp[];
extern struct acpi_dmtable_info acpi_dm_table_info_dmar[];
extern struct acpi_dmtable_info acpi_dm_table_info_dmar_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_dmar_scope[];
extern struct acpi_dmtable_info acpi_dm_table_info_dmar0[];
extern struct acpi_dmtable_info acpi_dm_table_info_dmar1[];
extern struct acpi_dmtable_info acpi_dm_table_info_dmar2[];
extern struct acpi_dmtable_info acpi_dm_table_info_dmar3[];
extern struct acpi_dmtable_info acpi_dm_table_info_drtm[];
extern struct acpi_dmtable_info acpi_dm_table_info_ecdt[];
extern struct acpi_dmtable_info acpi_dm_table_info_einj[];
extern struct acpi_dmtable_info acpi_dm_table_info_einj0[];
extern struct acpi_dmtable_info acpi_dm_table_info_erst[];
extern struct acpi_dmtable_info acpi_dm_table_info_erst0[];
extern struct acpi_dmtable_info acpi_dm_table_info_facs[];
extern struct acpi_dmtable_info acpi_dm_table_info_fadt1[];
extern struct acpi_dmtable_info acpi_dm_table_info_fadt2[];
extern struct acpi_dmtable_info acpi_dm_table_info_fadt3[];
extern struct acpi_dmtable_info acpi_dm_table_info_fadt5[];
extern struct acpi_dmtable_info acpi_dm_table_info_fpdt[];
extern struct acpi_dmtable_info acpi_dm_table_info_fpdt_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_fpdt0[];
extern struct acpi_dmtable_info acpi_dm_table_info_fpdt1[];
extern struct acpi_dmtable_info acpi_dm_table_info_gas[];
extern struct acpi_dmtable_info acpi_dm_table_info_gtdt[];
extern struct acpi_dmtable_info acpi_dm_table_info_header[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest0[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest1[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest2[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest6[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest7[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest8[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest9[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest_notify[];
extern struct acpi_dmtable_info acpi_dm_table_info_hest_bank[];
extern struct acpi_dmtable_info acpi_dm_table_info_hpet[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs0[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs1[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs4[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs8a[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs8b[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs8c[];
extern struct acpi_dmtable_info acpi_dm_table_info_ivrs_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt0[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt1[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt2[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt3[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt4[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt5[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt6[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt7[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt8[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt9[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt10[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt11[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt12[];
extern struct acpi_dmtable_info acpi_dm_table_info_madt_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_mcfg[];
extern struct acpi_dmtable_info acpi_dm_table_info_mcfg0[];
extern struct acpi_dmtable_info acpi_dm_table_info_mchi[];
extern struct acpi_dmtable_info acpi_dm_table_info_mpst[];
extern struct acpi_dmtable_info acpi_dm_table_info_mpst0[];
extern struct acpi_dmtable_info acpi_dm_table_info_mpst0_a[];
extern struct acpi_dmtable_info acpi_dm_table_info_mpst0_b[];
extern struct acpi_dmtable_info acpi_dm_table_info_mpst1[];
extern struct acpi_dmtable_info acpi_dm_table_info_mpst2[];
extern struct acpi_dmtable_info acpi_dm_table_info_msct[];
extern struct acpi_dmtable_info acpi_dm_table_info_msct0[];
extern struct acpi_dmtable_info acpi_dm_table_info_pmtt[];
extern struct acpi_dmtable_info acpi_dm_table_info_pmtt0[];
extern struct acpi_dmtable_info acpi_dm_table_info_pmtt1[];
extern struct acpi_dmtable_info acpi_dm_table_info_pmtt1a[];
extern struct acpi_dmtable_info acpi_dm_table_info_pmtt2[];
extern struct acpi_dmtable_info acpi_dm_table_info_pmtt_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_pcct[];
extern struct acpi_dmtable_info acpi_dm_table_info_pcct0[];
extern struct acpi_dmtable_info acpi_dm_table_info_rsdp1[];
extern struct acpi_dmtable_info acpi_dm_table_info_rsdp2[];
extern struct acpi_dmtable_info acpi_dm_table_info_s3pt[];
extern struct acpi_dmtable_info acpi_dm_table_info_s3pt_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_s3pt0[];
extern struct acpi_dmtable_info acpi_dm_table_info_s3pt1[];
extern struct acpi_dmtable_info acpi_dm_table_info_sbst[];
extern struct acpi_dmtable_info acpi_dm_table_info_slic_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_slic0[];
extern struct acpi_dmtable_info acpi_dm_table_info_slic1[];
extern struct acpi_dmtable_info acpi_dm_table_info_slit[];
extern struct acpi_dmtable_info acpi_dm_table_info_spcr[];
extern struct acpi_dmtable_info acpi_dm_table_info_spmi[];
extern struct acpi_dmtable_info acpi_dm_table_info_srat[];
extern struct acpi_dmtable_info acpi_dm_table_info_srat_hdr[];
extern struct acpi_dmtable_info acpi_dm_table_info_srat0[];
extern struct acpi_dmtable_info acpi_dm_table_info_srat1[];
extern struct acpi_dmtable_info acpi_dm_table_info_srat2[];
extern struct acpi_dmtable_info acpi_dm_table_info_tcpa[];
extern struct acpi_dmtable_info acpi_dm_table_info_uefi[];
extern struct acpi_dmtable_info acpi_dm_table_info_waet[];
extern struct acpi_dmtable_info acpi_dm_table_info_wdat[];
extern struct acpi_dmtable_info acpi_dm_table_info_wdat0[];
extern struct acpi_dmtable_info acpi_dm_table_info_wddt[];
extern struct acpi_dmtable_info acpi_dm_table_info_wdrt[];

extern struct acpi_dmtable_info acpi_dm_table_info_generic[][2];

/*
 * dmtable
 */
extern struct acpi_dmtable_data acpi_dm_table_data[];

u8 acpi_dm_generate_checksum(void *table, u32 length, u8 original_checksum);

struct acpi_dmtable_data *acpi_dm_get_table_data(char *signature);

void acpi_dm_dump_data_table(struct acpi_table_header *table);

acpi_status
acpi_dm_dump_table(u32 table_length,
		   u32 table_offset,
		   void *table,
		   u32 sub_table_length, struct acpi_dmtable_info *info);

void acpi_dm_line_header(u32 offset, u32 byte_length, char *name);

void acpi_dm_line_header2(u32 offset, u32 byte_length, char *name, u32 value);

/*
 * dmtbdump
 */
void acpi_dm_dump_asf(struct acpi_table_header *table);

void acpi_dm_dump_cpep(struct acpi_table_header *table);

void acpi_dm_dump_dmar(struct acpi_table_header *table);

void acpi_dm_dump_einj(struct acpi_table_header *table);

void acpi_dm_dump_erst(struct acpi_table_header *table);

void acpi_dm_dump_fadt(struct acpi_table_header *table);

void acpi_dm_dump_fpdt(struct acpi_table_header *table);

void acpi_dm_dump_hest(struct acpi_table_header *table);

void acpi_dm_dump_ivrs(struct acpi_table_header *table);

void acpi_dm_dump_madt(struct acpi_table_header *table);

void acpi_dm_dump_mcfg(struct acpi_table_header *table);

void acpi_dm_dump_mpst(struct acpi_table_header *table);

void acpi_dm_dump_msct(struct acpi_table_header *table);

void acpi_dm_dump_pcct(struct acpi_table_header *table);

void acpi_dm_dump_pmtt(struct acpi_table_header *table);

u32 acpi_dm_dump_rsdp(struct acpi_table_header *table);

void acpi_dm_dump_rsdt(struct acpi_table_header *table);

u32 acpi_dm_dump_s3pt(struct acpi_table_header *table);

void acpi_dm_dump_slic(struct acpi_table_header *table);

void acpi_dm_dump_slit(struct acpi_table_header *table);

void acpi_dm_dump_srat(struct acpi_table_header *table);

void acpi_dm_dump_wdat(struct acpi_table_header *table);

void acpi_dm_dump_xsdt(struct acpi_table_header *table);

/*
 * dmwalk
 */
void
acpi_dm_disassemble(struct acpi_walk_state *walk_state,
		    union acpi_parse_object *origin, u32 num_opcodes);

void
acpi_dm_walk_parse_tree(union acpi_parse_object *op,
			asl_walk_callback descending_callback,
			asl_walk_callback ascending_callback, void *context);

/*
 * dmopcode
 */
void
acpi_dm_disassemble_one_op(struct acpi_walk_state *walk_state,
			   struct acpi_op_walk_info *info,
			   union acpi_parse_object *op);

void acpi_dm_decode_internal_object(union acpi_operand_object *obj_desc);

u32 acpi_dm_list_type(union acpi_parse_object *op);

void acpi_dm_method_flags(union acpi_parse_object *op);

void acpi_dm_field_flags(union acpi_parse_object *op);

void acpi_dm_address_space(u8 space_id);

void acpi_dm_region_flags(union acpi_parse_object *op);

void acpi_dm_match_op(union acpi_parse_object *op);

/*
 * dmnames
 */
u32 acpi_dm_dump_name(u32 name);

acpi_status
acpi_ps_display_object_pathname(struct acpi_walk_state *walk_state,
				union acpi_parse_object *op);

void acpi_dm_namestring(char *name);

/*
 * dmobject
 */
void
acpi_dm_display_internal_object(union acpi_operand_object *obj_desc,
				struct acpi_walk_state *walk_state);

void acpi_dm_display_arguments(struct acpi_walk_state *walk_state);

void acpi_dm_display_locals(struct acpi_walk_state *walk_state);

void
acpi_dm_dump_method_info(acpi_status status,
			 struct acpi_walk_state *walk_state,
			 union acpi_parse_object *op);

/*
 * dmbuffer
 */
void acpi_dm_disasm_byte_list(u32 level, u8 *byte_data, u32 byte_count);

void
acpi_dm_byte_list(struct acpi_op_walk_info *info, union acpi_parse_object *op);

void acpi_dm_is_eisa_id(union acpi_parse_object *op);

void acpi_dm_eisa_id(u32 encoded_id);

u8 acpi_dm_is_unicode_buffer(union acpi_parse_object *op);

u8 acpi_dm_is_string_buffer(union acpi_parse_object *op);

/*
 * dmextern
 */

acpi_status acpi_dm_add_to_external_file_list(char *path_list);

void acpi_dm_clear_external_file_list(void);

void
acpi_dm_add_to_external_list(union acpi_parse_object *op,
			     char *path, u8 type, u32 value);

void acpi_dm_add_externals_to_namespace(void);

u32 acpi_dm_get_external_method_count(void);

void acpi_dm_clear_external_list(void);

void acpi_dm_emit_externals(void);

/*
 * dmresrc
 */
void acpi_dm_dump_integer8(u8 value, char *name);

void acpi_dm_dump_integer16(u16 value, char *name);

void acpi_dm_dump_integer32(u32 value, char *name);

void acpi_dm_dump_integer64(u64 value, char *name);

void
acpi_dm_resource_template(struct acpi_op_walk_info *info,
			  union acpi_parse_object *op,
			  u8 *byte_data, u32 byte_count);

acpi_status acpi_dm_is_resource_template(union acpi_parse_object *op);

void acpi_dm_bit_list(u16 mask);

void acpi_dm_descriptor_name(void);

/*
 * dmresrcl
 */
void
acpi_dm_word_descriptor(union aml_resource *resource, u32 length, u32 level);

void
acpi_dm_dword_descriptor(union aml_resource *resource, u32 length, u32 level);

void
acpi_dm_extended_descriptor(union aml_resource *resource,
			    u32 length, u32 level);

void
acpi_dm_qword_descriptor(union aml_resource *resource, u32 length, u32 level);

void
acpi_dm_memory24_descriptor(union aml_resource *resource,
			    u32 length, u32 level);

void
acpi_dm_memory32_descriptor(union aml_resource *resource,
			    u32 length, u32 level);

void
acpi_dm_fixed_memory32_descriptor(union aml_resource *resource,
				  u32 length, u32 level);

void
acpi_dm_generic_register_descriptor(union aml_resource *resource,
				    u32 length, u32 level);

void
acpi_dm_interrupt_descriptor(union aml_resource *resource,
			     u32 length, u32 level);

void
acpi_dm_vendor_large_descriptor(union aml_resource *resource,
				u32 length, u32 level);

void
acpi_dm_gpio_descriptor(union aml_resource *resource, u32 length, u32 level);

void
acpi_dm_serial_bus_descriptor(union aml_resource *resource,
			      u32 length, u32 level);

void acpi_dm_vendor_common(char *name, u8 *byte_data, u32 length, u32 level);

/*
 * dmresrcs
 */
void
acpi_dm_irq_descriptor(union aml_resource *resource, u32 length, u32 level);

void
acpi_dm_dma_descriptor(union aml_resource *resource, u32 length, u32 level);

void
acpi_dm_fixed_dma_descriptor(union aml_resource *resource,
			     u32 length, u32 level);

void acpi_dm_io_descriptor(union aml_resource *resource, u32 length, u32 level);

void
acpi_dm_fixed_io_descriptor(union aml_resource *resource,
			    u32 length, u32 level);

void
acpi_dm_start_dependent_descriptor(union aml_resource *resource,
				   u32 length, u32 level);

void
acpi_dm_end_dependent_descriptor(union aml_resource *resource,
				 u32 length, u32 level);

void
acpi_dm_vendor_small_descriptor(union aml_resource *resource,
				u32 length, u32 level);

/*
 * dmutils
 */
void acpi_dm_decode_attribute(u8 attribute);

void acpi_dm_indent(u32 level);

u8 acpi_dm_comma_if_list_member(union acpi_parse_object *op);

void acpi_dm_comma_if_field_member(union acpi_parse_object *op);

/*
 * dmrestag
 */
void acpi_dm_find_resources(union acpi_parse_object *root);

void
acpi_dm_check_resource_reference(union acpi_parse_object *op,
				 struct acpi_walk_state *walk_state);

/*
 * acdisasm
 */
void ad_disassembler_header(char *filename);

#endif				/* __ACDISASM_H__ */
