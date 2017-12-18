#include "gdt.h"

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;
extern void gdt_flush(u32int);
void gdt_set_gate(s32int index, u32int base, u32int limit, u8int access, u8int gran);

void gdt_set_gate(s32int index, u32int base, u32int limit, u8int access, u8int gran)
{
	gdt_entries[index].base_low = base & 0xffff;//16bits
	gdt_entries[index].base_mid = base >> 16 & 0xff;//8bits
	gdt_entries[index].base_high = base >> 24 & 0xff;//8bits

	gdt_entries[index].limit_low = limit & 0xffff;//16bits
	gdt_entries[index].granularity = (limit >> 16 & 0x0f) | (gran & 0xf0); 
	//limit >> 16 | gran & 0xf0在bochs没事儿 qemu里就完蛋了
	//limit 应该只影响低4位 传进来的参数是全f 参与或运算的是0xff 
	//哎 这个bug耗了半个晚上才找出来
	//gdt_entries[index].granularity = (limit >> 16) & 0x0F;
    //gdt_entries[index].granularity |= gran & 0xF0;
	gdt_entries[index].access = access;
}


void init_gdt()
{
	gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base = (u32int)gdt_entries;//&gdt_entries is the addr of array

	gdt_set_gate(0, 0, 0, 0, 0);
	//acc 7	6	5	4	3		2	1	0 for data and code seg
	//    p [dpl]   1   0:data  e   w   a
	//                  1:code  c   r   a
	//    1  0  0   1   1       0   1   0  0x9a code
	//    1  0  0   1   0       0   1   0  0x92 data
	gdt_set_gate(1, 0, LIMIT_MAX, 0x9a, 0xc0);//0x9a : 1001 1010
   	gdt_set_gate(2, 0, LIMIT_MAX, 0x92, 0xc0);//0x92 : 1001 0010
	gdt_set_gate(3, 0, LIMIT_MAX, 0xfa, 0xc0);
	gdt_set_gate(4, 0, LIMIT_MAX, 0xf2, 0xc0);
	
	gdt_flush((u32int)&gdt_ptr);
}

/*
void gdt_set_gate(s32int num, u32int base, u32int limit, u8int access, u8int gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_mid = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base  = (u32int)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    gdt_flush((u32int)&gdt_ptr);
}
*/
