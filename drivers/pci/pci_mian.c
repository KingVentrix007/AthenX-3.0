#include "pci_dev.h" // Assuming you have a header file named pci.h
#include "pci_io.h"
#include "stddef.h"
pci_config_register devs[100];
int dev_count;
int unclassified_devices = 0;
int mass_storage_devices = 0;
int network_devices = 0;
int display_devices = 0;
int multimedia_devices = 0;
int memory_devices = 0;
int bridge_devices = 0;
int simple_comm_devices = 0;
int base_system_periph_devices = 0;
int input_devices = 0;
int docking_devices = 0;
int processor_devices = 0;
int serial_bus_devices = 0;
int wireless_devices = 0;
int intelligent_io_devices = 0;
int satellite_devices = 0;
int encryption_devices = 0;
int signal_processing_devices = 0;
int processing_accel_devices = 0;
int non_essential_devices = 0;
int coprocessor_devices = 0;
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
    switch (config->class_code) {
        case PCI_CLASS_UNCLASSIFIED:
            printf_com("Unclassified Device\n");
            unclassified_devices++;
            break;
        case PCI_CLASS_MASS_STORAGE:
            printf_com("Mass Storage Controller\n");
            if(is_ahci(config) == 1)
            {
                printf_com("AHCI Controller found\n");
            }
            mass_storage_devices++;
            break;
        case PCI_CLASS_NETWORK:
            printf_com("Network Controller\n");
            network_devices++;
            break;
        case PCI_CLASS_DISPLAY:
            printf_com("Display Controller\n");
            display_devices++;
            break;
        case PCI_CLASS_MULTIMEDIA:
            printf_com("Multimedia Controller\n");
            multimedia_devices++;
            break;
        case PCI_CLASS_MEMORY:
            printf_com("Memory Controller\n");
            memory_devices++;
            break;
        case PCI_CLASS_BRIDGE:
            printf_com("Bridge Device\n");
            bridge_devices++;
            break;
        case PCI_CLASS_SIMPLE_COMM:
            printf_com("Simple Communications Controller\n");
            simple_comm_devices++;
            break;
        case PCI_CLASS_BASE_SYSTEM_PERIPH:
            printf_com("Base System Peripheral\n");
            base_system_periph_devices++;
            break;
        case PCI_CLASS_INPUT:
            printf_com("Input Device Controller\n");
            input_devices++;
            break;
        case PCI_CLASS_DOCKING:
            printf_com("Docking Station\n");
            docking_devices++;
            break;
        case PCI_CLASS_PROCESSOR:
            printf_com("Processor\n");
            processor_devices++;
            break;
        case PCI_CLASS_SERIAL_BUS:
            printf_com("Serial Bus Controller\n");
            serial_bus_devices++;
            break;
        case PCI_CLASS_WIRELESS:
            printf_com("Wireless Controller\n");
            wireless_devices++;
            break;
        case PCI_CLASS_INTELLIGENT_IO:
            printf_com("Intelligent IO Controller\n");
            intelligent_io_devices++;
            break;
        case PCI_CLASS_SATELLITE:
            printf_com("Satellite Communication Controller\n");
            satellite_devices++;
            break;
        case PCI_CLASS_ENCRYPTION:
            printf_com("Encryption Controller\n");
            encryption_devices++;
            break;
        case PCI_CLASS_SIGNAL_PROCESSING:
            printf_com("Signal Processing Controller\n");
            signal_processing_devices++;
            break;
        case PCI_CLASS_PROCESSING_ACCEL:
            printf_com("Processing Accelerator\n");
            processing_accel_devices++;
            break;
        case PCI_CLASS_NON_ESSENTIAL:
            printf_com("Non-Essential Instrumentation\n");
            non_essential_devices++;
            break;
        case PCI_CLASS_COPROCESSOR:
            printf_com("Coprocessor\n");
            coprocessor_devices++;
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

pci_config_register *get_ahci_abar()
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


                
            return &devs[i];
            }   
           
        }

    }
    printf_com("loop done\n");
    return NULL;
}

uint32_t read_bar5(uint8_t bus, uint8_t slot, uint8_t func) {
    // BAR5 is located at offset 0x24 in the PCI configuration space
    return pci_config_read_dword(bus, slot, func, 0x24);
}
int get_num_unclassified_devices() {
    return unclassified_devices;
}

int get_num_mass_storage_devices() {
    return mass_storage_devices;
}

int get_num_network_devices() {
    return network_devices;
}

int get_num_display_devices() {
    return display_devices;
}

int get_num_multimedia_devices() {
    return multimedia_devices;
}

int get_num_memory_devices() {
    return memory_devices;
}

int get_num_bridge_devices() {
    return bridge_devices;
}

int get_num_simple_comm_devices() {
    return simple_comm_devices;
}

int get_num_base_system_periph_devices() {
    return base_system_periph_devices;
}

int get_num_input_devices() {
    return input_devices;
}

int get_num_docking_devices() {
    return docking_devices;
}

int get_num_processor_devices() {
    return processor_devices;
}

int get_num_serial_bus_devices() {
    return serial_bus_devices;
}

int get_num_wireless_devices() {
    return wireless_devices;
}

int get_num_intelligent_io_devices() {
    return intelligent_io_devices;
}

int get_num_satellite_devices() {
    return satellite_devices;
}

int get_num_encryption_devices() {
    return encryption_devices;
}

int get_num_signal_processing_devices() {
    return signal_processing_devices;
}

int get_num_processing_accel_devices() {
    return processing_accel_devices;
}

int get_num_non_essential_devices() {
    return non_essential_devices;
}

int get_num_coprocessor_devices() {
    return coprocessor_devices;
}
