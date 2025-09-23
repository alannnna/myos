# Top-level Makefile for myos
.PHONY: all clean kernel libc qemu iso install gdb

# Default target
all: kernel

# Configuration
HOST ?= $(shell ./default-host.sh)
HOSTARCH ?= $(shell ./target-triplet-to-arch.sh $(HOST))
CFLAGS ?= -O2 -g
CPPFLAGS ?= 
LDFLAGS ?= 
LIBS ?= 

# Export for sub-makes
export HOST HOSTARCH CFLAGS CPPFLAGS LDFLAGS LIBS

# Directories
SYSROOT = $(CURDIR)/sysroot
ISODIR = $(CURDIR)/isodir

# Install headers first
install-headers:
	@mkdir -p $(SYSROOT)/usr/include
	$(MAKE) -C kernel install-headers DESTDIR=$(SYSROOT) PREFIX=/usr
	$(MAKE) -C libc install-headers DESTDIR=$(SYSROOT) PREFIX=/usr

# Build targets
libc: install-headers
	$(MAKE) -C libc
	@mkdir -p $(SYSROOT)/usr/lib
	$(MAKE) -C libc install DESTDIR=$(SYSROOT) PREFIX=/usr

kernel: libc
	$(MAKE) -C kernel
	@mkdir -p $(SYSROOT)/boot
	@cp kernel/myos.kernel $(SYSROOT)/boot/myos.kernel

# Install libs to sysroot
install-libs: kernel

# Build everything
build: install-libs

# Create ISO
iso: build
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(SYSROOT)/boot/myos.kernel $(ISODIR)/boot/myos.kernel
	@echo 'set timeout=0' > $(ISODIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISODIR)/boot/grub/grub.cfg
	@echo 'menuentry "myos" {' >> $(ISODIR)/boot/grub/grub.cfg
	@echo '	multiboot2 /boot/myos.kernel' >> $(ISODIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISODIR)/boot/grub/grub.cfg
	@x86_64-elf-grub-mkrescue -o myos.iso $(ISODIR)

# Run QEMU
qemu: iso
	@HOST=$(HOST) qemu-system-$(HOSTARCH) -boot d -cdrom myos.iso -machine q35 -vga std -serial stdio -drive if=pflash,format=raw,readonly=on,file=/opt/homebrew/share/qemu/edk2-x86_64-code.fd -s -S

# Run QEMU without GDB
qemu-run: iso
	@HOST=$(HOST) qemu-system-$(HOSTARCH) -boot d -cdrom myos.iso -machine q35 -vga std -serial stdio -drive if=pflash,format=raw,readonly=on,file=/opt/homebrew/share/qemu/edk2-x86_64-code.fd

# Debug build
debug: CFLAGS = -O0 -g
debug: clean build

# Clean everything
clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C libc clean
	@rm -rf $(ISODIR)
	@rm -f myos.iso
	@rm -rf $(SYSROOT)

# GDB debugging
gdb:
	@x86_64-elf-gdb kernel/myos.kernel -ex "target remote localhost:1234"

# Help
help:
	@echo "Available targets:"
	@echo "  all        - Build kernel (default)"
	@echo "  kernel     - Build kernel only"
	@echo "  libc       - Build libc only"
	@echo "  build      - Build everything"
	@echo "  iso        - Create bootable ISO"
	@echo "  qemu       - Run QEMU with GDB server"
	@echo "  qemu-run   - Run QEMU without GDB"
	@echo "  debug      - Build with -O0 -g for debugging"
	@echo "  gdb        - Connect GDB to running QEMU instance"
	@echo "  clean      - Clean everything"
	@echo "  help       - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  make debug qemu    - Debug build and run with GDB"
	@echo "  make CFLAGS=-O0 qemu - Custom CFLAGS and run"
