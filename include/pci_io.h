#ifndef __PCI__IO__H
#define __PCI__IO__H

#include "pci_dev.h"
void found_device(uint8_t bus, uint8_t slot, uint8_t func, pci_config_register *device);
int create_device(pci_config_register *config);
pci_config_register * get_ahci_abar();
#endif