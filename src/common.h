#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>


typedef uint32_t u32int;
typedef int32_t s32int;
typedef unsigned short u16int;
typedef short s16int;
typedef unsigned char u8int;
typedef char s8int;

extern void panic(const char *, const char *, u32int);

extern void panic_assert(const char *, u32int, const char *);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

void outb(u16int, u8int);

u8int inb(u16int);

u16int inw(u16int);

void memset(void *, u8int, u32int);

u32int align(u32int addr);

#endif