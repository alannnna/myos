#ifndef _KERNEL_MULTIBOOT2_H
#define _KERNEL_MULTIBOOT2_H

#include <stdint.h>

// TODO: Seems like multiboot stuff should live under arch, not kernel

// Multiboot2 header magic value
#define MULTIBOOT2_MAGIC 0x36d76289

// Memory map types
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED              2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE      3
#define MULTIBOOT_MEMORY_NVS                   4
#define MULTIBOOT_MEMORY_BADRAM                5

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} __attribute__((packed));

struct multiboot_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct multiboot_mmap_entry entries[];
};

// Function to get memory map from multiboot info
const struct multiboot_tag_mmap* get_memory_map(void);

#endif
