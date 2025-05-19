#include "string.h"

// void *memcpy(void *dest, const void *src, int n) {
//     char *d = (char *)dest;
//     const char *s = (const char *)src;
//     while (n--) *d++ = *s++;
//     return dest;
// }

// void *memset(void *s, int c, int n) {
//     unsigned char *p = (unsigned char *)s;
//     while (n--) *p++ = (unsigned char)c;
//     return s;
// }

// int strcmp(const char *s1, const char *s2) {
//     while (*s1 && (*s1 == *s2)) s1++, s2++;
//     return *(unsigned char *)s1 - *(unsigned char *)s2;
// }

int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}
