#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "child.h"
#include "mount.h"      // For setmountns
#include "cap.h"        // For drop_capabilities


static int childFunc(void *arg)
{
    struct proc_info *info = arg;

    if (sethostname(info->hostname, strlen(info->hostname)) || setmountns(info))
        return -1;

    struct utsname uts;
    if (uname(&uts) == -1)
        return -1;

    drop_capabilities();

    if (execve(info->argv[0], info->argv, NULL)) {
        perror("execve failed");
        return -1;
    }

    fprintf(stdout, "Child process started. Hostname: %s", uts.nodename);

    sleep(10);

    return 0;
}

int create_child_process(int flags, struct proc_info *config)
{
    int err = 0;
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        fprintf(stderr, "=> malloc failed, out of memory?\n");
        return -1;
    }

    char *stackTop = stack + STACK_SIZE;

    pid_t pid = clone(childFunc, stackTop, flags | SIGCHLD, config);
    if (pid == -1) {
        perror("=> child clone failed");
        free(stack);
        return -1;
    }

    fprintf(stdout, "=> child process created with PID %d\n", pid);

    int child_status = 0;
    waitpid(pid, &child_status, 0);
    err |= WEXITSTATUS(child_status);
    fprintf(stdout, "=> child exited with %d\n", WEXITSTATUS(child_status));

    free(stack);
    return err;
}

