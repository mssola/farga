.text

.globl reverse_string
.type reverse_string, @function

// char * reverse_string(char *str);
reverse_string:
    // Return early on null pointer.
    beq a0, zero, end

    // Preserve the original pointer.
    addi sp, sp, -8
    sd a0, 0(sp)

    // Set `t0` to point to the end of the string.
    add t0, a0, zero
set_end_ptr:
    lbu t2, 0(t0)
    beq t2, zero, end_ptr_done
    addi t0, t0, 1
    j set_end_ptr

    // We are done iterating, if `a0` and `t0` are equal, then there's nothing
    // to be done and we can return early. Otherwise decrement `t0` so it points
    // to the byte right before the null termination.
end_ptr_done:
    beq a0, t0, reverse_done
    addi t0, t0, -1

reverse_loop:
    // Swap values between the two pointers.
    lb t1, 0(a0)
    lb t2, 0(t0)
    sb t1, 0(t0)
    sb t2, 0(a0)

    // Move pointers and check whether the pointers have already crossed. If
    // they have not crossed yet there is still looping to be done. Otherwise
    // we are done.
    addi a0, a0, 1
    addi t0, t0, -1
    bltu a0, t0, reverse_loop

reverse_done:
    // Restore things back and return to the caller.
    ld a0, 0(sp)
    addi sp, sp, 8
end:
    jr ra

.globl is_palindrome
.type is_palindrome, @function

// bool is_palindrome(char *str);
is_palindrome:
    // Return early on null pointer.
    beq a0, zero, palindrome_no

    // Set `t0` to point to the end of the string.
    add t0, a0, zero
pal_set_end_ptr:
    lbu t2, 0(t0)
    beq t2, zero, pal_end_ptr_done
    addi t0, t0, 1
    j pal_set_end_ptr

    // We are done iterating, if `a0` and `t0` are equal, then there's nothing
    // to be done and we can return early. Otherwise decrement `t0` so it points
    // to the byte right before the null termination.
pal_end_ptr_done:
    beq a0, t0, palindrome_no
    addi t0, t0, -1

pal_loop:
    // Swap values between the two pointers.
    lb t1, 0(a0)
    lb t2, 0(t0)
    bne t1, t2, palindrome_no

    // Move pointers and check whether the pointers have already crossed. If
    // they have not crossed yet there is still looping to be done. Otherwise
    // we are done.
    addi a0, a0, 1
    addi t0, t0, -1
    bleu a0, t0, pal_loop

palindrome_yes:
    li a0, 1
    jr ra

palindrome_no:
    li a0, 0
    jr ra

.globl mstrcmp
.type mstrcmp, @function

// int mstrcmp(const char *, const char *);
mstrcmp:
1:
	lbu t0, 0(a0)
	lbu t1, 0(a1)
	bne t0, t1, 2f
	addi a0, a0, 1
	addi a1, a1, 1
	bnez t0, 1b
	li a0, 0
	ret
2:
	sub a0, t0, t1
	ret
