#define _GNU_SOURCE

#include <sched.h>
#include <asm/hwprobe.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#define NR_RISCV_HWPROBE 258

// Merged in Linux v6.15.
// See 4458b8f68dc7 ("riscv: hwprobe: export Zicntr and Zihpm extensions").
#define RISCV_HWPROBE_EXT_ZICNTR (1ULL << 50)
#define RISCV_HWPROBE_EXT_ZIHPM (1ULL << 51)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])

struct ext_pair_t {
	char *id;
	uint64_t value;
};

static struct ext_pair_t ext_pairs[] = {
	{
		.id = "F, D",
		.value = RISCV_HWPROBE_IMA_FD,
	},
	{
		.id = "C",
		.value = RISCV_HWPROBE_IMA_C,
	},
	{
		.id = "V",
		.value = RISCV_HWPROBE_IMA_V,
	},
	{
		.id = "ZBA",
		.value = RISCV_HWPROBE_EXT_ZBA,
	},
	{
		.id = "ZBB",
		.value = RISCV_HWPROBE_EXT_ZBB,
	},
	{
		.id = "ZBS",
		.value = RISCV_HWPROBE_EXT_ZBS,
	},
	{
		.id = "ZICBOZ",
		.value = RISCV_HWPROBE_EXT_ZICBOZ,
	},
	{
		.id = "ZBC",
		.value = RISCV_HWPROBE_EXT_ZBC,
	},
	{
		.id = "ZBKB",
		.value = RISCV_HWPROBE_EXT_ZBKB,
	},
	{
		.id = "ZBKC",
		.value = RISCV_HWPROBE_EXT_ZBKC,
	},
	{
		.id = "ZBKX",
		.value = RISCV_HWPROBE_EXT_ZBKX,
	},
	{
		.id = "ZKND",
		.value = RISCV_HWPROBE_EXT_ZKND,
	},
	{
		.id = "ZKNE",
		.value = RISCV_HWPROBE_EXT_ZKNE,
	},
	{
		.id = "ZKNH",
		.value = RISCV_HWPROBE_EXT_ZKNH,
	},
	{
		.id = "ZKSED",
		.value = RISCV_HWPROBE_EXT_ZKSED,
	},
	{
		.id = "ZKSH",
		.value = RISCV_HWPROBE_EXT_ZKSH,
	},
	{
		.id = "ZKT",
		.value = RISCV_HWPROBE_EXT_ZKT,
	},
	{
		.id = "ZVBB",
		.value = RISCV_HWPROBE_EXT_ZVBB,
	},
	{
		.id = "ZVBC",
		.value = RISCV_HWPROBE_EXT_ZVBC,
	},
	{
		.id = "ZVKB",
		.value = RISCV_HWPROBE_EXT_ZVKB,
	},
	{
		.id = "ZVKG",
		.value = RISCV_HWPROBE_EXT_ZVKG,
	},
	{
		.id = "ZVKNED",
		.value = RISCV_HWPROBE_EXT_ZVKNED,
	},
	{
		.id = "ZVKNHA",
		.value = RISCV_HWPROBE_EXT_ZVKNHA,
	},
	{
		.id = "ZVKNHB",
		.value = RISCV_HWPROBE_EXT_ZVKNHB,
	},
	{
		.id = "ZVKSED",
		.value = RISCV_HWPROBE_EXT_ZVKSED,
	},
	{
		.id = "ZVKSH",
		.value = RISCV_HWPROBE_EXT_ZVKSH,
	},
	{
		.id = "ZVKT",
		.value = RISCV_HWPROBE_EXT_ZVKT,
	},
	{
		.id = "ZFH",
		.value = RISCV_HWPROBE_EXT_ZFH,
	},
	{
		.id = "ZFHMIN",
		.value = RISCV_HWPROBE_EXT_ZFHMIN,
	},
	{
		.id = "ZIHINTNTL",
		.value = RISCV_HWPROBE_EXT_ZIHINTNTL,
	},
	{
		.id = "ZVFH",
		.value = RISCV_HWPROBE_EXT_ZVFH,
	},
	{
		.id = "ZVFHMIN",
		.value = RISCV_HWPROBE_EXT_ZVFHMIN,
	},
	{
		.id = "ZFA",
		.value = RISCV_HWPROBE_EXT_ZFA,
	},
	{
		.id = "ZTSO",
		.value = RISCV_HWPROBE_EXT_ZTSO,
	},
	{
		.id = "ZACAS",
		.value = RISCV_HWPROBE_EXT_ZACAS,
	},
	{
		.id = "ZICOND",
		.value = RISCV_HWPROBE_EXT_ZICOND,
	},
	{
		.id = "ZIHINTPAUSE",
		.value = RISCV_HWPROBE_EXT_ZIHINTPAUSE,
	},
	{
		.id = "ZVE32X",
		.value = RISCV_HWPROBE_EXT_ZVE32X,
	},
	{
		.id = "ZVE32F",
		.value = RISCV_HWPROBE_EXT_ZVE32F,
	},
	{
		.id = "ZVE64X",
		.value = RISCV_HWPROBE_EXT_ZVE64X,
	},
	{
		.id = "ZVE64F",
		.value = RISCV_HWPROBE_EXT_ZVE64F,
	},
	{
		.id = "ZVE64D",
		.value = RISCV_HWPROBE_EXT_ZVE64D,
	},
	{
		.id = "ZIMOP",
		.value = RISCV_HWPROBE_EXT_ZIMOP,
	},
	{
		.id = "ZCA",
		.value = RISCV_HWPROBE_EXT_ZCA,
	},
	{
		.id = "ZCB",
		.value = RISCV_HWPROBE_EXT_ZCB,
	},
	{
		.id = "ZCD",
		.value = RISCV_HWPROBE_EXT_ZCD,
	},
	{
		.id = "ZCF",
		.value = RISCV_HWPROBE_EXT_ZCF,
	},
	{
		.id = "ZCMOP",
		.value = RISCV_HWPROBE_EXT_ZCMOP,
	},
	{
		.id = "ZAWRS",
		.value = RISCV_HWPROBE_EXT_ZAWRS,
	},
	{
		.id = "ZICNTR",
		.value = RISCV_HWPROBE_EXT_ZICNTR,
	},
	{
		.id = "ZIHPM",
		.value = RISCV_HWPROBE_EXT_ZIHPM,
	},
};

void print_bits(size_t size, void *ptr)
{
	unsigned char *b = (unsigned char *)ptr;
	unsigned char byte;
	int i, j;

	for (i = size - 1; i >= 0; i--) {
		for (j = 7; j >= 0; j--) {
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
	}
}

int main()
{
	struct riscv_hwprobe pairs;
	long rc;

	// base: rv64ima

	pairs = (struct riscv_hwprobe){
		.key = RISCV_HWPROBE_KEY_BASE_BEHAVIOR,
	};
	rc = syscall(NR_RISCV_HWPROBE, &pairs, 1, 0, NULL, 0);
	if (rc) {
		perror("could not fetch the base behavior for the system");
		exit(EXIT_FAILURE);
	}
	printf("Supports base extensions I, M, A? %s\n", (bool)pairs.value ? "yes" : "no");

	// Extensions

	pairs = (struct riscv_hwprobe){
		.key = RISCV_HWPROBE_KEY_IMA_EXT_0,
	};
	rc = syscall(NR_RISCV_HWPROBE, &pairs, 1, 0, NULL, 0);
	if (rc) {
		perror("could not fetch extensions as given by hwprobe");
		exit(EXIT_FAILURE);
	}

	printf("\nWe got back this bit map: ");
	print_bits(sizeof(long), &pairs.value);

	puts("; which means:\n");
	for (size_t i = 0; i < ARRAY_SIZE(ext_pairs); i++) {
		if ((ext_pairs[i].value & pairs.value) == ext_pairs[i].value) {
			printf("%s: OK\n", ext_pairs[i].id);
		}
	}

	exit(EXIT_SUCCESS);
}
