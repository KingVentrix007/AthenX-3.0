#include "pci_dev.h" // Assuming you have a header file named pci.h
#include "pci_io.h"
#include "stddef.h"
#include "io_ports.h"

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
                    dev.device_id = (vendorDeviceID >> 16) & 0xFFFF;
                    dev.vendor_id = vendorDeviceID & 0xFFFF;
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
            dbgprintf("Unclassified Device\n");
            unclassified_devices++;
            break;
        case PCI_CLASS_MASS_STORAGE:
            dbgprintf("Mass Storage Controller\n");
            if(is_ahci(config) == 1)
            {
                dbgprintf("AHCI Controller found\n");
            }
            mass_storage_devices++;
            break;
        case PCI_CLASS_NETWORK:
            dbgprintf("Network Controller\n");
            network_devices++;
            break;
        case PCI_CLASS_DISPLAY:
            dbgprintf("Display Controller\n");
            display_devices++;
            break;
        case PCI_CLASS_MULTIMEDIA:
            dbgprintf("Multimedia Controller\n");
            multimedia_devices++;
            break;
        case PCI_CLASS_MEMORY:
            dbgprintf("Memory Controller\n");
            memory_devices++;
            break;
        case PCI_CLASS_BRIDGE:
            dbgprintf("Bridge Device\n");
            bridge_devices++;
            break;
        case PCI_CLASS_SIMPLE_COMM:
            dbgprintf("Simple Communications Controller\n");
            simple_comm_devices++;
            break;
        case PCI_CLASS_BASE_SYSTEM_PERIPH:
            dbgprintf("Base System Peripheral\n");
            base_system_periph_devices++;
            break;
        case PCI_CLASS_INPUT:
            dbgprintf("Input Device Controller\n");
            input_devices++;
            break;
        case PCI_CLASS_DOCKING:
            dbgprintf("Docking Station\n");
            docking_devices++;
            break;
        case PCI_CLASS_PROCESSOR:
            dbgprintf("Processor\n");
            processor_devices++;
            break;
        case PCI_CLASS_SERIAL_BUS:
            dbgprintf("Serial Bus Controller\n");
            serial_bus_devices++;
            break;
        case PCI_CLASS_WIRELESS:
            dbgprintf("Wireless Controller\n");
            wireless_devices++;
            break;
        case PCI_CLASS_INTELLIGENT_IO:
            dbgprintf("Intelligent IO Controller\n");
            intelligent_io_devices++;
            break;
        case PCI_CLASS_SATELLITE:
            dbgprintf("Satellite Communication Controller\n");
            satellite_devices++;
            break;
        case PCI_CLASS_ENCRYPTION:
            dbgprintf("Encryption Controller\n");
            encryption_devices++;
            break;
        case PCI_CLASS_SIGNAL_PROCESSING:
            dbgprintf("Signal Processing Controller\n");
            signal_processing_devices++;
            break;
        case PCI_CLASS_PROCESSING_ACCEL:
            dbgprintf("Processing Accelerator\n");
            processing_accel_devices++;
            break;
        case PCI_CLASS_NON_ESSENTIAL:
            dbgprintf("Non-Essential Instrumentation\n");
            non_essential_devices++;
            break;
        case PCI_CLASS_COPROCESSOR:
            dbgprintf("Coprocessor\n");
            coprocessor_devices++;
            break;
        default:
            // dbgprintf("Unknown or Unassigned Class Code\n");
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
            // dbgprintf("PCI read %d -> ven = 0x%X || dev = 0x%X\n ",i,devs[i].vendor_id,devs[i].device_id);

        
        if(devs[i].class_code == PCI_CLASS_MASS_STORAGE)
        {
            // printf("device_id == 0x%x\n",devs[i].device_id);
            // printf("vendor_id == 0x%x\n",devs[i].vendor_id);
            // 0x8086

            //0x2922

            //0x8c02
            if((devs[i].device_id ==0x2922 && devs[i].vendor_id ==  0x8086) || (devs[i].device_id == 0x8c02 && devs[i].vendor_id == 0x8086))
            {
                
                
                
                // printf("Memory Address: 0x%08X\n", devs[i].base_address_5);
                // printf("Memory Address: %p\n", devs[i].base_address_5);
                 dbgprintf("0x%08X\n", devs[i].base_address_4);
                //  dbgprintf("0x%08X\n", devs[i].base_address_3);
                //  dbgprintf("0x%08X\n", devs[i].base_address_2);
                //  dbgprintf("0x%08X\n", devs[i].base_address_1);
                //  dbgprintf("0x%08X\n", devs[i].base_address_0);


                
            return &devs[i];
            }   
           
        }

    }
    dbgprintf("loop done\n");
    return NULL;
}

pci_config_register *get_e1000_data()
{
    for (size_t i = 0; i < dev_count; i++)
    {
            // dbgprintf("PCI read %d -> ven = 0x%X || dev = 0x%X\n ",i,devs[i].vendor_id,devs[i].device_id);

        
        if(devs[i].class_code == PCI_CLASS_NETWORK)
        {
            // printf("device_id == 0x%x\n",devs[i].device_id);
            // printf("vendor_id == 0x%x\n",devs[i].vendor_id);
            if(devs[i].device_id ==0x100E    && devs[i].vendor_id ==  0x8086)
            {
                
                
                
                // printf("Memory Address: 0x%08X\n", devs[i].base_address_5);
                // printf("Memory Address: %p\n", devs[i].base_address_5);
                 dbgprintf("0x%08X\n", devs[i].base_address_4);
                 dbgprintf("0x%08X\n", devs[i].base_address_3);
                 dbgprintf("0x%08X\n", devs[i].base_address_2);
                 dbgprintf("0x%08X\n", devs[i].base_address_1);
                 dbgprintf("0x%08X\n", devs[i].base_address_0);


                
            return &devs[i];
            }   
           
        }

    }
    dbgprintf("loop done\n");
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


int remap_irq(pci_config_register *info, int irq)
{
    uint32_t old_config = pci_read(info->bus, info->slot, info->func, 0x3C);
    uint8_t old_irq = (uint8_t)(old_config & 0x000000FF);
    printf("Old IRQ: %u\n", old_irq);

    // Set the new IRQ
    uint8_t new_irq = irq;//request_irq(pci_registered_device_list[i].device_id,old_irq);  // Change this to the desired IRQ line
    // print_used_irq();
    if(new_irq == -1)
    {
        printf("Error\n");
    }
    uint32_t current_config = pci_read(info->bus, info->slot, info->func, 0x3C);
    current_config &= 0xFFFFFF00;  // Clear the lower 8 bits
    current_config |= new_irq;
    
    // Print the new IRQ
    printf("New IRQ: %u\n", new_irq);
    // 
    info->interrupt_line = new_irq;
    
    // Update the IRQ
    pci_write(info->bus, info->bus, info->func, 0x3C, current_config);
    uint32_t new_confg_read = pci_read(info->bus, info->bus, info->func, 0x3C);
    uint8_t new_irq_read = (uint8_t)(new_confg_read & 0x000000FF);
        // printf("New2 IRQ: %u\n",new_irq_read);
}
