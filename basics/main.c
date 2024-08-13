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

void test_factorial()
{
	assert(factorial(0) == 0);
	assert(factorial(1) == 1);
	assert(factorial(2) == 2);
	assert(factorial(3) == 6);
	assert(factorial(4) == 24);

	printf("factorial:\tOK\n");
}

/*
 * Returns a pointer with the same given string but reversed. Note that the
 * string cannot be in read-only space since the reversal is done in-place.
 *
 * Implemented in string.S
 */
char * reverse_string(char *str);

void test_reverse_string()
{
	assert(reverse_string(NULL) == NULL);
	assert(reverse_string("") == "");

	char s1[] = "This is a string.";
	assert(strcmp(reverse_string(s1), ".gnirts a si sihT") == 0);

	char s2[] = ".";
	assert(strcmp(reverse_string(s2), ".") == 0);

	printf("reverse_string:\tOK\n");
}

/*
 * Returns true if the given string is a palindrome, false otherwise.
 *
 * Implemented in string.S
 */
bool is_palindrome(char *str);

void test_is_palindrome()
{
	assert(is_palindrome(NULL) == 0);
	assert(is_palindrome("") == 0);
	assert(is_palindrome("aba") == 1);
	assert(is_palindrome("aaa") == 1);
	assert(is_palindrome("This is a a si sihT") == 1);

	printf("is_palindrome:\tOK\n");
}

int main()
{
	test_factorial();
	test_reverse_string();
	test_is_palindrome();
}
