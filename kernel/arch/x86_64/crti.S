.section .init
.global _init
.type _init, @function
_init:
    push %rbp
    mov %rsp, %rbp
    /* contents from crtbegin.o's .init will be inserted here */

.section .fini
.global _fini
.type _fini, @function
_fini:
    push %rbp
    mov %rsp, %rbp
    /* contents from crtbegin.o's .fini will be inserted here */

