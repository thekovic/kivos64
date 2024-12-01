#ifndef KIVOS64_MEMORY_H
#define KIVOS64_MEMORY_H

#include "intdef.h"

#define NULL    0

void* memcpy(void* dest, const void* src, size_t len);

void* memset(void* dst, int value, size_t len);

char* strncat(char* dst, const char* src, size_t n);

int strncmp(const char* s1, const char* s2, size_t n);

char* strncpy(char* dst, const char* src, size_t n);

size_t strlen(const char* str);

char* utoa(uint32_t value, char *str, int base);

void itoa(int value, char* str, int base);

#endif
