.global _start
.section .head.text

_start:
	// Start by simply disabling interrupts.
	csrw sie, zero
	csrw sip, zero
	fence.i

	// Initialize the 'a4' register, which is going to be used for printing out
	// the end message by the main hart.
	li a4, 0

	// Hart lottery! The main hart will win it, the rest will simply jump into
	// .Lhalt. See hsm.c for more details.
	la a3, hart_lottery
	li a2, 1
	amoadd.w a3, a2, (a3)
	bnez a3, .Lhalt

	// Setup a dummy stack and call the kernel.
	la sp, _STACK_PTR
	call start_kernel

	// We actually expect the main kernel routine to quit. Here we make the
	// .Lhalt part of the code to print special messages for that case.
	addi a4, a4, 1

.Lhalt:
	// Increase the number of hart that lost the lottery.
	la a3, hart_failed_lottery
	li a2, 1
	amoadd.w zero, a2, (a3)

	// If it's the main hart that is bailing out, first print some final
	// messages.
	beqz a4, .Lend
	call print_lottery
	call end

	// Die Bart, die!
.Lend:
	wfi
	j .Lend
