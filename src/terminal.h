#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <stddef.h>
#include <stdint.h>

void terminal_initialize();

void terminal_writestring(const char *);


void monitor_put(char c);

void monitor_clear();

void monitor_write(const char *c);

void monitor_write_hex(uint32_t n);

void monitor_write_dec(uint32_t n);

void prtf(const char *format, ...);

#endif