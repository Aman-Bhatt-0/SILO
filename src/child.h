#ifndef CHILD_H
#define CHILD_H

#include <stddef.h>  // for size_t

#include "proc_info.h"

// Stack size used for child process
#define STACK_SIZE (1024 * 1024)

// Function to create child process
int create_child_process(int flags, struct proc_info *config);

// The child function is made static in the C file and not declared here.

#endif // CHILD_H

