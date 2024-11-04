#ifndef __SYSTEM__H
#define __SYSTEM__H
#include "multiboot.h"
#include "types.h"
#include "stdint.h"
#include "stdbool.h"
#include "kernel.h"

typedef enum {
    CPU_ARCH_UNKNOWN = 0,
    CPU_ARCH_X86,
    CPU_ARCH_X86_64,
    CPU_ARCH_ARM,
    CPU_ARCH_ARM64,
    CPU_ARCH_MIPS,
    CPU_ARCH_MIPS64,
    CPU_ARCH_POWERPC,
    CPU_ARCH_POWERPC64,
    CPU_ARCH_SPARC,
    CPU_ARCH_SPARC64,
    CPU_ARCH_RISCV,
    CPU_ARCH_RISCV64,
    CPU_ARCH_IA64,
    CPU_ARCH_ALPHA,
    CPU_ARCH_M68K,
    CPU_ARCH_SH,
    CPU_ARCH_S390,
    CPU_ARCH_S390X,
    CPU_ARCH_VAX,
    CPU_ARCH_HPPA,
    CPU_ARCH_AVR,
    CPU_ARCH_E2K,
    CPU_ARCH_NUM_ARCHES // Always keep this as the last entry
} cpu_arch_t;

typedef struct
{
    cpu_arch_t arch;           // CPU architecture
    int cores;                 // Number of cores
    uint32_t frequency;        // Base frequency in MHz
    uint32_t cache_size;       // L1 cache size in KB
    char manufacturer[50];     // CPU manufacturer
    char model_name[100];      // Model name
    bool hyper_threading;      // Hyper-threading support
    uint32_t supported_isas;   // Bitmask of supported instruction sets
    uint32_t tdp;              // Thermal Design Power in watts
    uint32_t max_frequency;    // Maximum frequency in MHz
    uint32_t l2_cache_size;    // L2 cache size in KB
    uint32_t l3_cache_size;    // L3 cache size in KB
    float current_temperature; // Current temperature in Celsius

} cpu_info_t;

typedef struct
{
    char pc_name[256];

    cpu_info_t cpu_info; 
    KERNEL_MEMORY_MAP kmap;
}system_info_t;

extern system_info_t system_info;
void init_system_info(void);
void populate_cpu_info(cpu_info_t *info) ;
void populate_system_info(system_info_t *sys_info) ;
#endif