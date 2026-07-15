This example contains a set of files which showcase how the shutdown ioctl works
on BTRFS. This ioctl was introduced in Linux 6.19 and it was marked as an
experimental feature.

BTRFS has been able to freeze the filesystem for a long time, which might sound
similar to shutting it down. The main difference is that shutdown is meant for a
complete, persistent power-off; while freezing can be undone, it's temporary. As
it stands in Linux kernel 6.19, BTRFS shutdown comes in two flavors:

1. _With_ log flushing (default): commits the current transaction, leaving the
   file system into a consistent state, and shuts down the filesystem right
   after freezing it. Then it's marked as thawed, but the shutdown state is
   preserved. This is a bit of trickery to get all logs flushed, the filesystem
   consistent, but with the proper flag set to shutdown.
2. _Without_ log flushing: discards any current transaction to keep the
   filesystem into a consistent state and shuts down the filesystem right
   away. This is probably the fastest and most aggresive route.

## Requirements

In order to be able to run this example as intended you need:

- Linux kernel 6.19+ with BTRFS experimental features enabled. **NOTE**: the
  shutdown ioctl is no longer marked as experimental since the Linux kernel
  v7.1.
- A Linux kernel built with the following patch applied: (["btrfs: report
   filesystem shutdown via
   fserror"](https://lore.kernel.org/linux-btrfs/20260216002806.3831884-1-mssola@mssola.com)). **NOTE**:
   this patch is applied as of Linux kernel v7.1.

Thus, if your Linux kernel is v7.1 or later, then you are good to go.

## Listening for filesystem errors

BTRFS will notify user-space whenever a shutdown happens. This way user-space
can mark this device as not available and hence can avoid writing into it. The
`fserror.c` file contains a small program that does just that. It receives a
mount point as an argument, and then waits for events on that filesystem,
filtering for the shutdown error. Run this program and then go onto the next
section. Whenever the shutdown happens, the terminal from where you executed
`fserror.c` will print the following:

```
Shutdown detected!
```

If you don't see it, either the kernel you have does not support BTRFS shutdown
yet, or some requirements are missing.

## Actually shutting down

The `shutdown.c` file contains a small program which accepts a mount point as an
argument. It then checks that it's a BTRFS mount point, and afterwards it calls
the shutdown ioctl with the default flags (i.e. with log flushing). Note that it
does not do any validation on the kernel you are using. Hence, if you are using
a Linux kernel version which has no support for this (e.g. previous than 6.19 or
6.19 without BTRFS experimental features), you will get the following message:

```
Shutdown failed: Inappropriate ioctl for device
```

This is the `ETTY` errno message, which just means that the ioctl support for
this filesystem does not implement this function. This will also happen in
virtualized environments, as the virtualized filesystem (e.g. overlayfs) will be
on the receiving end of this ioctl and, hence, they will return the same errno.

But if you are running on a Linux kernel which does support this, then running
this program will give you the following message:

```
Success: filesystem shut down.
```

Then you will see the following message on `dmesg`:

```
[  201.048599][  T995] BTRFS critical (device sda state E): emergency shutdown
```

From now on you will receive `EROFS` errors whenever you try to write into this
device. For example:

```
# echo "hello" >> mnt/lala.txt
bash: mnt/lala.txt: Read-only file system
```

## Patches sent to the Linux kernel as a result

As a result of looking into this I have sent two patches to the Linux kernel:

1. ["btrfs: report filesystem shutdown via
   fserror"](https://lore.kernel.org/linux-btrfs/20260216002806.3831884-1-mssola@mssola.com),
   which adds the 'fanotify' event for filesystem shutdown in BTRFS.
2. ["btrfs: don't commit the super block when unmounting a shutdown
   filesystem"](https://lore.kernel.org/linux-btrfs/20260216002252.3831277-1-mssola@mssola.com),
   which prevents BTRFS from trying to commit the super block on filesystem
   unmount after a filesystem was shutdown.
