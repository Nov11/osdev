/*
[global gdt_flush]

gdt_flush:
	mov eax, [esp + 4]  ;get param
	;lgdt要个内存参数 就是说类型LGDT m16&32 
	;eax内容是gdt_ptr地址
	;[eax]是个访存参数
	lgdt [eax]			;load gdt pointer to gdtr 

	mov ax, 0x10		;0x10 : 0: first desc 0x8 2nd 0x10 3rd desc
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:.flush		;jmp after set gdtr 0x8 goes to cs
.flush:
	ret
*/

.global gdt_flush

gdt_flush:
	movl 4(%esp), %eax
/*	;lgdt要个内存参数 就是说类型LGDT m16&32
	;eax内容是gdt_ptr地址
	;[eax]是个访存参数
	*/
	lgdt (%eax)			#load gdt pointer to gdtr

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ljmp $0x8, $gdt_flush_label		#jmp after set gdtr 0x8 goes to cs
gdt_flush_label:
	ret
