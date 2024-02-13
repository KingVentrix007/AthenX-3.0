#ifndef __PMM__H
#define __PMM__H

#include "stdint.h"
#include "stddef.h"

void init_pmm_page(uint32_t pmm_start);
uint32_t pmm_alloc_page();
uint32_t pmm_alloc_pages(uint32_t num_pages);
#endif