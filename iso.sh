#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=0
set default=0
set gfxpayload=keep

menuentry "myos" {
	multiboot2 /boot/myos.kernel
}
EOF
x86_64-elf-grub-mkrescue -o myos.iso isodir
