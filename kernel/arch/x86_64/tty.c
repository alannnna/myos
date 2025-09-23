#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

// #include "vga.h"
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	__asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static int serial_initialized = 0;

static void serial_init(void) {
	if (serial_initialized) return;
	/* COM1 base 0x3F8 */
	outb(0x3F8 + 1, 0x00);    // Disable all interrupts
	outb(0x3F8 + 3, 0x80);    // Enable DLAB
	outb(0x3F8 + 0, 0x01);    // Set divisor to 1 (115200 baud)
	outb(0x3F8 + 1, 0x00);
	outb(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(0x3F8 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
	serial_initialized = 1;
}

static int serial_is_transmit_empty() {
	return inb(0x3F8 + 5) & 0x20;
}

static void serial_write_char(char a) {
	while (serial_is_transmit_empty() == 0) {}
	outb(0x3F8, (uint8_t)a);
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
	serial_init();
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	(void)color; (void)x; (void)y;
	// Support newlines by sending a carriage return (CR) before the newline (LF/line feed)
	if (c == '\n') {
		serial_write_char('\r');
	}
	serial_write_char((char)c);
}

static void terminal_advance_row() {
	if (++terminal_row == VGA_HEIGHT) {
		terminal_row = 0;
	}
}

static void terminal_advance_column() {
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_advance_row();
	}
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	terminal_advance_column();
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

