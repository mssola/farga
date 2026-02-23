#define _GNU_SOURCE

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/btrfs.h>
#include <linux/magic.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/statfs.h>

/*
 * Constants defined in BTRFS' experimental shutdown support
 * starting from Linux 6.19.
 */

#ifndef BTRFS_SHUTDOWN_FLAGS_DEFAULT
#define BTRFS_SHUTDOWN_FLAGS_DEFAULT	0x0
#endif

#ifndef BTRFS_IOC_SHUTDOWN
#define BTRFS_IOC_SHUTDOWN	_IOR('X', 125, __u32)
#endif

int main(int argc, char *argv[])
{
	int fd;
	__u32 flags;
	struct statfs info;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <mount point>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (statfs(argv[1], &info) == 0) {
		/*
		 * We could certainly use this ioctl against EXT4 or XFS, but that's not
		 * the point we want to illustrate here.
		 */
		if (info.f_type != BTRFS_SUPER_MAGIC) {
			printf("error: you are supposed to be running this against BTRFS;"
				   " %lx magic detected instead\n", info.f_type);
			exit(EXIT_FAILURE);
		}
	} else {
		perror("statfs failed");
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY | O_DIRECTORY);
	if (fd < 0) {
	    perror("open");
		exit(EXIT_FAILURE);
	}

	flags = BTRFS_SHUTDOWN_FLAGS_DEFAULT;
	if (ioctl(fd, BTRFS_IOC_SHUTDOWN, &flags) == 0) {
	    printf("Success: filesystem shut down.\n");
	} else {
	    perror("Shutdown failed");
		exit(EXIT_FAILURE);
	}

	close(fd);
	return 0;
}
