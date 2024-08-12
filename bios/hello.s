# From: https://github.com/qemu/qemu/blob/23fa74974d8c96bc95cbecc0d4e2d90f984939f6/hw/riscv/virt.c#L82.
.set QEMU_UART0_ADDRESS, 0x10000000

.global _start
.section .text.bios

_start:	li a0, 0x68
	li a1, QEMU_UART0_ADDRESS
	sb a0, (a1) # 'h'

	li a0, 0x65
	sb a0, (a1) # 'e'

	li a0, 0x6C
	sb a0, (a1) # 'l'

	li a0, 0x6C
	sb a0, (a1) # 'l'

	li a0, 0x6F
	sb a0, (a1) # 'o'

loop:	j loop
