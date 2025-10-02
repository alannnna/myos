#include <stdlib.h>

char* itoa(int value, char* str) {
    return lltoa(value, str, 10);
}

char* lltoa(int64_t value, char* str, int radix) {
    char *ret = str;

    if (value < 0) {
        str++[0] = '-';
        value = -value;
    }

    int digit_selector = 1;
    for (int v = value; v >= radix; v /= radix) {
        digit_selector *= radix;
    }
    while (digit_selector > 0) {
        unsigned top_digit = value / digit_selector;
        *str++ = top_digit < 10 ? '0' + top_digit : 'a' + top_digit - 10;
        value -= top_digit * digit_selector;
        digit_selector /= radix;
    }
    *str = '\0';

	return ret;
}
