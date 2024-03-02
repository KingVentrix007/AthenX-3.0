#include "pci_dev.h" // Assuming you have a header file named pci.h
#include "pci_io.h"
#include "stddef.h"
pci_config_register devs[100];
int dev_count;
uint32_t read_bar5(uint8_t bus, uint8_t slot, uint8_t func);
void pci_scan() {
    for (uint8_t bus = 0; bus < 256; ++bus) {
        for (uint8_t device = 0; device < 32; ++device) {
            for (uint8_t function = 0; function < 8; ++function) {
                uint32_t vendorDeviceID = pci_read(bus, device, function, 0);
                if (vendorDeviceID != 0xFFFFFFF) {
                    
                    // Found a PCI device, call found_device
                    pci_config_register dev;
                    found_device(bus, device, function,&dev);
                    if(dev.vendor_id == 0xFFFF && dev.device_id == 0xFFFF)
                    {

                    }
                    else
                    {
                        create_device(&dev);
                    devs[dev_count] = dev;
                    dev_count++;
                    }
                    
                }
            }
        }
        if (bus <= 256 && bus >= 250) {
            break;
        }
    }
}

int create_device(pci_config_register *config)
{
    pci_device *device;
    switch (config->class_code) {
        case PCI_CLASS_UNCLASSIFIED:
            printf_com("Unclassified Device\n");
            break;
        case PCI_CLASS_MASS_STORAGE:
            printf_com("Mass Storage Controller\n");
            if(is_ahci(config) == 1)
            {
                printf_com("AHCI Controller found\n");
            }
            break;
        case PCI_CLASS_NETWORK:
            printf_com("Network Controller\n");
            break;
        case PCI_CLASS_DISPLAY:
            printf_com("Display Controller\n");
            break;
        case PCI_CLASS_MULTIMEDIA:
            printf_com("Multimedia Controller\n");
            break;
        case PCI_CLASS_MEMORY:
            printf_com("Memory Controller\n");
            break;
        case PCI_CLASS_BRIDGE:
            printf_com("Bridge Device\n");
            break;
        case PCI_CLASS_SIMPLE_COMM:
            printf_com("Simple Communications Controller\n");
            break;
        case PCI_CLASS_BASE_SYSTEM_PERIPH:
            printf_com("Base System Peripheral\n");
            break;
        case PCI_CLASS_INPUT:
            printf_com("Input Device Controller\n");
            break;
        case PCI_CLASS_DOCKING:
            printf_com("Docking Station\n");
            break;
        case PCI_CLASS_PROCESSOR:
            printf_com("Processor\n");
            break;
        case PCI_CLASS_SERIAL_BUS:
            printf_com("Serial Bus Controller\n");
            break;
        case PCI_CLASS_WIRELESS:
            printf_com("Wireless Controller\n");
            break;
        case PCI_CLASS_INTELLIGENT_IO:
            printf_com("Intelligent IO Controller\n");
            break;
        case PCI_CLASS_SATELLITE:
            printf_com("Satellite Communication Controller\n");
            break;
        case PCI_CLASS_ENCRYPTION:
            printf_com("Encryption Controller\n");
            break;
        case PCI_CLASS_SIGNAL_PROCESSING:
            printf_com("Signal Processing Controller\n");
            break;
        case PCI_CLASS_PROCESSING_ACCEL:
            printf_com("Processing Accelerator\n");
            break;
        case PCI_CLASS_NON_ESSENTIAL:
            printf_com("Non-Essential Instrumentation\n");
            break;
        case PCI_CLASS_COPROCESSOR:
            printf_com("Coprocessor\n");
            break;
        default:
            // printf_com("Unknown or Unassigned Class Code\n");
            break;
    }
}

int is_ahci(pci_config_register *config)
{
    if(config->subclass == 0x06)
    {
        return 1;
    }
}
uint32_t get_ahci_abar()
{
    for (size_t i = 0; i < dev_count; i++)
    {
            // printf_com("PCI read %d -> ven = 0x%X || dev = 0x%X\n ",i,devs[i].vendor_id,devs[i].device_id);

        
        if(devs[i].class_code == PCI_CLASS_MASS_STORAGE)
        {
            if(devs[i].device_id ==0x8086 && devs[i].vendor_id ==  0x2922)
            {
                
                

                printf_com("Memory Address: 0x%08X\n", devs[i].base_address_5);

                 printf_com("0x%08X\n", devs[i].base_address_4);
                //  printf_com("0x%08X\n", devs[i].base_address_3);
                //  printf_com("0x%08X\n", devs[i].base_address_2);
                //  printf_com("0x%08X\n", devs[i].base_address_1);
                //  printf_com("0x%08X\n", devs[i].base_address_0);


                
            return devs[i].base_address_5;
            }   
           
        }

    }
    printf_com("loop done\n");
    return -1;
}

uint32_t read_bar5(uint8_t bus, uint8_t slot, uint8_t func) {
    // BAR5 is located at offset 0x24 in the PCI configuration space
    return pci_config_read_dword(bus, slot, func, 0x24);
}