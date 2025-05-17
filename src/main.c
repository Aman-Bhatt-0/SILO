#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "child.h"
#include "mount.h"
#include "cap.h"

int choose_hostname(char *buf, size_t size) {
    // Simple hostname chooser (you can improve this)
    strncpy(buf, "mycontainer", size - 1);
    buf[size - 1] = '\0';
    return 0;
}
int main(int argc, char **argv) {

    /* Store proc info */
    struct proc_info config = {0};

    /* Error code to return from main program */
    int err = 0;
    /* Used to store flag given by user. */
    int option = 0;

    int lastopt = 0;
    while ((option = getopt(argc, argv, "c:m:"))) {
        switch (option) {
            // -c <process-to-be-run> (e.g. -c /bin/sh)
            case 'c':
                config.argc = argc - lastopt - 1;
                config.argv = &argv[argc - 1];
                goto finish_config;
            case 'm':
                config.mount_dir = optarg;
                break;
            default:
                goto usage;
        }
    }

finish_config:

    if (!config.argc) goto usage;
    if (!config.mount_dir) goto usage;

    /* Choosing hostname for container. */
    fprintf(stdout, "=> choosing hostname for container..\n");
    char hostname[10] = {0};
    if (choose_hostname(hostname, sizeof(hostname)))
        goto err;
    config.hostname = hostname;
    fprintf(stdout, "=> Hostname: %s\n", config.hostname);


    /* Create (fork) child process, which will run executable.*/
    if(create_child_process(CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUTS, &config) == -1) {
        fprintf(stderr, "=> create child process failed! %m\n");
        goto err;
    }

    goto cleanup;

usage:
    fprintf(stderr, "Usage: %s -m <mount-dir> -c /bin/sh ~\n", argv[0]);
err:
    err = 1;
cleanup:
    fprintf(stdout, "Done.\n");
    return err;
}
