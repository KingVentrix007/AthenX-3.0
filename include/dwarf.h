#ifndef _DWARF_H
#define	_DWARF_H 1
#include "stdint.h"
// DWARF Line Number Standard Opcodes
#define DW_LNS_extended_op   0x01
#define DW_LNS_copy          0x02
#define DW_LNS_advance_pc    0x03
#define DW_LNS_advance_line  0x04
#define DW_LNS_set_file      0x05
#define DW_LNS_set_column    0x06
#define DW_LNS_negate_stmt   0x07
#define DW_LNS_set_basic_block 0x08
#define DW_LNS_const_add_pc  0x09
#define DW_LNS_fixed_advance_pc 0x0A

// Extended OpCodes
#define DW_LNE_end_sequence   0x01
#define DW_LNE_set_address    0x02
#define DW_LNE_define_file    0x03
#define DW_LNE_set_discriminator 0x04

typedef struct __attribute__((packed)) {
    uint32_t length;
    uint16_t version;
    uint32_t header_length;
    uint8_t min_instruction_length;
    uint8_t default_is_stmt;
    int8_t line_base;
    uint8_t line_range;
    uint8_t opcode_base;
    uint8_t std_opcode_lengths[12];
} DebugLineHeader;
#pragma pack(push, 1)
typedef struct {
    uint32_t unit_length;   // Length of the CU excluding this field
    uint16_t version;       // DWARF version number
    uint32_t abbrev_offset; // Offset into the .debug_abbrev section
    uint8_t address_size;   // Size of addresses in this CU
} DWARF_CU_Header;
#pragma pack(pop)

typedef struct {
    uint32_t abbrev_code; // Abbreviation code
    // Attribute-value pairs will follow here
} DWARF_DIE;
void find_section_for_address(const char *elf_file_path);
#endif	/* dwarf.h */