#include "string.h"


// useful functions, place them in separate files in the future
char* strncpy(char* dest, const char* src, int n) {
    char *ret = dest;
    do {
        if (!n--)
            return ret;
    } while (*dest++ = *src++);
    while (n--)
        *dest++ = 0;
    return ret;
}

int strlen(const char* s) {
		int i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}
