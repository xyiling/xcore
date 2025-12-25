#ifndef SYSCALL_H
#define SYSCALL_H

#include "../include/type.h"

void init_syscall(void);
int32_t handle_syscall(uint32_t syscall_no, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

#endif

