#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

/**
 * Convert an integer to a string (base 10).
 * @param value The integer to convert.
 * @param str A buffer long enough to hold the result plus a null terminator.
 * @return Another pointer to str.
 */
char* itoa(int value, char* str);
char* lltoa(int64_t ll, char * buffer, int radix);

#ifdef __cplusplus
}
#endif

#endif
