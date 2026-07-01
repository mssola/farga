#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/btrfs.h>
#include <linux/btrfs_tree.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "usage: %s <mount point> <tree ID> <inode number | --tree-id>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	const char *const path = argv[1];
	unsigned long long treeid = strtoull(argv[2], NULL, 10);

	unsigned long long ino;
	if (strcmp(argv[3], "--tree-id") == 0) {
		ino = BTRFS_FIRST_FREE_OBJECTID;
		if (treeid > 0) {
			printf("warning: the given tree ID value will be discarded.\n");
		}
		treeid = 0;
	} else {
		ino = strtoull(argv[3], NULL, 10);
	}

	int fd = open(path, O_RDONLY | O_DIRECTORY);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	// Check the README.md file on the args.
	struct btrfs_ioctl_ino_lookup_args args;
	memset(&args, 0, sizeof(args));
	args.treeid = treeid;
	args.objectid = ino;

	if (ioctl(fd, BTRFS_IOC_INO_LOOKUP, &args) < 0) {
		perror("BTRFS_IOC_INO_LOOKUP");
		close(fd);
		exit(EXIT_FAILURE);
	}

	printf("tree ID: %llu\n", args.treeid);
	printf("inode: %llu\n", ino);
	printf("path:  %s\n", args.name);

	close(fd);
	return 0;
}
