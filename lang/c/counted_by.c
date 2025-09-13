#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct foo {
	uint64_t a;
	uint64_t b;
};

struct bounded_flex_t {
	uint64_t count;

	// NOTE: try to move this member somewhere else and you will get:
	//   counted_by.c:13:20: error: flexible array member not at end of struct
	struct foo flex_array[] __attribute__((counted_by(count)));
};

void free_flex(struct bounded_flex_t **flex)
{
	struct bounded_flex_t *ptr = *flex;

	printf("Freeing a flexible array of %ld members.\n", ptr->count);
	free(*flex);
}

__attribute__((noreturn)) void die(const char *const str)
{
	fprintf(stderr, str);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	uint64_t len = 1;

	if (argc == 2) {
		len = (uint64_t) strtol(argv[1], NULL, 10);
		if (!len) {
			die("error: bad integer value!\n");
		}
	} else if (argc > 2) {
		die("error: only pass one argument maximum!\n");
	}

	// That's a mouthful :P
	struct bounded_flex_t *flex __attribute__((cleanup(free_flex))) = malloc(sizeof(struct bounded_flex_t) + (len * sizeof(struct foo)));
	if (!flex) {
		die("error: could not allocate memory for it");
	}

	// NOTE: if this is not the first thing that we do, and we use 'len' for the
	// for loop below, then we will get a runtime error from the array bound
	// checker. Hence, pretty much like in Rust :)
	flex->count = len;

	uint64_t accumulator = 0;
	for (uint64_t i = 0; i < flex->count; i++) {
		flex->flex_array[i] = (struct foo){ .a = accumulator, .b = accumulator + 1};
		accumulator++;
	}

	for (uint64_t i = 0; i < flex->count; i++) {
		printf("%ld: .a = %ld, .b = %ld\n", i, flex->flex_array[i].a, flex->flex_array[i].b);
	}
}
