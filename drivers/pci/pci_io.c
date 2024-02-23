#include "stdint-gcc.h"
#include "pci_dev.h"
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    outportl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inportl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}
uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    //write_serial("*",DEFAULT_COM_DEBUG_PORT);
    uint32_t address = (1U << 31) | ((uint32_t)bus << 16) | ((uint32_t)device << 11) | ((uint32_t)function << 8) | (offset & 0xFC);
    outportl(0xCF8, address);
    return inportl(0xCFC);
}
void pci_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t data) {
    // Construct the PCI configuration address
    uint32_t address = (1U << 31) | ((uint32_t)bus << 16) | ((uint32_t)device << 11) | ((uint32_t)function << 8) | (offset & 0xFC);

    // Write the address to the PCI configuration address port
    outportl(0xCF8, address);

    // Write the data to the PCI configuration data port
    outportl(0xCFC, data);
}

/**
 * Function Name: found_device
 * Description: Called when a PCI device is found during scanning.
 *
 * Parameters:
 *   bus (uint8_t) - The PCI bus number.
 *   device (uint8_t) - The PCI device number.
 *   function (uint8_t) - The PCI function number.
 *
 * Return:
 *   void
 */

void found_device(uint8_t bus, uint8_t slot, uint8_t func, pci_config_register *device) {
    // Fill each field in the pci_config_register struct using pci_config_read_word function
    device->device_id = pci_config_read_word(bus, slot, func, 0x00);
    device->vendor_id = pci_config_read_word(bus, slot, func, 0x02);
    device->status = pci_config_read_word(bus, slot, func, 0x04);
    device->command = pci_config_read_word(bus, slot, func, 0x06);
    device->class_code = pci_config_read_word(bus, slot, func, 0x0B) >> 8;
    device->subclass = pci_config_read_word(bus, slot, func, 0x0B) & 0xFF;
    device->prog_if = pci_config_read_word(bus, slot, func, 0x09) >> 8;
    device->revision_id = pci_config_read_word(bus, slot, func, 0x08) & 0xFF;
    device->bist = pci_config_read_word(bus, slot, func, 0x0F) >> 8;
    device->header_type = pci_config_read_word(bus, slot, func, 0x0E) & 0xFF;
    device->latency_timer = pci_config_read_word(bus, slot, func, 0x0D) >> 8;
    device->cache_line_size = pci_config_read_word(bus, slot, func, 0x0C) & 0xFF;
    device->base_address_0 = pci_config_read_word(bus, slot, func, 0x10);
    device->base_address_1 = pci_config_read_word(bus, slot, func, 0x14);
    device->base_address_2 = pci_config_read_word(bus, slot, func, 0x18);
    device->base_address_3 = pci_config_read_word(bus, slot, func, 0x1C);
    device->base_address_4 = pci_config_read_word(bus, slot, func, 0x20);
    device->base_address_5 = pci_config_read_word(bus, slot, func, 0x24);
    device->cardbus_cis_pointer = pci_config_read_word(bus, slot, func, 0x28);
    device->subsystem_id = pci_config_read_word(bus, slot, func, 0x2C);
    device->subsystem_vendor_id = pci_config_read_word(bus, slot, func, 0x2E);
    device->expansion_rom_base_address = pci_config_read_word(bus, slot, func, 0x30);
    device->max_latency = pci_config_read_word(bus, slot, func, 0x3E) >> 8;
    device->min_grant = pci_config_read_word(bus, slot, func, 0x3E) & 0xFF;
    device->interrupt_pin = pci_config_read_word(bus, slot, func, 0x3D) >> 8;
    device->interrupt_line = pci_config_read_word(bus, slot, func, 0x3D) & 0xFF;
    device->reserved = 0; // Set reserved field to 0
    device->capabilities_pointer = pci_config_read_word(bus, slot, func, 0x34);
}