#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Needed for 'getauxval'.
#include <sys/auxv.h>

/*
 * In projects like github.com/mssola/fbos I already dealt with ELF parsing
 * myself and it wasn't the nicest experience. Plus, in here I have to do some
 * heave usage of some ELF sections. For this reason I will not do things
 * manually and use the standard library instead.
 */
#include <elf.h>

// Terminate the current program with a failure exit code while also printing
// the given `message` into stderr.
__attribute__((noreturn)) void die(const char *const message)
{
	fprintf(stderr, message);
	exit(EXIT_FAILURE);
}

// Assert that the given address 'addr' points to a 64-bit little-endian ELF
// file.
void ensure_proper_elf_header(const char *const addr)
{
	if (addr[0] != 0x7F || memcmp(&addr[1], "ELF", 3) != 0) {
		die("Bad ELF format\n");
	}
	if (addr[4] != 2) {
		die("64-bit format is mandatory\n");
	}
	if (addr[5] != 1) {
		die("Little-endian only\n");
	}
}

// Returns true if the given name looks like a '__vdso_' function name.
bool is_vdso_function(const char *const name)
{
	if (strlen(name) < 9) {
		return false;
	}

	return name[0] == '_' && name[1] == '_' && name[2] == 'v' && name[3] == 'd' && name[4] == 's' &&
		   name[5] == 'o' && name[6] == '_';
}

int main(void)
{
	/*
	 * From the vDSO man page: "The base address of the vDSO (if one exists) is
	 * passed by the kernel to each program in the initial auxiliary vector (see
	 * getauxval(3)), via the AT_SYSINFO_EHDR tag".
	 */
	void *vdso = (void *)getauxval(AT_SYSINFO_EHDR);
	if (!vdso) {
		die("Could not get base address for vDSO\n");
	}
	printf("=> vDSO base address: %pS\n", vdso);

	// Basic assertion :)
	ensure_proper_elf_header((const char *)vdso);

	// The ELF header is the first thing starting from the fetched address.
	const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *)vdso;

	/*
	 * Grab the base address for the Section Header Table for the ELF
	 * binary. From there, we will search section and fetch the address for the
	 * dynamic symbol table and the dynamic string table. This will be used
	 * later for looking up the address for each given function name.
	 */
	const Elf64_Shdr *shdr = (const Elf64_Shdr *)((const char *)vdso + ehdr->e_shoff);
	const Elf64_Shdr *symbols_header = NULL;
	const Elf64_Shdr *strings_header = NULL;

	for (int i = 0; i < ehdr->e_shnum; i++) {
		if (shdr[i].sh_type == SHT_DYNSYM) {
			symbols_header = &shdr[i];
		} else if (shdr[i].sh_type == SHT_STRTAB) {
			const char *shstrtab = (const char *)vdso + shdr[ehdr->e_shstrndx].sh_offset;
			const char *strtab_name = shstrtab + shdr[i].sh_name;

			if (strcmp(strtab_name, ".dynstr") == 0) {
				strings_header = &shdr[i];
			}
		}
	}
	if (!symbols_header || !strings_header) {
		die("Could not find .dynsym or .dynstr sections\n");
	}

	/*
	 * Now that we know where to pick the strings and their corresponding
	 * addresses, let's use their offsets to pair it with the original base
	 * 'vdso' address. This will give us a pointer to the proper tables.
	 */
	const Elf64_Sym *symbols_table =
		(const Elf64_Sym *)((const char *)vdso + symbols_header->sh_offset);
	const char *strings_table = (const char *)((const char *)vdso + strings_header->sh_offset);
	int num_symbols = symbols_header->sh_size / symbols_header->sh_entsize;

	/*
	 * As an example we will find the 'getcpu' system call which is exported on
	 * a Linux x86_64 system.
	 */
	int (*vdso_getcpu)(unsigned int *, unsigned int *) = NULL;

	printf("=> The following system calls are available via vDSO:\n");
	for (int i = 0; i < num_symbols; i++) {
		// The name of the function directly sits at the strings table plus the
		// offset given on the symbols table.
		const char *name = strings_table + symbols_table[i].st_name;

		if (is_vdso_function(name)) {
			void *address = (void *)((char *)vdso + symbols_table[i].st_value);
			printf("\t%s (address: %p)\n", name, address);

			if (strcmp(name, "__vdso_getcpu") == 0) {
				vdso_getcpu = (int (*)(unsigned int *, unsigned int *))address;
			}
		}
	}

	/*
	 * As an example, run the 'getcpu' system call via the vDSO mechanism.
	 */
	if (vdso_getcpu) {
		unsigned int cpu, core;
		int res = vdso_getcpu(&cpu, &core);
		if (res < 0) {
			perror("");
			exit(EXIT_FAILURE);
		}
		printf("=> After running 'getcpu' via vDSO: cpu: %d - core: %d\n", cpu, core);
	} else {
		printf("=> No 'getcpu' fun this time around!\n");
	}

	exit(EXIT_SUCCESS);
}
