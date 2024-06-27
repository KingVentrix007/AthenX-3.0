#ifndef PCI_STRUCT_H
#define PCI_STRUCT_H
#include "stdint.h"
#include "stdbool.h"
#include "ahci.h"
#define DRIVER_STRUCT_SIZE 365
//class codes
#define PCI_CLASS_UNCLASSIFIED       0x00
#define PCI_CLASS_MASS_STORAGE       0x01
#define PCI_CLASS_NETWORK            0x02
#define PCI_CLASS_DISPLAY            0x03
#define PCI_CLASS_MULTIMEDIA         0x04
#define PCI_CLASS_MEMORY             0x05
#define PCI_CLASS_BRIDGE             0x06
#define PCI_CLASS_SIMPLE_COMM        0x07
#define PCI_CLASS_BASE_SYSTEM_PERIPH 0x08
#define PCI_CLASS_INPUT              0x09
#define PCI_CLASS_DOCKING            0x0A
#define PCI_CLASS_PROCESSOR          0x0B
#define PCI_CLASS_SERIAL_BUS         0x0C
#define PCI_CLASS_WIRELESS           0x0D
#define PCI_CLASS_INTELLIGENT_IO     0x0E
#define PCI_CLASS_SATELLITE          0x0F
#define PCI_CLASS_ENCRYPTION         0x10
#define PCI_CLASS_SIGNAL_PROCESSING  0x11
#define PCI_CLASS_PROCESSING_ACCEL   0x12
#define PCI_CLASS_NON_ESSENTIAL      0x13
#define PCI_CLASS_COPROCESSOR        0x40
#define PCI_CLASS_UNASSIGNED         0xFF

#define PCI_CLASS_CODE_MAX           0xFF
/**
 * Struct Name: pci_config_register
 * Description: Represents the PCI configuration space registers.
 */
typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
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

#define AHCI_DEVICE  0x1
#define IDE_DEVICE  0x2
#define USB_DEVICE  0x3
typedef int (*storage_medium_read) (uint32 sector, uint8 *buffer, uint32 sector_count);
typedef int (*storage_medium_write)(uint32 sector, uint8 *buffer, uint32 sector_count);
typedef struct
{
    int set;
    int storage_type;
    int storage_count; // Overall storage count
    int storage_specific_number; // drive number for specific storage device
}pci_storage_device;


typedef struct _pci_device
{
    char device_name[100];
    uint32_t device_type;
    bool has_driver;
    pci_config_register *config;
    void *driver[DRIVER_STRUCT_SIZE];
    void *extra[100];
}pci_device;

// IDE specific code
typedef int (*ReadFunction)(uint32_t sector, uint8_t *buffer, uint32_t sector_count); //Read function
typedef int (*WriteFunction)(uint32_t sector, uint8_t *buffer, uint32_t sector_count); //Write function
typedef int (*InitFunction)(int drive); //Init function
typedef struct _ide_deriver
{
    ReadFunction read;
    WriteFunction write;
    InitFunction init;
    int initialized;
    
}ide_deriver;

uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

int get_num_unclassified_devices();
int get_num_mass_storage_devices();
int get_num_network_devices();
int get_num_display_devices();
int get_num_multimedia_devices();
int get_num_memory_devices();
int get_num_bridge_devices();
int get_num_simple_comm_devices();
int get_num_base_system_periph_devices();
int get_num_input_devices();
int get_num_docking_devices();
int get_num_processor_devices();
int get_num_serial_bus_devices();
int get_num_wireless_devices();
int get_num_intelligent_io_devices();
int get_num_satellite_devices();
int get_num_encryption_devices();
int get_num_signal_processing_devices();
int get_num_processing_accel_devices();
int get_num_non_essential_devices();
int get_num_coprocessor_devices();
void pci_scan();
#endif // PCI_STRUCT_H
