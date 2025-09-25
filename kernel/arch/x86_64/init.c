#include <kernel/init.h>
#include <stdio.h>
#include <stdint.h>

// Use some not-super-pretty chatgpt code to get through GDT setup
// GDT entry structure
struct GDTEntry {
    uint16_t limit_low;      // Lower 16 bits of limit
    uint16_t base_low;       // Lower 16 bits of base
    uint8_t  base_middle;    // Next 8 bits of base
    uint8_t  access;         // Access byte
    uint8_t  flags_limit;    // Flags (upper 4 bits) + high 4 bits of limit
    uint8_t  base_high;      // High 8 bits of base
} __attribute__((packed));

// GDT information structure, for lgdt instruction
struct GDTInfo {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Example GDT entries
struct GDTEntry gdt[5];

// Helper to set a GDT entry
void set_gdt_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[idx].limit_low    = limit & 0xFFFF;
    gdt[idx].base_low     = base & 0xFFFF;
    gdt[idx].base_middle  = (base >> 16) & 0xFF;
    gdt[idx].access       = access;
    gdt[idx].flags_limit  = ((limit >> 16) & 0x0F) | (flags << 4);
    gdt[idx].base_high    = (base >> 24) & 0xFF;
}

// Example initialization
void init_gdt() {
	// See table here https://wiki.osdev.org/GDT_Tutorial#Flat_/_Long_Mode_Setup
    set_gdt_entry(0, 0, 0, 0, 0);           // Null
    set_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0xA); // Kernel code
    set_gdt_entry(2, 0, 0xFFFFF, 0x92, 0xA); // Kernel data
    set_gdt_entry(3, 0, 0xFFFFF, 0xFA, 0xA); // User code
    set_gdt_entry(4, 0, 0xFFFFF, 0xF2, 0xA); // User data

    struct GDTInfo gdtr = { sizeof(gdt)-1, (uint64_t)gdt };
    asm volatile("lgdt %0" : : "m"(gdtr));
}

void setup_arch() {
	init_gdt();
	printf("gdt initialized\n");
}

