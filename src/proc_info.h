// src/proc_info.h
#ifndef PROC_INFO_H
#define PROC_INFO_H

struct proc_info {
    char *hostname;
    char *mount_dir;
    char **argv;
    int argc;
};

#endif // PROC_INFO_H

