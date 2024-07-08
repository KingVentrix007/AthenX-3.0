#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "elf.h"
#include "file.h"
#include "fat_filelib.h"
#include "dwarf.h"
#define ELFCLASS32 1
#define ELFCLASS64 2
#define DW_LNE_define_file 3
#define DW_LNE_set_address 2
#define EI_CLASS 4
#define DW_FORM_string8   0x1a
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
#define SHT_PROGBITS 1
void printDebugLineHeader(DebugLineHeader *header);
void parse_debug_line_section(FILE *file, Elf32_Shdr *section_header);
void parse_debug_info_section(FILE *file, Elf32_Ehdr *ehdr, Elf32_Shdr *shdrs, char *strtab);
void parse_debug_info_section_content(FILE *file, Elf32_Shdr *debug_info_section);
void parse_debug_info(char *debug_info_data, size_t size);
void parse_attributes(char *data);
void parse_elf32(const char *binary_path);

const char *attribute_names[] = {
    "DW_AT_sibling",        "DW_AT_location",       "DW_AT_name",           "DW_AT_ordering",
    "DW_AT_subscr_data",    "DW_AT_byte_size",      "DW_AT_bit_offset",     "DW_AT_bit_size",
    "DW_AT_element_list",   "DW_AT_stmt_list",      "DW_AT_low_pc",         "DW_AT_high_pc",
    "DW_AT_language",       "DW_AT_member",         "DW_AT_discr",          "DW_AT_discr_value",
    "DW_AT_visibility",     "DW_AT_import",         "DW_AT_string_length",  "DW_AT_common_reference",
    "DW_AT_comp_dir",       "DW_AT_const_value",    "DW_AT_containing_type","DW_AT_default_value",
    "DW_AT_inline",         "DW_AT_is_optional",    "DW_AT_lower_bound",    "DW_AT_producer",
    "DW_AT_prototyped",     "DW_AT_return_addr",    "DW_AT_start_scope",    "DW_AT_stride_size",
    "DW_AT_upper_bound",    "DW_AT_abstract_origin","DW_AT_accessibility",  "DW_AT_address_class",
    "DW_AT_artificial",     "DW_AT_base_types",     "DW_AT_calling_convention","DW_AT_count",
    "DW_AT_data_member_location","DW_AT_decl_column","DW_AT_decl_file",      "DW_AT_decl_line",
    "DW_AT_declaration",    "DW_AT_discr_list",     "DW_AT_encoding",       "DW_AT_external",
    "DW_AT_frame_base",     "DW_AT_friend",         "DW_AT_identifier_case","DW_AT_macro_info",
    "DW_AT_namelist_item",  "DW_AT_priority",       "DW_AT_segment",        "DW_AT_specification",
    "DW_AT_static_link",    "DW_AT_type",           "DW_AT_use_location",   "DW_AT_variable_parameter",
    "DW_AT_virtuality",     "DW_AT_vtable_elem_location","DW_AT_allocated",  "DW_AT_associated",
    "DW_AT_data_location",  "DW_AT_byte_stride",    "DW_AT_entry_pc",       "DW_AT_use_UTF8",
    "DW_AT_extension",      "DW_AT_ranges",         "DW_AT_trampoline",     "DW_AT_call_column",
    "DW_AT_call_file",      "DW_AT_call_line",      "DW_AT_description",    "DW_AT_binary_scale",
    "DW_AT_decimal_scale",  "DW_AT_small",          "DW_AT_decimal_sign",   "DW_AT_digit_count",
    "DW_AT_picture_string", "DW_AT_mutable",        "DW_AT_threads_scaled", "DW_AT_explicit",
    "DW_AT_object_pointer", "DW_AT_endianity",      "DW_AT_elemental",      "DW_AT_pure",
    "DW_AT_recursive",      "DW_AT_signature",      "DW_AT_main_subprogram","DW_AT_data_bit_offset",
    "DW_AT_const_expr",     "DW_AT_enum_class",     "DW_AT_linkage_name",   "DW_AT_MIPS_loop_unroll_factor",
    "DW_AT_MIPS_software_pipeline_depth", "DW_AT_MIPS_linkage_name","DW_AT_MIPS_stride", "DW_AT_MIPS_abstract_name",
    "DW_AT_MIPS_clone_origin", "DW_AT_MIPS_has_inlines", "DW_AT_MIPS_stride_byte", "DW_AT_MIPS_stride_elem",
    "DW_AT_MIPS_ptr_dopetype", "DW_AT_MIPS_allocatable_dopetype", "DW_AT_MIPS_assumed_shape_seq", "DW_AT_lo_user",
    "DW_AT_MIPS_fde",       "DW_AT_MIPS_loop_begin", "DW_AT_MIPS_tail_loop", "DW_AT_MIPS_epilog_begin",
    "DW_AT_MIPS_loop_unroll_factor", "DW_AT_MIPS_software_pipeline_depth", "DW_AT_MIPS_linkage_name", "DW_AT_MIPS_stride",
    "DW_AT_MIPS_abstract_name", "DW_AT_MIPS_clone_origin", "DW_AT_MIPS_has_inlines", "DW_AT_MIPS_stride_byte",
    "DW_AT_MIPS_stride_elem", "DW_AT_MIPS_ptr_dopetype", "DW_AT_MIPS_allocatable_dopetype", "DW_AT_MIPS_assumed_shape_seq",
    "DW_AT_lo_user",        "DW_AT_MIPS_fde",        "DW_AT_MIPS_loop_begin", "DW_AT_MIPS_tail_loop",
    "DW_AT_MIPS_epilog_begin", "DW_AT_MIPS_loop_inner_count", "DW_AT_MIPS_loop_outer_count", "DW_AT_MIPS_terminator",
    "DW_AT_MIPS_epilog_end", "DW_AT_MIPS_loop_unroll_factor", "DW_AT_MIPS_software_pipeline_depth", "DW_AT_MIPS_linkage_name",
    "DW_AT_MIPS_stride",    "DW_AT_MIPS_abstract_name", "DW_AT_MIPS_clone_origin", "DW_AT_MIPS_has_inlines",
    "DW_AT_MIPS_stride_byte","DW_AT_MIPS_stride_elem","DW_AT_MIPS_ptr_dopetype","DW_AT_MIPS_allocatable_dopetype",
    "DW_AT_MIPS_assumed_shape_seq","DW_AT_hi_user"
};

const char *form_names[] = {
    "DW_FORM_addr",         "DW_FORM_block",        "DW_FORM_block1",       "DW_FORM_block2",
    "DW_FORM_block4",       "DW_FORM_data1",        "DW_FORM_data2",        "DW_FORM_data4",
    "DW_FORM_data8",        "DW_FORM_string",       "DW_FORM_block",        "DW_FORM_block1",
    "DW_FORM_block2",       "DW_FORM_block4",       "DW_FORM_data1",        "DW_FORM_data2",
    "DW_FORM_data4",        "DW_FORM_data8",        "DW_FORM_string",       "DW_FORM_string8",
    "DW_FORM_ref_addr",     "DW_FORM_ref1",         "DW_FORM_ref2",         "DW_FORM_ref4",
    "DW_FORM_ref8",         "DW_FORM_ref_udata",    "DW_FORM_indirect",     "DW_FORM_sec_offset",
    "DW_FORM_exprloc",      "DW_FORM_flag",         "DW_FORM_ref_sig8",     "DW_FORM_implicit_const",
};

void printDebugLineHeader(DebugLineHeader *header) {
    printf("Debug Line Header:\n");
    printf("  length: %u\n", header->length);
    printf("  version: %u\n", header->version);
    printf("  header_length: %u\n", header->header_length);
    printf("  min_instruction_length: %u\n", header->min_instruction_length);
    printf("  default_is_stmt: %u\n", header->default_is_stmt);
    printf("  line_base: %d\n", header->line_base);
    printf("  line_range: %u\n", header->line_range);
    printf("  opcode_base: %u\n", header->opcode_base);
    printf("  std_opcode_lengths: ");
    for (int i = 0; i < 12; ++i) {
        printf("%u ", header->std_opcode_lengths[i]);
    }
    printf("\n");
}
void parse_debug_line_section(FILE *file, Elf32_Shdr *section_header) {
    fseek(file, section_header->sh_offset, SEEK_SET);

    // Read and print the debug line header
    DebugLineHeader debug_line_header;
    fread(&debug_line_header, sizeof(DebugLineHeader), 1, file);
    printDebugLineHeader(&debug_line_header);

    // Calculate initial offset after the header
    uint32_t current_offset = section_header->sh_offset + sizeof(DebugLineHeader);

    // Move file pointer to the initial offset after the header
    fseek(file, current_offset, SEEK_SET);

    // Read prologue length to determine where the opcodes start
    uint32_t prologue_length;
    fread(&prologue_length, sizeof(uint32_t), 1, file);

    // Skip prologue bytes (excluding the prologue_length itself)
    fseek(file, prologue_length - sizeof(uint32_t), SEEK_CUR);

    // Now 'file' should be at the start of the opcode sequence
    // Continue your parsing logic from here...
}
void parse_debug_info_section(FILE *file, Elf32_Ehdr *ehdr, Elf32_Shdr *shdrs, char *strtab) {
    for (int i = 0; i < ehdr->e_shnum; ++i) {
        if (shdrs[i].sh_type == SHT_PROGBITS) {
            char *section_name = &strtab[shdrs[i].sh_name];

            if (strcmp(section_name, ".debug_info") == 0) {
                printf("Section .debug_info found at offset 0x%x with size 0x%x\n", 
                       shdrs[i].sh_offset, shdrs[i].sh_size);

                // Load and process the .debug_info section
                parse_debug_info_section_content(file, &shdrs[i]);
            }
        }
    }
}

void parse_debug_info_section_content(FILE *file, Elf32_Shdr *debug_info_section) {
    fseek(file, debug_info_section->sh_offset, SEEK_SET);
    
    // Read the entire content of .debug_info section into memory
    char *debug_info_data = malloc(debug_info_section->sh_size);
    if (!debug_info_data) {
        printf("Memory allocation failed\n");
        return;
    }
    
    fread(debug_info_data, 1, debug_info_section->sh_size, file);
    
    // Now parse the .debug_info section content
    parse_debug_info(debug_info_data, debug_info_section->sh_size);
    
    free(debug_info_data);
}

void parse_debug_info(char *debug_info_data, size_t size) {
    size_t offset = 0;
    while (offset < size) {
        // Read the header
        uint32_t abbrev_code;
        uint32_t entry_length;
        
        // Assuming your data is in little-endian format
        memcpy(&abbrev_code, debug_info_data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        memcpy(&entry_length, debug_info_data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Print the entry header info
        // printf("Abbreviation code: %u, Entry length: %u\n", abbrev_code, entry_length);
        
        // Move to the attributes
        offset += 2; // Skip the CU version and abbreviation code
        
        // Read attributes until the end of this entry
        while (offset < size && *(debug_info_data + offset) != 0) {
            uint32_t attribute_name;
            uint32_t attribute_form;
            // printf("What is this doing?\n");
            // Read attribute name and form
            memcpy(&attribute_name, debug_info_data + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);
            
            memcpy(&attribute_form, debug_info_data + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);
            
            // Print attribute info
            if (attribute_name < sizeof(attribute_names) / sizeof(const char *)) {
                printf("Attribute: %s, Form: %s\n", attribute_names[attribute_name], form_names[attribute_form]);
            } else {
                printf("Unknown attribute: %u, Form: %s\n", attribute_name, form_names[attribute_form]);
            }
            
            // Determine the size of the attribute value
            size_t attribute_size = 0;
            switch (attribute_form) {
                case DW_FORM_addr:
                case DW_FORM_ref_addr:
                case DW_FORM_sec_offset:
                case DW_FORM_data4:
                case DW_FORM_ref4:
                    attribute_size = 4;
                    break;
                case DW_FORM_data8:
                case DW_FORM_ref8:
                    attribute_size = 8;
                    break;
                case DW_FORM_data1:
                case DW_FORM_flag:
                case DW_FORM_ref1:
                    attribute_size = 1;
                    break;
                case DW_FORM_data2:
                case DW_FORM_ref2:
                    attribute_size = 2;
                    break;
                case DW_FORM_string:
                case DW_FORM_strp:
                    attribute_size = strlen(debug_info_data + offset) + 1;
                    break;
                case DW_FORM_block:
                case DW_FORM_exprloc:
                    memcpy(&attribute_size, debug_info_data + offset, sizeof(uint32_t));
                    offset += sizeof(uint32_t);
                    break;
                case DW_FORM_block1:
                    attribute_size = *(debug_info_data + offset);
                    offset += 1;
                    break;
                case DW_FORM_block2:
                    memcpy(&attribute_size, debug_info_data + offset, sizeof(uint16_t));
                    offset += sizeof(uint16_t);
                    break;
                case DW_FORM_block4:
                    memcpy(&attribute_size, debug_info_data + offset, sizeof(uint32_t));
                    offset += sizeof(uint32_t);
                    break;
                case DW_FORM_string8:
                case DW_FORM_ref_sig8:
                    attribute_size = strlen(debug_info_data + offset) + 1;
                    break;
                case DW_FORM_indirect:
                    // Indirect form, read another form value
                    memcpy(&attribute_form, debug_info_data + offset, sizeof(uint32_t));
                    offset += sizeof(uint32_t);
                    break;
                default:
                    printf("Unhandled attribute form: %s\n", form_names[attribute_form]);
                    break;
            }
            
            // Move to the next attribute
            offset += attribute_size;
        }
        
        // Move to the next entry
        offset += 1; // Skip the end of entry marker
    }
}

void parse_attributes(char *data) {
    // Implement attribute parsing logic here
    // You need to handle the attributes according to DWARF standard
}

void parse_elf32(const char *binary_path) {
    printf("Parsing %s\n", binary_path);
    FILE *file = fl_fopen(binary_path, "rb");
    if (!file) {
        printf("Failed to open binary %s\n", binary_path);
        return;
    }

    Elf32_Ehdr ehdr;
    fread(&ehdr, 1, sizeof(ehdr), file);

    // Check ELF magic number
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Not an ELF file\n");
        fclose(file);
        return;
    }

    // Check ELF class
    if (ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
        printf("Not a 32-bit ELF file\n");
        fclose(file);
        return;
    }
    int IDK;
    Elf32_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!shdrs) {
        printf("Memory allocation failed\n");
        fclose(file);
        return;
    }

    fseek(file, ehdr.e_shoff, SEEK_SET);
    fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, file);

    char *strtab = NULL;
    if (ehdr.e_shstrndx < ehdr.e_shnum) {
        Elf32_Shdr *shstr_header = &shdrs[ehdr.e_shstrndx];
        strtab = malloc(shstr_header->sh_size);
        if (!strtab) {
            printf("Memory allocation failed\n");
            free(shdrs);
            fclose(file);
            return;
        }
        fseek(file, shstr_header->sh_offset, SEEK_SET);
        fread(strtab, 1, shstr_header->sh_size, file);
    }

    // Parse .debug_info section
    parse_debug_info_section(file, &ehdr, shdrs, strtab);

    free(shdrs);
    free(strtab);
    fclose(file);
}
// 0x4a467
