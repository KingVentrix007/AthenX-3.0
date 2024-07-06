#include "system.h"
#include "io_ports.h"


system_info_t system_info;

void populate_cpu_info(cpu_info_t *info) {
    unsigned int eax, ebx, ecx, edx;

    // Get CPU manufacturer
    cpuid(0, &eax, &ebx, &ecx, &edx);
    ((unsigned int*)info->manufacturer)[0] = ebx;
    ((unsigned int*)info->manufacturer)[1] = edx;
    ((unsigned int*)info->manufacturer)[2] = ecx;
    info->manufacturer[12] = '\0'; // Null-terminate the string

    // Get CPU model name (requires multiple calls to cpuid)
    char *model_str = info->model_name;
    cpuid(0x80000002, &eax, &ebx, &ecx, &edx);
    *(unsigned int *)(model_str + 0) = eax;
    *(unsigned int *)(model_str + 4) = ebx;
    *(unsigned int *)(model_str + 8) = ecx;
    *(unsigned int *)(model_str + 12) = edx;

    cpuid(0x80000003, &eax, &ebx, &ecx, &edx);
    *(unsigned int *)(model_str + 16) = eax;
    *(unsigned int *)(model_str + 20) = ebx;
    *(unsigned int *)(model_str + 24) = ecx;
    *(unsigned int *)(model_str + 28) = edx;

    cpuid(0x80000004, &eax, &ebx, &ecx, &edx);
    *(unsigned int *)(model_str + 32) = eax;
    *(unsigned int *)(model_str + 36) = ebx;
    *(unsigned int *)(model_str + 40) = ecx;
    *(unsigned int *)(model_str + 44) = edx;
    info->model_name[48] = '\0'; // Null-terminate the string

    // Get CPU features and core count
    cpuid(1, &eax, &ebx, &ecx, &edx);
    info->cores = (ebx >> 16) & 0xFF; // Number of logical cores
    info->hyper_threading = edx & (1 << 28); // Hyper-threading support

    // Print debug information
    printf_com("CPUID 1: eax=%u, ebx=%u, ecx=%u, edx=%u\n", eax, ebx, ecx, edx);
    printf_com("Cores: %d, Hyper-Threading: %d\n", info->cores, info->hyper_threading);

    // Get cache information (assuming a simple model, L1 only)
    cpuid(2, &eax, &ebx, &ecx, &edx);
    // This is a simplified model; actual parsing of cache details can be complex
    info->cache_size = ((eax >> 16) & 0xFFFF) * 1024; // L1 Cache size in KB

    // Get base and max frequency (assuming constant TSC for simplicity)
    cpuid(0x16, &eax, &ebx, &ecx, &edx);
    info->frequency = eax & 0xFFFF; // Base frequency in MHz
    info->max_frequency = ebx & 0xFFFF; // Maximum frequency in MHz

    // Print debug information
    printf_com("CPUID 0x16: eax=%u, ebx=%u, ecx=%u, edx=%u\n", eax, ebx, ecx, edx);
    printf_com("Base Frequency: %u MHz, Max Frequency: %u MHz\n", info->frequency, info->max_frequency);

    // Set default values for fields not retrieved from cpuid
    info->arch = CPU_ARCH_X86; // Assuming x86_64 architecture
    info->tdp = 0; // Placeholder, as TDP cannot be retrieved via cpuid
    info->l2_cache_size = 0; // Placeholder, L2 cache size not retrieved
    info->l3_cache_size = 0; // Placeholder, L3 cache size not retrieved
    info->current_temperature = 0.0f; // Placeholder, temperature not retrieved
}
void populate_system_info(system_info_t *sys_info) {
    // Populate CPU information
    
    populate_cpu_info(&(sys_info->cpu_info));
    sys_info->kmap = g_kmap;
    char name[256] = "AthenX-dev-pc";
    strcpy(sys_info->pc_name,name);
}
void init_system_info()
{
    populate_system_info(&system_info);
    
}