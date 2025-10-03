#include <kernel/phymm.h>
#include <kernel/page.h>
#include <arch/x86_64/multiboot2.h>
#include <stddef.h>

// Defined in boot.S
extern uint64_t mb2_info;

#define LOW_MEMORY_LIMIT 0x100000  // 1MB

const struct multiboot_tag_mmap* get_memory_map(void) {
    if (!mb2_info) {
        return NULL;
    }

    // Skip size field
    struct multiboot_tag* tag = (struct multiboot_tag*)(mb2_info + 8);

    // Search through the multiboot2 tag list to find the memory map tag
    while (tag->type != 0) {
        if (tag->type == 6) { // Memory map tag
            return (struct multiboot_tag_mmap*)tag;
        }

        // Tags are aligned to 8 bytes
        tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }

    return NULL;
}

/** Returns a list of free memory regions excluding kernel memory. */
struct phym_region* get_memory_regions(uintptr_t kernel_start, uintptr_t kernel_end) {
    const struct multiboot_tag_mmap* mmap = get_memory_map();
    if (!mmap) return NULL;

    struct phym_region* regions = NULL;
    // Round kernel_start down and kernel_end up to the nearest page boundary
    kernel_start = (kernel_start / PAGE_SIZE) * PAGE_SIZE;
    kernel_end = ((kernel_end + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;

    size_t entries = (mmap->size - sizeof(*mmap)) / sizeof(struct multiboot_mmap_entry);
    for (size_t i = 0; i < entries; i++) {
        const struct multiboot_mmap_entry* entry = &mmap->entries[i];
        
        // entry->addr = base physical address
        // entry->len = length in bytes
        // entry->type = type (1 = available, 2 = reserved, etc)
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        
        int entry_start = entry->addr;
        int entry_end = entry->addr + entry->len - 1;

        // Lazy way to avoid tracking memory below LOW_MEMORY_LIMIT.
        // Likely inefficient with a different memory map.
        if (entry_start < LOW_MEMORY_LIMIT) continue;

        if (entry_end < kernel_start || entry_start > kernel_end) {
            // This memory is not used by the kernel
            // Create a new phym_region
            struct phym_region* region = (struct phym_region*)entry->addr;
            region->size = entry->len;
            region->next = regions;
            regions = region;
        } else {
            // Region overlaps with kernel
            if (entry_start < kernel_start && kernel_start - entry_start >= PAGE_SIZE) {
                // Create a phym_region between entry_start and kernel_start
                struct phym_region* region = (struct phym_region*)entry->addr;
                region->size = kernel_start - entry_start;
                region->next = regions;
                regions = region;
            }
            if (entry_end > kernel_end && entry_end - kernel_end >= PAGE_SIZE) {
                // Create a phym_region between kernel_end and entry_end
                struct phym_region* region = (struct phym_region*)kernel_end;
                region->size = entry_end - kernel_end;
                region->next = regions;
                regions = region;
            }
        }
    }
    return regions;
}