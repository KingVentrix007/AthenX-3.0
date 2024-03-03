// rsdp.c
#include "rsdp.h"
#include "stddef.h"
uint32_t find_rsdp_internal(uint32_t start_address, uint32_t end_address);
// Function to calculate the checksum of the RSDP structure
uint8_t is_acpi_checksum_valid(const RSDP_t* rsdp);
uint8_t calculate_checksum(RSDP_t* rsdp) {
   
    return is_acpi_checksum_valid(rsdp);
}
uint8_t is_acpi_checksum_valid(const RSDP_t* rsdp) {
    uint8_t sum = 0;

    // Add up every byte in the structure
    for (size_t i = 0; i < sizeof(RSDP_t); ++i) {
        sum += *((uint8_t*)rsdp + i);
    }

    // Ensure the lowest byte of the sum is zero
    return (sum & 0xFF) == 0 ? 1 : 0;
}

// Function to validate the RSDP structure
int validate_rsdp(RSDP_t* rsdp) {
    // Check signature
    if (memcmp(rsdp->signature, "RSD PTR ", 8) != 0) {
        printf("invalid signature\n");
        return 0; // Invalid signature
    }
    // Checksum validation
    if (rsdp->revision == 0) {
        // ACPI Version 1.0
        if (calculate_checksum(rsdp) != 0) {
            printf("Invalid checksum %d\n",calculate_checksum(rsdp));
            return 0; // Invalid checksum
        }
    } else {
        // ACPI Version 2.0 or later
        uint8_t* rsdp_ptr = (uint8_t*)rsdp;
        // Calculate checksum for the original part of the structure
        uint8_t checksum = 0;
        for (size_t i = 0; i < 20; ++i) { // Size of the original part
            checksum += rsdp_ptr[i];
        }
        // Calculate checksum for the extended part of the structure
        for (size_t i = 24; i < sizeof(RSDP_t); ++i) {
            checksum += rsdp_ptr[i];
        }
        if (checksum != 0) {
            printf("Invalid checksum v2\n");
            return 0; // Invalid checksum
        }
    }
    // RSDP is valid
    return 1;
}

// Function to search for the RSDP structure in memory
RSDP_t* find_rsdp() {
    // Search in the EBDA (Extended BIOS Data Area)
    uint32_t rsdp_address = find_rsdp_internal(0x000E0000 , 0x000FFFFF );
     if (validate_rsdp(rsdp_address)) {
                return (RSDP_t *)rsdp_address;
            }
    return NULL; // RSDP not found or invalid
}
uint32_t find_rsdp_internal(uint32_t start_address, uint32_t end_address) {
    // Loop through the memory region in 16-byte increments
    for (uint32_t addr = start_address; addr <= end_address; addr += 16) {
        // Check if the current address contains the RSDP signature
        if (*(uint64_t *)addr == 0x2052545020445352ULL) { // "RSD PTR " in little-endian
            return addr;
        }
    }
    return 0; // RSDP signature not found
}
void *findMADT(void *RootSDT) {
    // Cast the input parameter to RSDT
    struct RSDT *rsdt = (struct RSDT *)RootSDT;
    // Iterate through each entry in the RSDT
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / sizeof(uint32_t);
    for (int i = 0; i < entries; i++) {
        // Obtain a pointer to the current entry
        struct ACPISDTHeader *h = (struct ACPISDTHeader *)(uintptr_t)rsdt->PointerToOtherSDT[i];
        // Check if the signature matches MADT
        if (!strncmp(h->Signature, "APIC", 4))
            return (void *)h; // Return pointer to MADT if found
    }
    // No MADT found
    return NULL;
}