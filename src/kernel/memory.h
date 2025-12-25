#ifndef MEMORY_H
#define MEMORY_H

#include "../include/type.h"

typedef struct {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
} __attribute__((packed)) ards_t;

typedef struct {
    uint32_t total_memory;
    uint32_t available_memory;
    uint32_t ards_count;
} memory_info_t;

void memory_detect(void);
memory_info_t *get_memory_info(void);
void print_memory_info(void);

#endif

