A bunch of code that messes with low level stuff. That is:

- `arch/riscv/`:
  - `machine`: prints "hello, world" on the serial port while running in M mode
    on QEMU. Check the [README file](./arch/riscv/machine/README.md) for more
    info.
  - `supervisor/hsm`: using the HSM extension from SBI to gather info from
    harts.
  - `user/`: user space programs that either delve into RISC-V assembly or that
    interface the Linux Kernel on the RISC-V architecture.
- `fs/btrfs`: miscellanous tests on BTRFS.
- `kernel/`: samples that explore the API exposed by the Linux kernel, both from
    kernel and user space.
- `lang/c`: miscellanous code on the C programming language.

A lot of this is not taken solely from my brain, and I will give credit whenever
it's relevant to each piece of code. For the rest of the code, the GPLv3+
license applies.
