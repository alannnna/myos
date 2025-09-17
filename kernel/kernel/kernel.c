#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/init.h>

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
}
