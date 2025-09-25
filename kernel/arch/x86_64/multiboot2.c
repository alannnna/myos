#include <kernel/multiboot2.h>
#include <stddef.h>

// Defined in boot.S
extern uint64_t mb2_info;

const struct multiboot_tag_mmap* get_memory_map(void) {
    if (!mb2_info) {
        return NULL;
    }

    // Skip size field
    struct multiboot_tag* tag = (struct multiboot_tag*)(mb2_info + 8);

    // Iterate through tags
    while (tag->type != 0) {
        if (tag->type == 6) { // Memory map tag
            return (struct multiboot_tag_mmap*)tag;
        }

        // Tags are aligned to 8 bytes
        tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }

    return NULL;
}
