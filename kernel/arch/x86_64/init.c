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

struct IDTInfo {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct IDTEntry {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attributes;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t zero;
} __attribute__((packed));

struct IDTEntry idt[256];

void set_idt_entry(int idx, uint64_t offset) {
    idt[idx].offset_1 = offset & 0xFFFF;
    idt[idx].offset_2 = (offset >> 16) & 0xFFFF;
    idt[idx].offset_3 = (offset >> 32) & 0xFFFFFFFF;
    idt[idx].selector = 0x08;
    idt[idx].ist = 0;
    idt[idx].type_attributes = 0x8E;
    idt[idx].zero = 0;
}

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

void page_fault_handler() {
    printf("Page fault handler\n");
}

void init_idt() {
    set_idt_entry(14, (uint64_t)page_fault_handler);
    struct IDTInfo idtr = { sizeof(idt)-1, (uint64_t)idt };
    asm volatile("lidt %0" : : "m"(idtr));
}
void setup_arch() {
	init_gdt();
    init_idt();
	printf("gdt initialized\n");
}

