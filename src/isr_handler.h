#ifndef ISR_HANDLER
#define ISR_HANDLER

#include "common.h"
#include "terminal.h"

typedef struct re{
	u32int ds;
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32int int_no, err_code;
	u32int eip, cs, eflags, useresp, ss;
}registers_t;

extern void(*spec_handler[256])(registers_t);

void isr_handler(registers_t regs);
void irq_handler(registers_t regs);

void reg_handler(void (*)(registers_t), u32int index);
#endif
