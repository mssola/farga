# Messing with the Hart State Management Extension ("HSM") from the SBI

This is a simple kernel that messes with the HSM Extension from the SBI v2.0. In
general, and according to the RISC-V specification, harts can enter Supervisor
mode at any time. This makes thing simple both for the RISC-V specification and
machines implementing the architecture. For the kernel this means that you never
know which hart is going to start first, and in cases like the Linux kernel, you
actually want to ensure that only one hart is running for initialization
purposes before waking up the rest. In order to guarantee this, the Linux kernel
runs a lottery.

To run a lottery on this context means that the first hart to appear will
actually take a lock in the kernel, which will block any other "secondary" hart
that appears later. This lock is going to be released once the "main" hart
guarantees that the kernel has been initialized up to a point where other harts
can come in. This lock is implemented with an atomic variable that holds how
many harts have been seen (hence, the first hart will read a zero value from
this variable, and the rest will read a non-zero one).

All of that being said, the SBI specification v2.0 comes with a cleaner
approach: the HSM extension. With this extension a Supervisor will be able to
manage the state of any hart of the system, which is either `started`, `stopped`
or `suspended` (plus transition states). Moreover, there is the guarantee that
only one hart will be initialized with the `started` state, whereas the rest
will be on `stopped`. This means that a kernel can drop the whole idea of a
lottery and have the guarantee that only one hart will run on start. Whenever
the kernel has been initialized to a specific stage, it will be able to change
the state of the rest of harts to `started`.

## This example

This example uses the HSM extension but it also keeps the idea of a lottery.
That is, the first hart will "acquire" the lock ("win" the lottery), and will be
responsible for bringing the rest up. At first this hart will simply print
information on the system. For example, on a system with four harts you will get
something like:

```
Hello, world!
Hart that won the lottery: 1
Harts still asleep: 0, 2, 3

How many harts have _participated_ in the lottery? 1
How many harts have _failed_ in the lottery? 0
```

In this example, only one hart is available: one that "won" the lottery, and the
rest have not even appeared. The hart with ID '1' will be responsible for waking
up '0', '2' and '3'. After making some initial checks, it will wake up the first
secondary hart (no reason to just wake up one, just doing it this way for the
show):

```
Waking up first secondary hart
How many harts have _participated_ in the lottery? 2
How many harts have _failed_ in the lottery? 1
```

Now two harts are available, but the one that woke up is a "secondary" one, so
it lost the lottery and is stuck in `head.S` on an infinite loop doing nothing.
After doing that, the main hart will wake up the rest:

```
Waking up the rest
How many harts have _participated_ in the lottery? 4
How many harts have _failed_ in the lottery? 3
```

Now all four harts are up, but three of them (the "secondary" ones) are simply
stuck in an infinite loop since they have "lost" the lottery. At this point this
example does something weird for a kernel: it will return from the main
function. Doing so allows `head.S` to count the main hart as a "failed" one, and
it will print some final messages for it:

```
Goodbye, cruel world!
How many harts have _participated_ in the lottery? 4
How many harts have _failed_ in the lottery? 4

THE END
```

At the point where "THE END" is printed, we will then perform an SBI call to the
System Reset Extension ("SRST") to gracefully shutdown the system.

## Test

To test this yourself you need a recent enough QEMU which is able to run as a
RISC-V system, and you need to set the `CROSS_COMPILE` environment variable as
you would on the Linux Kernel if you are doing this on a non-RISC-V machine.
After that, you can simply run `make` to build the binary, or simply run `make
qemu` to both build it and run QEMU.

Notice also that the Makefile accepts the `CPUS` variable, which by default is
set to 4. Change this variable to something else to get a different number of
harts on the example (but lesser than 10, since I haven't bothered to account
for that when printing numerical values). For example, `make qemu CPUS=8`:

``` shell
Hello, world!
Hart that won the lottery: 5
Harts still asleep: 0, 1, 2, 3, 4, 6, 7
How many harts have _participated_ in the lottery? 1
How many harts have _failed_ in the lottery? 0

Waking up first secondary hart
How many harts have _participated_ in the lottery? 2
How many harts have _failed_ in the lottery? 1

Waking up the rest
How many harts have _participated_ in the lottery? 8
How many harts have _failed_ in the lottery? 7

Goodbye, cruel world!
How many harts have _participated_ in the lottery? 8
How many harts have _failed_ in the lottery? 8

THE END
```

## Rationale

This example came after exploring the topic of SMP on
[fbos](https://github.com/mssola/fbos). Even if in there I did not explore this
extension further (because of the needs of the project), I decided that I wanted
a fresh simple kernel to mess with this extension. And hence this project.
