/*[extern isr_handler]
isr_common_stub:
	pusha
	mov ax, ds
	push eax
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	call isr_handler
	
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa
	add esp, 8
	sti
	iret
	*/
isr_common_stub:
	pusha
	movl %ds, %eax
	pushl %eax

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	call isr_handler

	popl %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	popa
	addl $8, %esp
	sti
	iret
	/*
;push byte 
;push word 
;push 
;反汇编之后是一样的
;错误码是32位的
;
;为什么regsiter_t里ino_no，err_code用32位是对的呢
;因为数据段b置位 栈操作以32位位单位
*/
.macro ISR_COM one
	pushl $\one#;写byte 或者不写都行
	jmp isr_common_stub
.endm

.macro ISR_NOERR one
.global isr\one
isr\one:
	cli
	#;push dword 0x12345678;打印的error code :305419896
	#;push byte 0x12345678;截断成0x78 error是120
	pushl $0
	ISR_COM \one
.endm

.macro ISR_ERR one
.global isr\one
isr\one:
	cli 
	ISR_COM \one
.endm

.macro IRQ one ,two
.global irq\one
irq\one:
	cli 
	pushl $0#;占个位置 irq没错误码的
	pushl $\two
	jmp irq_common_stub
.endm

irq_common_stub:
	pusha
	movl %ds, %eax
	pushl %eax
	
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	
	call irq_handler
	
	popl %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	
	popa
	
	addl $8, %esp
	sti
	iret
	

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31


IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47
