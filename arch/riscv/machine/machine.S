.section .text

# Returns the physical address that was identified to be for the serial port. If
# any error was found then it just returns 0. This function accepts one argument
# which is a pointer to the FDT structure as given by the firmware. You can
# consider this function to have this equivalent C signature:
#
#   uint64_t get_serial_address(void *fdt);
#
.type get_serial_address, @function
get_serial_address:
	# Leave early if the caller passed a NULL pointer.
	beqz a0, .parse_error

	# First loop. This one simply iterates over the FDT structure in search for
    # 'serial@', which marks the beginning of the address definition.
.search_serial_loop_reset:
	la t1, .serial
.search_serial_loop:
	lbu t2, (t1)
	beqz t2, .parse_hex

	lbu t3, (a0)
	addi a0, a0, 1
	bne t2, t3, .search_serial_loop_reset
	addi t1, t1, 1
	j .search_serial_loop

	# Second loop. On each digit shift left one nibble and add the character
    # converted to an integer. If the character cannot be converted, then we go
    # to `parse_error` which sets the return value to 0.
.parse_hex:
	mv t1, zero
	li t2, 10
	li t3, 0x30	# Character '0'

.parse_hex_loop:
	# The FDT specification guarantees that strings are NULL-terminated. Thus,
    # whenever we find the NULL character, then we are done parsing.
	lbu t0, (a0)
	beqz t0, .get_serial_address_end

	# Trying to parse a numeric character. Note that this parser is grossly
    # uncapable of handling Aa-Ff hexadecimal values, and only knows numeric
    # digits. This can be improved but I did not have an example to work
	# on this case.
	sub t0, t0, t3
	bltz t0, .parse_error
	blt t2, t0, .parse_error

	# Shift one nibble on the accumulator and add the computed value to it.
	sll t1, t1, 4
	add t1, t1, t0

	# Next iteration.
	addi a0, a0, 1
	j .parse_hex_loop

.parse_error:
	mv t1, zero
.get_serial_address_end:
	mv a0, t1
	ret

# Send the `message` to the given `address` so it's printed there. You can
# consider this function to have this equivalent C signature:
#
#   void printm(uint64_t address, char *message);
#
.type printm, @function
printm:
	mv t1, a1

.printm_loop:
	lbu t0, (t1)
	beqz t0, .printm_end

	sb t0, (a0)
	addi t1, t1, 1
	j .printm_loop

.printm_end:
	ret

.section .text.bios

# The entry point: the firmware will blindly jump here. We expect the firmware
# to pass up two arguments, the first one being the hart ID, and the other being
# a pointer to the FDT describing this machine. You can consider this function
# to have this equivalent C signature:
#
#   void _start(uint64_t hart_id, void *fdt) __attribute__((noreturn));
#
.global _start
.type _start, @function
_start:
	# We expect the configuration to be given as a devicetree blob pointed by
	# the second argument (a1). Hence, the configuration pointer from RISC-V
	# should be set to NULL indicating that. If that's not the case, then it's
	# not a supported scenario and we jump to the infinite loop gracefully.
	csrr t0, mconfigptr
	bnez t0, .end

	# We don't care about the hard ID, but we need the FDT pointer as an
    # argument for the `get_serial_address` function.
	mv a0, a1
	call get_serial_address

	# If the return value of `get_serial_address` is a NULL value, then
    # something went wrong there and we just have to end it here.
	beqz a0, .end

	# And now we can print our message.
	la a1, .msg1
	call printm

	# Infinite loop so not to crash :)
.end:
	j .end

.section .rodata
.msg1:
	.string "Hello, world!\n"
.serial:
	.string "serial@"
