#include "kernel.h"
#include "stddef.h"
#include "stdio.h"
#include "pmm.h"
#include "vmm.h"
#include "kheap.h"
#include "vesa.h"
#include "scheduler.h"
int init_memory()
{
    size_t size = (g_kmap.available.size / 2) + 10;
    uint32_t pmm_start = (uint32_t)g_kmap.available.start_addr;
    init_pmm_page(pmm_start, g_kmap.available.size);
    init_vmm();
    LockScheduler();

    pmm_collect_pages(mboot_info);

    map_vesa();
    UnlockScheduler();

    init_kheap(g_kmap.available.size);
    printf("Memroy init complete\n");
}
