# Multiboot2 header and 64-bit long mode entry

.set MB2_MAGIC,  0xE85250D6
.set MB2_ARCH,   0
.set MB2_LEN,    16
.set MB2_CHECK,  -(MB2_MAGIC + MB2_ARCH + MB2_LEN)

.section .multiboot
.align 8
.long MB2_MAGIC
.long MB2_ARCH
.long MB2_LEN
.long MB2_CHECK
.long 0
.long 0

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.align 4096
pml4_table:
.skip 4096
pdpt_table:
.skip 4096
pd_table:
.skip 4096

.section .rodata
.align 8
gdt64:
    .quad 0x0000000000000000       # null
    .quad 0x00af9a000000ffff       # code: base=0 limit=0xfffff, G=1, DPL=0, L=1
    .quad 0x00af92000000ffff       # data: base=0 limit=0xfffff, G=1, DPL=0
gdt64_end:
gdt64_desc:
    .word gdt64_end - gdt64 - 1
    .long gdt64

.section .text
.code32
.global _start
.type _start, @function
_start:
    # Load temporary 32-bit stack
    mov $stack_top, %esp

    # Load GDT (will be reused for 64-bit)
    lgdt gdt64_desc

    # Enable PAE
    mov %cr4, %eax
    or $0x20, %eax           # CR4.PAE
    mov %eax, %cr4

    # Build identity-mapped paging with 2MiB pages for first 1GiB
    # Clear tables
    lea pml4_table, %edi
    mov $0, %eax
    mov $1024, %ecx
    rep stosl

    lea pdpt_table, %edi
    mov $0, %eax
    mov $1024, %ecx
    rep stosl

    lea pd_table, %edi
    mov $0, %eax
    mov $1024, %ecx
    rep stosl

    # Set PML4[0] -> PDPT | present | write
    lea pdpt_table, %eax
    or $0x3, %eax
    mov %eax, pml4_table

    # Set PDPT[0] -> PD | present | write
    lea pd_table, %eax
    or $0x3, %eax
    mov %eax, pdpt_table

    # Fill PD entries with 2MiB pages covering 1GiB
    mov $0, %ebx              # phys base
    lea pd_table, %edi
    mov $512, %ecx
.fill_pd:
    mov %ebx, %eax
    or $0x83, %eax            # present|write|PS (2MiB)
    mov %eax, (%edi)
    add $8, %edi              # next PDE (64-bit entry width)
    add $(2*1024*1024), %ebx
    loop .fill_pd

    # Load CR3 with PML4 base
    lea pml4_table, %eax
    mov %eax, %cr3

    # Enable long mode via EFER.LME
    mov $0xC0000080, %ecx     # IA32_EFER
    rdmsr
    or $0x100, %eax           # LME
    wrmsr

    # Enable paging (CR0.PG) and protection (CR0.PE already set by GRUB)
    mov %cr0, %eax
    or $0x80000000, %eax      # PG
    mov %eax, %cr0

    # Far jump to 64-bit code segment
    ljmp $0x08, $long_mode_entry

.code64
long_mode_entry:
    # Update data segment registers to 64-bit data selector (0x10)
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    # Set up 64-bit stack
    lea stack_top, %rsp

    # Call global constructors, then kernel_main
    call _init
    call kernel_main
1:  hlt
    jmp 1b
.size _start, . - _start

