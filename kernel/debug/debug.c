#include "kernel.h"
void print_kernel_memory_map(KERNEL_MEMORY_MAP *memory_map) {
    printf("Kernel Memory Map:\n");
    printf("Kernel:\n");
    printf("  Kernel Start Address: 0x%x\n", memory_map->kernel.k_start_addr);
    printf("  Kernel End Address: 0x%x\n", memory_map->kernel.k_end_addr);
    printf("  Kernel Length: %u\n", memory_map->kernel.k_len);
    printf("  Text Start Address: 0x%x\n", memory_map->kernel.text_start_addr);
    printf("  Text End Address: 0x%x\n", memory_map->kernel.text_end_addr);
    printf("  Text Length: %u\n", memory_map->kernel.text_len);
    printf("  Data Start Address: 0x%x\n", memory_map->kernel.data_start_addr);
    printf("  Data End Address: 0x%x\n", memory_map->kernel.data_end_addr);
    printf("  Data Length: %u\n", memory_map->kernel.data_len);
    printf("  RoData Start Address: 0x%x\n", memory_map->kernel.rodata_start_addr);
    printf("  RoData End Address: 0x%x\n", memory_map->kernel.rodata_end_addr);
    printf("  RoData Length: %u\n", memory_map->kernel.rodata_len);
    printf("  BSS Start Address: 0x%x\n", memory_map->kernel.bss_start_addr);
    printf("  BSS End Address: 0x%x\n", memory_map->kernel.bss_end_addr);
    printf("  BSS Length: %u\n", memory_map->kernel.bss_len);
    printf("System:\n");
    printf("  Total Memory: %u\n", memory_map->system.total_memory);
    printf("Available:\n");
    printf("  Start Address: 0x%x\n", memory_map->available.start_addr);
    printf("  End Address: 0x%x\n", memory_map->available.end_addr);
    printf("  Size: %u\n", memory_map->available.size);
}