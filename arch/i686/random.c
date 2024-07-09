#include "cpuid.h"
#include "stdbool.h"
#include <stdint.h>
#include "clock.h"
#include "immintrin.h"
static uint32_t seed = 123456789;
uint64_t time_since_last_keypress = 1;
static inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
        "rdtsc"
        : "=a"(lo), "=d"(hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

static inline uint32_t read_memory_address(uintptr_t addr) {
    return *(volatile uint32_t *)addr;
}

void srand(uint32_t s) {
    uintptr_t addr1 = (uintptr_t)&s;
    uintptr_t addr2 = (uintptr_t)&rdtsc;

    uint32_t mem_val1 = read_memory_address(addr1);
    uint32_t mem_val2 = read_memory_address(addr2);

    TIME current_time = get_time();
    uint64_t cycle_val = rdtsc();

    seed = s ^ mem_val1 ^ mem_val2 ^ (current_time.h << 26) ^ (current_time.m << 20) ^ (current_time.s << 14) ^
           (current_time.y << 8) ^ (current_time.mo << 4) ^ current_time.d ^ (uint32_t)(cycle_val & 0xFFFFFFFF) ^ (uint32_t)(cycle_val >> 32);
}


uint32_t rand() {
    seed = (1103515245 * seed + 12345) % (1 << 31);
    return seed;
}
