#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fanotify.h>

int main(int argc, char *argv[])
{
    int fd;
    char buf[4096];
    ssize_t len;
    struct fanotify_event_metadata *metadata;
    struct fanotify_event_info_error *error_info;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <mount point>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*
	 * Initialize 'fanotify' with the default value and `FAN_REPORT_FID`, which
	 * will allow use to filter by `FAN_FS_ERROR` down the line.
	 */
    fd = fanotify_init(FAN_CLASS_NOTIF | FAN_REPORT_FID, O_RDONLY);
    if (fd < 0) {
        perror("fanotify_init");
        exit(EXIT_FAILURE);
    }

	/*
	 * Add a 'fanotify' mark to monitor all files from the filesystem pointed by
	 * 'fd'. Note that this general monitoring via 'FAN_MARK_FILESYSTEM' is the
	 * thing that makes this program to need root permission as documented in
	 * the manpage: "Use of this flag requires the CAP_SYS_ADMIN capability".
	 */
    if (fanotify_mark(fd, FAN_MARK_ADD | FAN_MARK_FILESYSTEM,
                      FAN_FS_ERROR, AT_FDCWD, argv[1]) < 0) {
        perror("fanotify_mark");
        exit(EXIT_FAILURE);
    }

    printf("Listening for filesystem errors on '%s'\n", argv[1]);

    // Event Loop
    while ((len = read(fd, buf, sizeof(buf))) > 0) {
        metadata = (struct fanotify_event_metadata *)buf;

        while (FAN_EVENT_OK(metadata, len)) {
			/*
			 * We only care about filesystem errors, which is the only thing we
			 * passed to 'fanotify_mark'. That being said, just to be sure,
			 * let's filter this out.
			 */
            if (metadata->mask & FAN_FS_ERROR) {
                // fancy...
                error_info = (struct fanotify_event_info_error *)
                             ((char *)metadata + metadata->event_len - sizeof(*error_info));

				if (error_info->error == ESHUTDOWN) {
					printf("Shutdown detected!\n");
					goto done;
				} else {
					printf("Filesystem error detected with code: %d\n", error_info->error);
				}
            }

            metadata = FAN_EVENT_NEXT(metadata, len);
        }
    }

 done:
    close(fd);
    return 0;
}
