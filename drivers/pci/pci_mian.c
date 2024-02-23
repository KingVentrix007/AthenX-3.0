#include "pci_dev.h" // Assuming you have a header file named pci.h
#include "pci_io.h"
pci_config_register devs[100];
int dev_count;
void pci_scan() {
    for (uint8_t bus = 0; bus < 256; ++bus) {
        for (uint8_t device = 0; device < 32; ++device) {
            for (uint8_t function = 0; function < 8; ++function) {
                uint32_t vendorDeviceID = pci_read(bus, device, function, 0);
                if (vendorDeviceID != 0xFFFF) {
                    // Found a PCI device, call found_device
                    pci_config_register dev;
                    found_device(bus, device, function,&dev);
                    devs[dev_count] = dev;
                    dev_count++;
                }
            }
        }
        if (bus <= 256 && bus >= 250) {
            break;
        }
    }
}
