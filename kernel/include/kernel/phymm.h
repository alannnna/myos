/**
 * Physical memory management
 */
#ifndef _KERNEL_PHYMM_H
#define _KERNEL_PHYMM_H

#include <stdint.h>

/**
 * A physical memory region.
 *
 * This struct is stored at the beginning of each free region.
 */
struct phym_region {
    uint64_t size;
    struct phym_region* next;
};

void phymm_init(void);

/** Returns a list of free memory regions excluding kernel memory. */
struct phym_region* get_memory_regions(uintptr_t kernel_start, uintptr_t kernel_end);

#endif
