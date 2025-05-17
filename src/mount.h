#ifndef MOUNTS_H
#define MOUNTS_H

#include "child.h"
#include "proc_info.h"


int setmountns(struct proc_info *info);
int prepare_procfs();

#endif
