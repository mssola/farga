#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 * Returns the factorial for the given unsigned 64-bit integer.
 *
 * Implemented in factorial.S
 */
int factorial(uint64_t n);

void test_factorial(void)
{
	assert(factorial(0) == 0);
	assert(factorial(1) == 1);
	assert(factorial(2) == 2);
	assert(factorial(3) == 6);
	assert(factorial(4) == 24);

	printf("factorial:\t\tOK\n");
}

/*
 * Returns a pointer with the same given string but reversed. Note that the
 * string cannot be in read-only space since the reversal is done in-place.
 *
 * Implemented in string.S
 */
char * reverse_string(char *str);

void test_reverse_string(void)
{
	assert(reverse_string(NULL) == NULL);
	assert(reverse_string("") == "");

	char s1[] = "This is a string.";
	assert(strcmp(reverse_string(s1), ".gnirts a si sihT") == 0);

	char s2[] = ".";
	assert(strcmp(reverse_string(s2), ".") == 0);

	printf("reverse_string:\t\tOK\n");
}

/*
 * Returns true if the given string is a palindrome, false otherwise.
 *
 * Implemented in string.S
 */
bool is_palindrome(char *str);

void test_is_palindrome(void)
{
	assert(is_palindrome(NULL) == 0);
	assert(is_palindrome("") == 0);
	assert(is_palindrome("aba") == 1);
	assert(is_palindrome("aaa") == 1);
	assert(is_palindrome("This is a a si sihT") == 1);

	printf("is_palindrome:\t\tOK\n");
}

/*
 * This function computes the following: ((a + b) * b) + b. Don't try to make
 * sense of it, that's the computation I ended up while messing with RISC-V
 * floating point instructions. Anyways, with this in mind, it will return if
 * the computed value is greater than 10.
 *
 * Implemented in float.S
 */
bool greater_than_ten(double a, uint64_t b);

void test_greater_than_ten(void)
{
	assert(!greater_than_ten(2.3, 1)); // 4.3
	assert(greater_than_ten(2.3, 2));  // 10.6

	printf("greater_than_ten:\tOK\n");
}

/*
 * Atomically add the integer pointed by `a` with the given `value`. Although
 * this is a function, I've checked that the assembly produced by GCC on a
 * decent optimization level actually inlines this.
 */
static inline void atomic_add(uint64_t *a, uint64_t value)
{
	/*
	 * The execution is a mere `amoadd` instruction, but it will set on `t0` the
	 * result. If it fails (e.g. the address was already being used), then `t0 =
	 * 0` and `beqz` will instruct it to try again.
	 *
	 * As for the 'A' RISC-V specific constraint, it means "An address that is
	 * held in a general-purpose register". GCC will then do the magic and
	 * convert it to `amoadd.d t0, a1, (a0)` or something like that. Note that
	 * we have to pass the '+' constraint modifier because the address will be
	 * both read and written atomically.
	 *
	 * See: https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html.
	 */
    asm volatile(".Latomic_retry:\n\t"
				 "amoadd.d t0, %1, %0\n\t"
				 "beqz t0, .Latomic_retry"
				 : "+A" (*a)
				 : "r" (value)
				 : "memory");
}

void test_atomic_add(void)
{
	uint64_t i = 4;

	atomic_add(&i, 0);
	assert(i == 4);

	atomic_add(&i, 2);
	assert(i == 6);

	printf("atomic_add:\t\tOK\n");
}

int main()
{
	test_factorial();
	test_reverse_string();
	test_is_palindrome();
	test_greater_than_ten();
	test_atomic_add();
}
