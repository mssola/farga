The RISC-V support for the Linux kernel has a new system call which is specific
to it, called `hwprobe`. With this system call you can check which features,
extensions, etc. are available on the current machine.

In order to build this example you need Linux headers for the RISC-V
architecture. You can achieve this in two ways:

1. Simply install the Linux headers from your distribution if you are already
   using a RISC-V machine.
2. Build the Linux kernel and use the `LINUX_HEADERS` environment variable to
   point to the headers directory (i.e. the directory produced by `make
   headers_install`)

After that, build it and run it. On a QEMU machine I got the following:

```
Supports base extensions I, M, A? yes

We got back this bit map: 0000001110001101001010000001000100100000000000000000000011111011; which means:

F, D: OK
C: OK
ZBA: OK
ZBB: OK
ZBS: OK
ZICBOZ: OK
ZBC: OK
ZIHINTNTL: OK
ZFA: OK
ZIHINTPAUSE: OK
ZCA: OK
ZCD: OK
ZAWRS: OK
ZICNTR: OK
ZIHPM: OK
```
