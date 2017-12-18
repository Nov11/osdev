#include "isr_handler.h"

void(*spec_handler[256])(registers_t) = {0};//it's weired that this has to be deliberately set or the value is not null

void isr_handler(registers_t regs)
{
	monitor_write("recv inte");
	monitor_write_dec(regs.int_no);
	monitor_write("err code");
	monitor_write_dec(regs.err_code);
	monitor_put('\n');
	if(spec_handler[regs.int_no] != NULL){
		spec_handler[regs.int_no](regs);
	}
}

void irq_handler(registers_t regs)
{
	monitor_write("irq");
	monitor_write_dec(regs.int_no);
	monitor_write("err code");//0
	monitor_write_dec(regs.err_code);
	if(spec_handler[regs.int_no] != NULL){
		spec_handler[regs.int_no](regs);
	}
	if(regs.int_no >= 40){
		outb(0xa0, 0x20);
	}
	outb(0x20, 0x20);
	
}

void reg_handler(void (*handler)(registers_t), u32int index)
{
	if(index >= 256){
		PANIC("regs index");
	}
	spec_handler[index] = handler;
}