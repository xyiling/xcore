#include "../include/type.h"
#include "../include/stdio.h"
#include "../include/string.h"

// 系统调用号定义
#define SYS_EXIT    1
#define SYS_WRITE   2
#define SYS_READ    3
#define SYS_OPEN    4
#define SYS_CLOSE   5
#define SYS_GETPID  6
#define SYS_FORK    7
#define SYS_EXEC    8
#define SYS_WAIT    9

// 系统调用处理函数指针类型
typedef int32_t (*syscall_handler_t)(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

// 系统调用处理函数数组
static syscall_handler_t syscall_handlers[256];

// 系统调用处理函数
int32_t sys_exit(uint32_t status, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int32_t sys_write(uint32_t fd, uint32_t buf, uint32_t count, uint32_t arg4);
int32_t sys_read(uint32_t fd, uint32_t buf, uint32_t count, uint32_t arg4);
int32_t sys_open(uint32_t filename, uint32_t flags, uint32_t mode, uint32_t arg4);
int32_t sys_close(uint32_t fd, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int32_t sys_getpid(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int32_t sys_fork(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int32_t sys_exec(uint32_t filename, uint32_t argv, uint32_t envp, uint32_t arg4);
int32_t sys_wait(uint32_t status, uint32_t arg2, uint32_t arg3, uint32_t arg4);

// 系统调用处理函数实现
int32_t sys_exit(uint32_t status, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg2; (void)arg3; (void)arg4;
    // 简单的退出处理
    printf("Process exit with status: %u\n", status);
    return 0;
}

int32_t sys_write(uint32_t fd, uint32_t buf, uint32_t count, uint32_t arg4) {
    (void)arg4;
    if (fd == 1 || fd == 2) {  // stdout or stderr
        char *str = (char *)buf;
        for (uint32_t i = 0; i < count; i++) {
            putchar(str[i]);
        }
        return count;
    }
    return -1;
}

int32_t sys_read(uint32_t fd, uint32_t buf, uint32_t count, uint32_t arg4) {
    (void)fd; (void)buf; (void)count; (void)arg4;
    // 简单的读取实现
    return 0;
}

int32_t sys_open(uint32_t filename, uint32_t flags, uint32_t mode, uint32_t arg4) {
    (void)filename; (void)flags; (void)mode; (void)arg4;
    // 简单的打开实现
    return 0;
}

int32_t sys_close(uint32_t fd, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)fd; (void)arg2; (void)arg3; (void)arg4;
    // 简单的关闭实现
    return 0;
}

int32_t sys_getpid(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4;
    return 1;  // 简单的PID返回
}

int32_t sys_fork(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4;
    // 简单的fork实现
    return -1;
}

int32_t sys_exec(uint32_t filename, uint32_t argv, uint32_t envp, uint32_t arg4) {
    (void)filename; (void)argv; (void)envp; (void)arg4;
    // 简单的exec实现
    return -1;
}

int32_t sys_wait(uint32_t status, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)status; (void)arg2; (void)arg3; (void)arg4;
    // 简单的wait实现
    return -1;
}

// 系统调用处理入口
int32_t handle_syscall(uint32_t syscall_no, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    if (syscall_no >= 256 || !syscall_handlers[syscall_no]) {
        return -1;
    }
    return syscall_handlers[syscall_no](arg1, arg2, arg3, arg4);
}

// 初始化系统调用
void init_syscall(void) {
    memset(syscall_handlers, 0, sizeof(syscall_handlers));
    
    syscall_handlers[SYS_EXIT] = sys_exit;
    syscall_handlers[SYS_WRITE] = sys_write;
    syscall_handlers[SYS_READ] = sys_read;
    syscall_handlers[SYS_OPEN] = sys_open;
    syscall_handlers[SYS_CLOSE] = sys_close;
    syscall_handlers[SYS_GETPID] = sys_getpid;
    syscall_handlers[SYS_FORK] = sys_fork;
    syscall_handlers[SYS_EXEC] = sys_exec;
    syscall_handlers[SYS_WAIT] = sys_wait;
}

