# Device Tree Module

This is a simple kernel module which show cases some of the kernel API functions
for manipulating device tree nodes.

In order to build this module you need either a local build of the Linux kernel,
or the the source that you can get from your Linux distribution (e.g.
`kernel-source` on openSUSE). For the former you will need to specify the path
to your local development tree with the `KERNEL` variable. So:

```
$ make KERNEL=<path/to/development/tree>
```

This will produce a `dtm.ko` file, which you will need to `insmod` on your
target machine. Then you will get this output from `dmesg`:

```
[  571.298548] [    T813] dtm: loading out-of-tree module taints kernel.
[  571.305842] [    T813] Inside of VisionFive2
[  571.309945] [    T813] RISC-V ISA for 'cpu@1': rv64imafdc_zba_zbb
[  571.315826] [    T813] parent: cpus
[  571.319091] [    T813] RISC-V ISA for 'cpu@2': rv64imafdc_zba_zbb
[  571.324962] [    T813] parent: cpus
[  571.328224] [    T813] RISC-V ISA for 'cpu@3': rv64imafdc_zba_zbb
[  571.334092] [    T813] parent: cpus
[  571.337354] [    T813] RISC-V ISA for 'cpu@4': rv64imafdc_zba_zbb
[  571.343228] [    T813] parent: cpus
```
