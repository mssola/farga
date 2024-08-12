.text
.globl factorial
.type factorial, @function

factorial:
	// If input <= 1, just return the given thing.
	addi t0, zero, 1
	bgt a0, t0, else
	jr ra
else:
	// Preserve the argument and the return address before the call.
	addi sp, sp, -16
	sd a0, 0(sp)
	sd ra, 8(sp)

	// Call the same function but with a value of argument-1.
	addi a0, a0, -1
	call factorial

	// Restore back the original argument and the return address. Then multiply
	// the original argument with the returned one from the previous call.
	ld t1, 0(sp)
	ld ra, 8(sp)
	addi sp, sp, 16
	mul a0, t1, a0

	// And return to the caller.
	jr ra
