// rsdp.h
#ifndef RSDP_H_
#define RSDP_H_

#include <stdint.h>

// Definition of the RSDP structure
typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    // Fields since ACPI Version 2.0
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) RSDP_t;
struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};
struct RSDT {
    struct ACPISDTHeader h;
    uint32_t PointerToOtherSDT[];
};
/**
 * Function Name: find_rsdp
 * Description: Finds the RSDP (Root System Description Pointer) structure in memory.
 *
 * Return:
 *   RSDP_t* - Pointer to the found RSDP structure, NULL if not found or invalid.
 */
RSDP_t* find_rsdp();

#endif  // RSDP_H_
