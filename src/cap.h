#ifndef CAP_H
#define CAP_H

#include <sys/prctl.h>
#include <sys/capability.h>
#include <stdio.h>

int drop_capabilities(void);

#endif // CAP_H

