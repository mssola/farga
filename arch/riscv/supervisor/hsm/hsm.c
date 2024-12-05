#include "compiler.h"
#include "config.h"
#include "sbi.h"

// Implemented in head.S. Here we need the symbol in order to pass the address
// of the function when starting secondary harts.
extern void _start(void);

/*
 * Similar to the Linux kernel and what I did in https://github.com/mssola/fbos,
 * harts will enter in a lottery when starting. See the README.md file for more
 * details on this.
 */
atomic_t hart_lottery __attribute__((__section__(".sdata")));
atomic_t hart_failed_lottery __attribute__((__section__(".sdata")));

// Fake a delay so it's easier on the eyes.
#define SLEEP()                            \
	for (int i = 0; i < 1000000000; i++) { \
	}

/*
 * Printing utilities.
 */

void printk(const char *const message);

void write(const char *const message, size_t n)
{
	struct sbi_ret ret = sbi_ecall2(DBCN_EXT, DBCN_WRITE, n, (unsigned long)message);
	if (ret.error != SBI_SUCCESS) {
		for (;;) {
			;
		}
	}
}

size_t strlen(const char *const message)
{
	size_t i = 0;

	for (; message[i] != '\0'; i++)
		;

	return i;
}

__attribute__((__noreturn__)) void die(const char *const message)
{
	if (message) {
		printk(message);
	}

	for (;;)
		;
}

void print_digit(uint64_t digit)
{
	char buffer[2];

	if (digit > 9) {
		die("We cannot print numbers with two or more digits :D\n");
	}

	buffer[0] = '0' + digit;
	buffer[1] = '\0';
	write(buffer, 2);
}

void printk(const char *const message)
{
	size_t len = strlen(message);
	if (!len) {
		return;
	}

	write(message, len);
}

/*
 * Printing/asserting the stats of the lottery.
 */

void assert_lottery(uint64_t participated, uint64_t failed)
{
	if (hart_lottery.value != participated) {
		die("Bad number for participating harts\n");
	}
	if (hart_failed_lottery.value != failed) {
		die("Bad number for failed harts\n");
	}
}

void print_lottery(void)
{
	printk("How many harts have _participated_ in the lottery? ");
	print_digit(hart_lottery.value);
	write("\n", 1);

	printk("How many harts have _failed_ in the lottery? ");
	print_digit(hart_failed_lottery.value);
	write("\n", 1);
}

// Called at the very end of the program. It will shutdown the machine.
void end(void)
{
	printk("\nTHE END\n");

	// SBI call to the SRST extension to gracefully shutdown the machine.
	sbi_ecall2(SRST_EXT, SRST_RESET, SRST_SHUTDOWN, SRST_NO_REASON);
}

void start_kernel(uint64_t hart_id, uintptr_t opaque)
{
	printk("Hello, world!\n");

	/*
	 * Figure out the harts that are meant to be stopped for now.
	 */

	unsigned int other_harts[NR_CPUS - 1];
	struct sbi_ret ret;

	printk("Hart that won the lottery: ");
	print_digit(hart_id);
	write("\n", 1);

	printk("Hart still asleep: ");
	for (unsigned int i = 0, j = 0; i < NR_CPUS; i++) {
		if (i != hart_id) {
			print_digit(i);
			if (j + 1 < NR_CPUS - 1) {
				write(", ", 2);
			}
			other_harts[j++] = i;
		}
	}
	write("\n", 1);

	/*
	 * ASSERT: all harts in 'other_harts' are marked as stopped by HSM.
	 */

	for (unsigned int i = 0; i < NR_CPUS - 1; i++) {
		ret = sbi_ecall1(HSM_EXT, HSM_HART_GET_STATUS, other_harts[i]);

		if (ret.error != SBI_SUCCESS) {
			die("HSM_HART_GET_STATUS: unexpected error from SBI\n");
		}
		if (ret.value != HSM_HART_STOPPED) {
			die("HSM_HART_GET_STATUS: hart that was supposed to be stopped is not!\n");
		}
	}

	// Initial state of the lottery.
	print_lottery();
	assert_lottery(1, 0);
	write("\n", 1);

	/*
	 * Wake up the first hart.
	 */

	printk("Waking up first secondary hart\n");

	ret = sbi_ecall3(HSM_EXT, HSM_HART_START, other_harts[0], (uintptr_t)&_start, opaque);
	if (ret.error != SBI_SUCCESS) {
		die("HSM_HART_START: unexpected error from SBI\n");
	}

	SLEEP();

	ret = sbi_ecall1(HSM_EXT, HSM_HART_GET_STATUS, other_harts[0]);
	if (ret.error != SBI_SUCCESS) {
		die("HSM_HART_GET_STATUS: unexpected error from SBI\n");
	}
	if (ret.value != HSM_HART_STARTED) {
		die("HSM_HART_GET_STATUS: hart that was supposed to be started is not!\n");
	}

	// Another hart should have entered the lottery and failed.
	print_lottery();
	assert_lottery(2, 1);
	write("\n", 1);

	/*
	 * Now wake up the rest.
	 */

	printk("Waking up the rest\n");

	for (unsigned int i = 1; i < NR_CPUS - 1; i++) {
		ret = sbi_ecall3(HSM_EXT, HSM_HART_START, other_harts[i], (uintptr_t)&_start, opaque);
		if (ret.error != SBI_SUCCESS) {
			die("HSM_HART_START: unexpected error from SBI\n");
		}
	}

	SLEEP();

	// All awake, only one good.
	print_lottery();
	assert_lottery(NR_CPUS, NR_CPUS - 1);
	write("\n", 1);

	/*
	 * Kernels usually don't quit, but let's do it for the heck of it; head.S
	 * will handle it.
	 */

	printk("Goodbye, cruel world!\n");
	SLEEP();
}
