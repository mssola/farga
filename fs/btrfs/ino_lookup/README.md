The ["inode lookup"
ioctl](https://btrfs.readthedocs.io/en/latest/btrfs-ioctl.html#btrfs-ioc-ino-lookup)
allows you to request certain information for a given inode. This ioctl is a bit
special because it has two modes of operation.

First, you can provide both an inode and a tree ID. Doing this will allow
callers to retrieve the name that identifies the given inode on the tree
ID. Doing this requires `CAP_SYS_ADMIN` privileges. If you don't know which tree
ID to pick, you can also pick `0`, which selects the current subvolume you are
currently working with.

I have written a small example in the form of `ino_lookup.c`, which performs a
call to this ioctl. I do this with a mount point with four subvolumes like so:

```
# btrfs subvolume list mnt/
ID 256 gen 138 top level 5 path a
ID 257 gen 138 top level 5 path b
ID 258 gen 138 top level 5 path c
ID 259 gen 138 top level 5 path d

# tree --inodes --device mnt/
[    256  55]  mnt/
├── [    256  46]  a
│   ├── [    257  46]  a.txt
│   └── [    258  46]  b.txt
├── [    257  55]  a.txt
├── [    256  47]  b
│   └── [    257  47]  b.txt
├── [    256  56]  c
│   └── [    257  56]  c.txt
└── [    256  57]  d
    └── [    257  57]  d.txt
```

The first argument to pass to this small program is the tree ID, and the second
is the inode. With this into account, I have written a small shell script that
runs this program with different arguments like so (output with `set -x`):

```
# ./lookup_test.sh
+ ./ino_lookup mnt 256 258
tree ID: 256
inode: 258
path:  b.txt/
+ ./ino_lookup mnt 0 257
tree ID: 5
inode: 257
path:  a.txt/
+ ./ino_lookup mnt 256 257
tree ID: 256
inode: 257
path:  a.txt/
+ ./ino_lookup mnt 257 257
tree ID: 257
inode: 257
path:  b.txt/
+ ./ino_lookup mnt/a 0 257
tree ID: 256
inode: 257
path:  a.txt/
+ ./ino_lookup mnt/a 256 257
tree ID: 256
inode: 257
path:  a.txt/
+ ./ino_lookup mnt/a 257 257
tree ID: 257
inode: 257
path:  b.txt/
```

But, as I briefly mentioned earlier, this ioctl also has another mode of
operation. This mode of operation does not require `CAP_SYS_ADMIN` and you can
call it by setting the tree ID to 0, and passing the special
`BTRFS_FIRST_FREE_OBJECTID` constant as a value for the inode. With all of this
you will be able to fetch the tree ID for the targetted mount point. Hence:

```
$ ./ino_lookup mnt/ 0 --tree-id
tree ID: 5
inode: 256
path:
$ ./ino_lookup mnt/a 0 --tree-id
tree ID: 256
inode: 256
path:
$ ./ino_lookup mnt/b 0 --tree-id
tree ID: 257
inode: 256
path:
$ ./ino_lookup mnt/c 0 --tree-id
tree ID: 258
inode: 256
path:
$ ./ino_lookup mnt/d 0 --tree-id
tree ID: 259
inode: 256
path:
```

You will also notice that:

1. The inode is always set to `256`. This ioctl doesn't change the value, it
   just happens to be the value of the `BTRFS_FIRST_FREE_OBJECTID` constant we
   set for this mode of operation.
2. The path will be blank out on purpose.

## Patches sent to the Linux kernel as a result

I arrived to this ioctl pretty much out of nowhere as I was cruising through
another patch set I was working on. After looking into it later, I saw that a
bit of cleanup could be done on the kernel side. Then I worked on the patch and
pretty much forgot about it when I went into paternity leave :)

Once I got back and saw the patch as something to be sent, I rebased the patch
and submitted it. Read the discussion in
[lore.kernel.org](https://lore.kernel.org/all/20260629142056.309300-1-mssola@mssola.com/).
