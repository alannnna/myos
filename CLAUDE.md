# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a custom x86-64 operating system written in C and assembly. The OS boots via UEFI using GRUB as a bootloader and follows the Multiboot2 specification.

## Build System

The project uses Make with a three-tier structure:
- Top-level `Makefile` orchestrates kernel and libc builds
- `kernel/Makefile` builds the kernel binary
- `libc/Makefile` builds libk.a (kernel C library)

### Essential Commands

```bash
# Build everything (default)
make

# Build and create bootable ISO
make iso

# Run in QEMU
make run

# Run in QEMU with GDB server (waits for connection)
make run-gdb

# In another terminal, connect GDB
make gdb

# Debug build (O0 optimization)
make debug

# Clean all build artifacts
make clean
```

### Build Dependencies

Required tools (install via Homebrew on macOS):
- `x86_64-elf-gcc` - Cross compiler
- `x86_64-elf-grub` - Bootloader tools
- `x86_64-elf-gdb` - Debugger
- `qemu` - System emulator

## Architecture

### Boot Flow

1. **UEFI Firmware** boots in 64-bit mode
2. **GRUB Bootloader** (Multiboot2-compliant) loads the kernel and switches CPU to 32-bit protected mode (required by Multiboot2 spec)
3. **boot.S** (`kernel/arch/x86_64/boot.S`) performs the 32-bit to 64-bit transition:
   - Saves Multiboot2 info pointer
   - Sets up identity paging (2MiB pages for first 1GiB)
   - Enables PAE and long mode (64-bit)
   - Jumps to 64-bit code
4. **kernel_main** (`kernel/kernel/kernel.c`) runs in 64-bit mode

### Directory Structure

```
kernel/
  arch/x86_64/        # Architecture-specific code
    boot.S            # Multiboot2 header and 32→64 bit transition
    init.c            # GDT initialization
    multiboot2.c      # Multiboot2 info parsing
    tty.c             # Serial port (COM1) output
    linker.ld         # Kernel linker script
    make.config       # Architecture object files list
  kernel/
    kernel.c          # Main kernel entry point
  include/kernel/     # Kernel headers

libc/
  arch/x86_64/        # Architecture-specific libc code
  stdio/              # printf, puts, putchar
  stdlib/             # abort, itoa
  string/             # memcpy, memset, strlen, etc.
  include/            # Standard C headers

sysroot/              # Cross-compilation system root (generated)
  usr/include/        # Installed headers
  usr/lib/            # Installed libraries (libk.a)
  boot/               # Installed kernel binary

isodir/               # ISO filesystem staging (generated)
```

### Build Process

1. **install-headers**: Installs headers from kernel and libc to `sysroot/usr/include`
2. **libc**: Builds `libk.a` (freestanding C library for kernel use) and installs to `sysroot/usr/lib`
3. **kernel**: Builds kernel using headers and libk.a from sysroot, produces `myos.kernel`
4. **iso**: Creates bootable ISO with GRUB and kernel binary

The kernel is linked with:
- `crti.o`, `crtn.o` - C runtime initialization/finalization stubs
- `crtbegin.o`, `crtend.o` - GCC-provided constructor/destructor support
- Kernel object files
- `libk.a` - Kernel C library

### Key Implementation Details

**Paging**: Identity-mapped 2MiB pages covering first 1GiB, set up in boot.S

**GDT**: Two GDTs exist:
- Temporary GDT in boot.S for 32→64 bit transition (rodata section)
- Runtime GDT initialized in `kernel/arch/x86_64/init.c` with kernel/user code/data segments

**Multiboot2**: Multiboot info pointer saved in `mb2_info` global variable, parsed in `multiboot2.c` to extract memory map

**Serial Output**: COM1 (0x3F8) configured for 115200 baud, 8N1. All terminal output goes to serial (not VGA). QEMU captures serial output with `-serial stdio`.

## Development Notes

- The kernel uses freestanding C (no hosted libc features)
- libk.a contains only freestanding C functions (memcpy, printf, etc.)
- All kernel code must compile with `-ffreestanding`
- SYSROOT environment variable exported to sub-makes for header/library paths
- Cross-compilation target: `x86_64-elf`
