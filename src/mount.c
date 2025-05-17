#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <errno.h>

#include "mount.h"

int pivot_root(const char *new_root, const char *put_old)
{
    return syscall(SYS_pivot_root, new_root, put_old);
}

int prepare_procfs()
{
    if (rmdir("/proc")) {
        fprintf(stderr, "rmdir /proc failed! %m\n");
        return -1;
    }
    if (mkdir("/proc", 0555)) {
        fprintf(stderr, "Failed to mkdir /proc!\n");
        return -1;
    }
    if (mount("proc", "/proc", "proc", 0, "")) {
        fprintf(stderr, "Failed to mount proc!\n");
        return -1;
    }
    return 0;
}

int setmountns(struct proc_info *info)
{
    fprintf(stderr, "=> mounting MNT namespace to %s\n", info->mount_dir);

    fprintf(stderr, "=> remounting everything with MS_PRIVATE...");
    if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL)) {
        fprintf(stderr, "failed! %m\n");
        return -1;
    }
    fprintf(stderr, "remounted.\n");

    char mount_dir[] = "/tmp/tmp.XXXXXX";
    if (!mkdtemp(mount_dir)) {
        fprintf(stderr, "failed making a directory!\n");
        return -1;
    }

    if (mount(info->mount_dir, mount_dir, NULL, MS_BIND | MS_PRIVATE, NULL)) {
        fprintf(stderr, "bind mount failed!\n");
        return -1;
    }

    char inner_mount_dir[] = "/tmp/tmp.XXXXXX/oldroot.XXXXXX";
    memcpy(inner_mount_dir, mount_dir, sizeof(mount_dir) - 1);
    if (!mkdtemp(inner_mount_dir)) {
        fprintf(stderr, "failed making the inner directory!\n");
        return -1;
    }

    fprintf(stderr, "=> pivoting root...");
    if (pivot_root(mount_dir, inner_mount_dir)) {
        fprintf(stderr, "failed!\n");
        return -1;
    }
    fprintf(stderr, "done.\n");

    char *old_root_dir = basename(inner_mount_dir);
    char old_root[sizeof(inner_mount_dir) + 1] = { "/" };
    strcpy(&old_root[1], old_root_dir);

    if (prepare_procfs()) {
        fprintf(stderr, "Preparing procfs failed! %m\n");
        return -1;
    }

    fprintf(stderr, "=> unmounting %s...", old_root);
    if (chdir("/")) {
        fprintf(stderr, "chdir failed! %m\n");
        return -1;
    }
    if (umount2(old_root, MNT_DETACH)) {
        fprintf(stderr, "umount failed! %m\n");
        return -1;
    }
    if (rmdir(old_root)) {
        fprintf(stderr, "rmdir failed! %m\n");
        return -1;
    }

    fprintf(stderr, "done.\n");

    return 0;
}

