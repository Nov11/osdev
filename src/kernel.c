/* Surely you will remove the processor conditionals and this comment
   appropriately depending on whether or not you use C++. */
#if !defined(__cplusplus)

#include <stdbool.h> /* C doesn't have booleans by default. */

#endif

#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks we are targeting the wrong operating system. */
//#if defined(__linux__)
//#error "You are not using a cross-compiler, you will most certainly run into trouble"
//#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

#include "terminal.h"
#include "idt.h"
#include "gdt.h"
#include "timer.h"
#include "paging.h"
#include "kheap.h"
extern u32int placement_address;
extern u32int AFTER_BSS_END;
void kernel_main(void) {
  placement_address = (u32int) &AFTER_BSS_END;
  monitor_clear();
  prtf("Hello, kernel World!\n");
  prtf("%x\n", placement_address);
  init_gdt();
  init_idt();
//  asm volatile("sti");
//  init_timer(50);
//  monitor_clear();

//  initialise_paging();
  monitor_write("Hello, paging world!\n");

//  u32int *ptr = (u32int *) 0xA0000000;
//  u32int do_page_fault = *ptr;
//  (void)do_page_fault;

//  asm volatile("int $0x20");
//  asm volatile("int $0x4");



  u32int a = kmalloc(8);
  initialise_paging();
  u32int b = kmalloc(8);
  u32int c = kmalloc(8);
  monitor_write("a: ");
  monitor_write_hex(a);
  monitor_write(", b: ");
  monitor_write_hex(b);
  monitor_write("\nc: ");
  monitor_write_hex(c);

  kfree((void *) c);
  kfree((void *) b);
  u32int d = kmalloc(12);
  monitor_write(", d: ");
  monitor_write_hex(d);
}