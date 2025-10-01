#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/init.h>
#include <kernel/multiboot2.h>
#include <kernel/phymm.h>

void kernel_main(void) {
	terminal_initialize();
	setup_arch();
	
	if (sizeof(void*) == 4) {
		printf("32-bit mode\n");
	} else {
		printf("64-bit mode\n");
	}
	for (int i = 0; i < 10; i++) {
		printf("Hello, kernel World! %d\n", i);
	}

	const struct multiboot_tag_mmap* mmap = get_memory_map();
    if (mmap) {
        size_t entries = (mmap->size - sizeof(*mmap)) / sizeof(struct multiboot_mmap_entry);
        
        for (size_t i = 0; i < entries; i++) {
            const struct multiboot_mmap_entry* entry = &mmap->entries[i];
            
            // entry->addr = base physical address
            // entry->len = length in bytes
            // entry->type = type (1 = available, 2 = reserved, etc)
            
            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                // This is usable memory
                printf("Available memory: 0x%llx - 0x%llx (len: 0x%x / %d KB)\n", 
                       entry->addr, 
                       entry->addr + entry->len - 1, 
                       entry->len, 
                       entry->len / 1024);
            }
        }
    }

    phymm_init();
}
