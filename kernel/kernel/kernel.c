#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/init.h>

void kernel_main(void) {
	terminal_initialize();
	setup_arch();
	for (int i = 0; i < 10; i++) {
		printf("Hello, kernel World! %d\n", i);
	}
}
