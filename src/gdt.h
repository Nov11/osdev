#ifndef GDT_H
#define GDT_H
#include "common.h"
#define LIMIT_MAX 0xffffffff
#define GRAN_G	0x80
#define GRAN_D  0x40
#define GRAN_B	0x40
#define ACCS_P	0x80
#define DPL_0   0x00
#define DPL_1   0x20
#define DPL_2   0x40
#define DPL_3   0x60
//granularity
//bits :7 6   5  4   3 2 1 0
//      g b/d 0  avl limit bit range 19 ~ 16
//g:granularity 1:add 12lower 1
//b,d:   代码段就称之为 D位。D是一个很特殊的位，在描述可执行段、向下扩展数据段或由SS寄存器寻址的段(通常是堆栈段)的三种描述符中的意义各不相同。 
//;     ⑴ 在描述可执行段的描述符中，D位决定了指令使用的地址及操作数所默认的大小。
//;		① D=1表示默认情况下指令使用32位地址及32位或8位操作数，这样的代码段也称为32位代码段；
//;		② D=0 表示默认情况下，使用16位地址及16位或8位操作数，这样的代码段也称为16位代码段，它与80286兼容。可以使用地址大小前缀和操作数大小前缀分别改变默认的地址或操作数的大小。 
//		数据段叫B位好了
//;     ⑵ 在向下扩展数据段的描述符中(也就是数据段type里的E置位)，B位决定段的上部边界。
//;		① B=1表示段的上部界限为4G；
//;		② B=0表示段的上部界限为64K，这是为了与80286兼容。 
//;     ⑶ 在描述由SS寄存器寻址的段描述符中，D其实这个时候一般称之为B位，数据段专用，D给代码段用。
//			这个位决定隐式的堆栈访问指令(如PUSH和POP指令)使用何种堆栈指针寄存器。
//;		① B=1表示使用32位堆栈指针寄存器ESP；
//;		② B=0表示使用16位堆栈指针寄存器SP，这与80286兼容。 
//access
//bits:7 65  4  3 2 1 0
//     p dpl dt type
//p:present
//dpl ring0 ~ 3
//dt system descriptor or data|code descriptor
//
// (4) TYPE: 说明存储段描述符所描述的存储段的具体属性。
//
//		 
//	数据段类型	类型值		说明
//			----------------------------------
//			0		只读 
//			1		只读、已访问 
//			2		读/写 
//			3		读/写、已访问 
//			4		只读、向下扩展 
//			5		只读、向下扩展、已访问 
//			6		读/写、向下扩展 
//			7		读/写、向下扩展、已访问 
//	简单点说type 3 2 1 0
//				 0 e w a   e:下扩 跟下扩栈是同一个下扩
//
//		
//			类型值		说明
//	代码段类型	----------------------------------
//			8		只执行 
//			9		只执行、已访问 
//			A		执行/读 
//			B		执行/读、已访问 
//			C		只执行、一致码段 
//			D		只执行、一致码段、已访问 
//			E		执行/读、一致码段 
//			F		执行/读、一致码段、已访问 
//  简单点说type 3 2 1 0
//  			 1 c r a c是一致性代码的意思
//
//		
//	系统段类型	类型编码	说明
//			----------------------------------
//			0		<未定义>
//			1		可用286TSS
//			2		LDT
//			3		忙的286TSS
//			4		286调用门
//			5		任务门
//			6		286中断门
//			7		286陷阱门
//			8		未定义
//			9		可用386TSS
//			A		<未定义>
//			B		忙的386TSS
//			C		386调用门
//			D		<未定义>
//			E		386中断门
//			F		386陷阱门

struct gdt_entry_struct{
	u16int limit_low;
	u16int base_low;
	u8int base_mid;
	u8int access;
	u8int granularity;
	u8int base_high;
}__attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct{
	u16int limit;
	u32int base;
}__attribute((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

void init_gdt();
#endif
