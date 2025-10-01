#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/init.h>
#include <kernel/multiboot2.h>
#include <kernel/phymm.h>

void kernel_main(void) {
	terminal_initialize();
	
	if (sizeof(void*) == 4) {
		printf("32-bit mode\n");
	} else {
		printf("64-bit mode\n");
	}
	for (int i = 0; i < 10; i++) {
		printf("Hello, kernel World! %d\n", i);
	}

    setup_arch();
    phymm_init();

	int *bad_addr = (int*)0xDEADBEEF;
	int something = *bad_addr;
	printf("Something: %d\n", something);
}
