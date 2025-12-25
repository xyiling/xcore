#ifndef TASK_H
#define TASK_H

#include "../include/type.h"

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_ZOMBIE
} task_state_t;

typedef struct task {
    uint32_t pid;
    uint32_t esp;
    uint32_t ebp;
    task_state_t state;
    uint32_t priority;
    struct task *next;
    char name[32];
} task_t;

void init_task_system(void);
task_t *create_task(const char *name, void (*entry)(void), uint32_t stack_size);
void task_switch(task_t *next_task);
void schedule(void);
task_t *get_current_task(void);

#endif

