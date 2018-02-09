#include "kheap.h"

extern u32int end;//bss 结束的位置 把从这里开始的内存当成堆
u32int placement_address = (u32int)&end;

u32int kmalloc_int(u32int sz, u8int align, u32int* phys)
{
  //if(align == 1 && (placement_address & 0xfffff000)){//低于4K的不管?
  if(align == 1){
    placement_address &= 0xfffff000;
    placement_address += 0x1000;
  }
  if(phys){
    *phys = placement_address;
  }
  u32int ret = placement_address;
  placement_address += sz;

  return ret;
}

u32int kmalloc_a(u32int sz)
{
  return kmalloc_int(sz, 1, 0);
}

u32int kmalloc_p(u32int sz, u32int* phys)
{
  return kmalloc_int(sz, 0, phys);
}

u32int kmalloc_ap(u32int sz, u32int* phys)
{
  return kmalloc_int(sz, 1, phys);
}

u32int kmalloc(u32int sz)
{
  return kmalloc_int(sz, 0, 0);
}