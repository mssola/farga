A bunch of code to messes with low level stuff. That is:

- `arch`:
  - `machine`: prints "hello, world" on the serial port while running in M mode
    on QEMU. Check the [README file](./arch/riscv/machine/README.md) for more
    info.
  - `supervisor/hsm`: using the HSM extension from SBI to gather info from
    harts.
  - `user/`:
    - `basics`: bunch of code checking on basic programming concepts from the
      RISC-V ISA.
    - `hwprobe`: using the `hwprobe` system call from the Linux kernel to check
      on the features from the current machine.
- `kernel/dtm`: simple kernel module showcasing the API from the `of` driver.

A lot of this is not taken solely from my brain, and I will give credit whenever
it's relevant to each piece of code. For the rest of the code, the GPLv3+
license applies.
