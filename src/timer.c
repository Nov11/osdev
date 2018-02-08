#include "timer.h"
#include "isr_handler.h"

void timer_irq_routine(registers_t regs){
  static u32int tick = 0;
  tick++;
  monitor_write("ticker:");
  monitor_write_dec(tick);
  monitor_put('\n');
}

void init_timer(u32int feq)
{
  spec_handler[32] = timer_irq_routine;

  u32int divsor = PITFREQUENCE / feq;

  //command
  outb(0x43, 0x36);
  //divsor
  outb(0x40, (u8int)divsor);
  outb(0x40, (u8int)(divsor >> 8));

}