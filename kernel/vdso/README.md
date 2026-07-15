## vDSO

The "vDSO" (virtual dynamic shared object) is a small shared library that the
kernel automatically maps into the address space of all user-space
applications. This way, user space can perform some system calls via this
interface and avoid the cost of entering kernel space.

This is important for system calls that the community has detected to be
performed most often by all applications. Note that the regular joe doesn't have
to deal with any of this as glibc and the likes will already take care of this.

For much more, this is all better documented via `man vdso`.

### This example

No arguments are required, just:

```
$ make
$ ./vdso
=> vDSO base address: 0x7fab930dc000S
=> The following system calls are available via vDSO:
	__vdso_gettimeofday (address: 0x7fab930dc890)
	__vdso_clock_getres (address: 0x7fab930dcf70)
	__vdso_time (address: 0x7fab930dcb80)
	__vdso_sgx_enter_enclave (address: 0x7fab930dd530)
	__vdso_getrandom (address: 0x7fab930dd020)
	__vdso_clock_gettime (address: 0x7fab930dcbb0)
	__vdso_getcpu (address: 0x7fab930dcfe0)
=> After running 'getcpu' via vDSO: cpu: 14 - core: 0
```

That is, this program:

1. Fetches the base address for the vDSO.
2. Parses the ELF file that is the vDSO and obtains the name of all functions
   starting with `__vdso_*` (i.e. those that the kernel expects to be called).
3. As an example, it calls the `getcpu` system call by calling the parsed
   address for it directly.
