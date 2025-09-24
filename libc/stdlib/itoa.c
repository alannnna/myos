#include <stdlib.h>

char* itoa(int value, char* str) {
    char *ret = str;

    if (value < 0) {
        str++[0] = '-';
    }

    int digit_selector = 1;
    for (int v = value; v > 10; v /= 10) {
        digit_selector *= 10;
    }
    while (digit_selector > 0) {
        unsigned top_digit = value / digit_selector;
        *str++ = '0' + top_digit;
        value -= top_digit * digit_selector;
        digit_selector /= 10;
    }
    *str = '\0';

	return ret;
}