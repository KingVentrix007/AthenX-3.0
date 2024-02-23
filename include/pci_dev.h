#ifndef PCI_STRUCT_H
#define PCI_STRUCT_H
#include "stdint.h"

/**
 * Struct Name: pci_config_register
 * Description: Represents the PCI configuration space registers.
 */
typedef struct {
    uint16_t device_id;             // Device ID
    uint16_t vendor_id;             // Vendor ID
    uint16_t status;                // Status
    uint16_t command;               // Command
    uint8_t class_code;             // Class code
    uint8_t subclass;               // Subclass
    uint8_t prog_if;                // Prog IF
    uint8_t revision_id;            // Revision ID
    uint8_t bist;                   // BIST
    uint8_t header_type;            // Header type
    uint8_t latency_timer;          // Latency Timer
    uint8_t cache_line_size;        // Cache Line Size
    uint32_t base_address_0;        // Base address #0 (BAR0)
    uint32_t base_address_1;        // Base address #1 (BAR1)
    uint32_t base_address_2;        // Base address #2 (BAR2)
    uint32_t base_address_3;        // Base address #3 (BAR3)
    uint32_t base_address_4;        // Base address #4 (BAR4)
    uint32_t base_address_5;        // Base address #5 (BAR5)
    uint32_t cardbus_cis_pointer;   // Cardbus CIS Pointer
    uint16_t subsystem_id;          // Subsystem ID
    uint16_t subsystem_vendor_id;   // Subsystem Vendor ID
    uint32_t expansion_rom_base_address; // Expansion ROM base address
    uint8_t max_latency;            // Max latency
    uint8_t min_grant;              // Min Grant
    uint8_t interrupt_pin;          // Interrupt PIN
    uint8_t interrupt_line;         // Interrupt Line
    uint32_t reserved;              // Reserved
    uint32_t capabilities_pointer;  // Capabilities Pointer
} pci_config_register;

#endif // PCI_STRUCT_H
