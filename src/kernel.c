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
#include "initrd.h"
#include "multiboot.h"
extern u32int placement_address;
extern u32int AFTER_BSS_END;
extern u32int _binary_initrd_img_start;
void kernel_main(unsigned long magic, multiboot_info_t* mboot_ptr) {
  ASSERT(magic == 0x2BADB002);
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


//  u32int initrd_location = *((u32int*)mboot_ptr->mods_addr);
  fs_root = initialise_initrd((u32int)&_binary_initrd_img_start);
  // list the contents of /
  int i = 0;
  struct dirent *node = 0;
  while ( (node = readdir_fs(fs_root, i)) != 0)
  {
    monitor_write("Found file ");
    monitor_write(node->name);
    fs_node_t *fsnode = finddir_fs(fs_root, node->name);

    if ((fsnode->flags&0x7) == FS_DIRECTORY)
    {
      monitor_write("\n\t(directory)\n");
    }
    else
    {
      monitor_write("\n\t contents: \"");
      char buf[256];
      u32int sz = read_fs(fsnode, 0, 256, (u8int*)buf);
      int j;
      for (j = 0; j < sz; j++)
        monitor_put(buf[j]);

      monitor_write("\"\n");
    }
    i++;
  }
}