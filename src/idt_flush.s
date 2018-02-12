/*

[global idt_flush]
idt_flush:
	mov eax, [esp + 4]
	lidt [eax]			;LIDT m16&32 
	ret
*/

.global idt_flush
idt_flush:
    movl 4(%esp), %eax
    lidt (%eax)
    ret
