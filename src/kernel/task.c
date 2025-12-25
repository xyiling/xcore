#include "../include/type.h"
#include "../include/stdio.h"
#include "../include/string.h"

// 任务状态
typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_ZOMBIE
} task_state_t;

// 任务控制块（TCB）
typedef struct task {
    uint32_t pid;               // 进程ID
    uint32_t esp;               // 栈指针
    uint32_t ebp;               // 基址指针
    task_state_t state;         // 任务状态
    uint32_t priority;          // 优先级
    struct task *next;          // 下一个任务
    char name[32];              // 任务名称
} task_t;

// 最大任务数
#define MAX_TASKS 64
static task_t tasks[MAX_TASKS];
static uint32_t task_count = 0;
static task_t *current_task = NULL;
static task_t *task_queue = NULL;

// 初始化任务系统
void init_task_system(void) {
    memset(tasks, 0, sizeof(tasks));
    task_count = 0;
    current_task = NULL;
    task_queue = NULL;
}

// 创建新任务
task_t *create_task(const char *name, void (*entry)(void), uint32_t stack_size) {
    if (task_count >= MAX_TASKS) {
        return NULL;
    }
    
    task_t *task = &tasks[task_count++];
    task->pid = task_count;
    task->state = TASK_READY;
    task->priority = 0;
    strncpy(task->name, name, 31);
    task->name[31] = '\0';
    
    // 分配栈空间（简化实现）
    static uint8_t task_stacks[MAX_TASKS][4096];
    uint32_t *stack = (uint32_t *)task_stacks[task_count - 1];
    stack += stack_size / sizeof(uint32_t);
    
    // 设置初始栈帧
    *--stack = 0x202;           // EFLAGS
    *--stack = 0x08;            // CS
    *--stack = (uint32_t)entry; // EIP
    *--stack = 0;               // EAX
    *--stack = 0;               // ECX
    *--stack = 0;               // EDX
    *--stack = 0;               // EBX
    *--stack = 0;               // ESP
    *--stack = 0;               // EBP
    *--stack = 0;               // ESI
    *--stack = 0;               // EDI
    
    task->esp = (uint32_t)stack;
    task->ebp = (uint32_t)stack;
    
    // 添加到任务队列
    task->next = task_queue;
    task_queue = task;
    
    return task;
}

// 任务切换
void task_switch(task_t *next_task) {
    if (current_task == next_task) {
        return;
    }
    
    task_t *prev_task = current_task;
    current_task = next_task;
    
    if (prev_task) {
        // 保存当前任务上下文
        asm volatile(
            "movl %%esp, %0\n\t"
            "movl %%ebp, %1\n\t"
            : "=m"(prev_task->esp), "=m"(prev_task->ebp)
            :
        );
    }
    
    if (next_task) {
        // 恢复新任务上下文
        asm volatile(
            "movl %0, %%esp\n\t"
            "movl %1, %%ebp\n\t"
            : 
            : "m"(next_task->esp), "m"(next_task->ebp)
        );
    }
}

// 调度器
void schedule(void) {
    if (!task_queue) {
        return;
    }
    
    // 简单的轮询调度
    task_t *next = task_queue;
    if (current_task) {
        // 找到当前任务的下一个
        task_t *curr = task_queue;
        while (curr && curr->next != current_task) {
            curr = curr->next;
        }
        if (curr && curr->next) {
            next = curr->next;
        } else {
            next = task_queue;
        }
    }
    
    task_switch(next);
}

// 获取当前任务
task_t *get_current_task(void) {
    return current_task;
}

