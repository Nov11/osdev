#include "common.h"

//a:eax d:edx N 1字节常数 volatile : gcc 别动汇编语句
void outb(u16int port, u8int value) {
    asm volatile("outb %1, %0"::"dN"(port), "a"(value));
}

u8int inb(u16int port) {
    u8int ret;
    asm volatile("inb %1, %0":"=a"(ret):"dN"(port));
    return ret;
}

u16int inw(u16int port) {
    u16int ret;
    asm volatile("inw %1, %0":"=a"(ret):"dN"(port));
    return ret;
}

void memset(void *des, u8int c, u32int count) {
    u8int *ptr = des;
    while (count > 0) {
        *ptr++ = c;
        count--;
    }
}

u32int align(u32int addr) {
    if (addr & 0xfff) {
        return (addr + 0x1000) & ~0xfff;
    }
    return addr;
}
