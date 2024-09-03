# TL;DR

This example is an extension from [this blog post from Uros
Popovic](https://popovicu.com/posts/bare-metal-programming-risc-v/). Here we
take full advantage that the early boot process will pass down the
[FDT](https://devicetree-specification.readthedocs.io/en/stable/flattened-format.html)
to the `a1` register. This way we don't have to hardcode the value for the UART
address.

# Description

When booting up a RISC-V system it will start in M ("machine") privilege mode.
This mode allows code to access any piece of the hardware and it will be able to
interact with it without any intermediaries. This is where projects like
[openSBI](https://github.com/riscv-software-src/opensbi) come into place. That
is, this is software that is meant to be run at M privilege mode and provide an
API wrapper (see the [RISC-V
SBI](https://github.com/riscv-non-isa/riscv-sbi-doc) documentation) so code
above it (in S privilege mode) can talk to it without having to deal with the
myriad of hardware configurations.

That being said this example scratches that and is meant to be run at this very
early stage of boot time. So, how does it work? Well, when you hit the power
button the system will start by running the ZSBL (Zero-Stage Boot Loader). You
can check this boot loader from QEMU take place when running this example in
debug mode. So, on one terminal run:

```
$ make DEBUG=1
```

And then on another terminal:

```
$ make gdb
```

If you step on the code you will get the following:

```
0x0000000000001000 in ?? ()
=> 0x0000000000001000:	00000297        	auipc	t0,0x0
(gdb) si
0x0000000000001004 in ?? ()
=> 0x0000000000001004:	02828613        	addi	a2,t0,40
(gdb) si
0x0000000000001008 in ?? ()
=> 0x0000000000001008:	f1402573        	csrr	a0,mhartid
(gdb) si
0x000000000000100c in ?? ()
=> 0x000000000000100c:	0202b583        	ld	a1,32(t0)
(gdb) si
0x0000000000001010 in ?? ()
=> 0x0000000000001010:	0182b283        	ld	t0,24(t0)
(gdb) si
0x0000000000001014 in ?? ()
=> 0x0000000000001014:	00028067        	jr	t0
```

That is, only six instructions before it jumps to our code (how it knows where
to jump is another story which I won't dwelve right now). From this, `a0` will
contain the Hart ID and `a1` will contain a pointer to the
[FDT](https://devicetree-specification.readthedocs.io/en/stable/flattened-format.html)
structure. This can be further validated by looking at the [QEMU
code](https://github.com/qemu/qemu/blob/039003995047b2f7911142c7c5cfb845fda044fd/hw/riscv/boot.c#L397-L409)
that handles the reset vector. As for the FDT structure, you can check the
default one from QEMU with the following commands:

```
$ qemu-system-riscv64 -machine virt -machine dumpdtb=qemu.dtb
$ dtc -I dtb -O dts -o - qemu.dtb
```

The important information here is that the `serial` device is mapped to a
specific physical address. This address could have also been validated, again,
by [checking at QEMU's
code](https://github.com/qemu/qemu/blob/23fa74974d8c96bc95cbecc0d4e2d90f984939f6/hw/riscv/virt.c#L82).
Either way, if you send bytes to this physical address then these bytes are
guaranteed to be passed into the `serial` port. This is what the code from
[@popovicu](https://popovicu.com/posts/bare-metal-programming-risc-v/) used to
do:

1. Hardcode the value for the UART device.
2. Send one byte at a time there to print "hello".

As a shorter example, the following code will send "h" to the serial port:

``` assembly
addi a0, x0, 0x68  # `a0` now contains the ASCII value for "h"
li a1, 0x10000000  # the hardcoded UART address
sb a0, (a1)        # send the byte to the UART address
```

This is fine and all, but I wanted to take a step further. Now the code does the
following:

1. Specifically requires `a1` to contain a pointer to the FDT blob.
2. Calls the newly created `get_serial_address` function to parse the FDT blob
   in search for the address of the `serial` device.

After calling `get_serial_address` we will have the physical address stored in
`a0`. From then on we could have just:

``` assembly
addi a1, x0, 0x68  # `a1` now contains the ASCII value for 'h'
sb a1, (a0)        # send the byte to the UART address already stored in `a0`
```

But, as a cherry on top, I have written a simple `printm` function, which allows
the caller to print a whole message.

# Special thanks to

Thanks to [@popovicu](https://github.com/popovicu) and his blog. More
specifically, [this blog post from
him](https://popovicu.com/posts/bare-metal-programming-risc-v/) was specially
useful as an entry point to this whole topic.
