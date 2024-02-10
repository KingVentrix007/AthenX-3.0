#ifndef __PAGEPMM__H
#define __PAGEPMM__H

#include "stdint.h"
void init_pmm_page(uint32_t pmm_start);
uint32_t pmm_alloc_page();
#endif