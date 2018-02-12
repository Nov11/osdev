#ifndef _STRINGOPS_H
#define _STRINGOPS_H

#include <stddef.h>
#include "common.h"

//size_t strlen(const char *str);
void memset(void *, u8int, u32int);

void memcpy(u8int *dest, const u8int *src, u32int len);

char *strcpy(char *dest, const char *src);

int strlen(char *src);

int strcmp(char *str1, char *str2);
#endif