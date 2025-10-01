#include <kernel/phymm.h>
#include <stdio.h>
#include <stddef.h>

extern char _kernel_start[];
extern char _kernel_end[];

static struct phym_region* free_regions = NULL;

void phymm_init(void) {
    // Get memory regions
    free_regions = get_memory_regions((uintptr_t)_kernel_start, (uintptr_t)_kernel_end);
    if (!free_regions) {
        printf("No free memory regions found!\n");
        return;
    }

    // Debug prints
    printf("Kernel start: 0x%llx\n", (uintptr_t)_kernel_start);
    printf("Kernel end: 0x%llx\n", (uintptr_t)_kernel_end);

    printf("Free memory regions:\n");
    struct phym_region* current_region = free_regions;
    while (current_region) {
        printf("0x%llx - 0x%llx\n", current_region, (uintptr_t)current_region + current_region->size - 1);
        current_region = current_region->next;
    }
        
    // Convert to a free list of pages??
}