#include "../include/type.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "memory.h"

// 内存类型定义
#define ARDS_TYPE_AVAILABLE 1
#define ARDS_TYPE_RESERVED 2
#define ARDS_TYPE_ACPI_RECLAIMABLE 3
#define ARDS_TYPE_ACPI_NVS 4
#define ARDS_TYPE_BAD 5


// 全局内存信息
memory_info_t mem_info = {0};

// ARDS缓冲区（在loader中检测的内存信息会存储在这里）
// 假设最多256个ARDS条目，每个20字节
#define MAX_ARDS 256
#define ARDS_BUFFER_ADDR 0x5000  // 在loader中检测后存储的位置

// 从loader传递的内存信息（在kern.c中定义）
extern uint32_t loader_ards_count;
extern uint32_t loader_ards_buffer[];

// 计算内存大小（KB）
static uint32_t calculate_memory_kb(uint32_t low, uint32_t high) {
    uint64_t memory = ((uint64_t)high << 32) | low;
    return (uint32_t)(memory / 1024);
}

// 检测内存（在loader中已经检测，这里只是解析）
void memory_detect(void) {
    ards_t *ards = (ards_t *)loader_ards_buffer;
    uint32_t count = loader_ards_count;
    
    if (count == 0 || count > MAX_ARDS) {
        count = 0;
    }
    
    mem_info.ards_count = count;
    mem_info.total_memory = 0;
    mem_info.available_memory = 0;
    
    for (uint32_t i = 0; i < count; i++) {
        uint32_t base_low = ards[i].base_low;
        uint32_t base_high = ards[i].base_high;
        uint32_t length_low = ards[i].length_low;
        uint32_t length_high = ards[i].length_high;
        uint32_t type = ards[i].type;
        
        uint64_t base = ((uint64_t)base_high << 32) | base_low;
        uint64_t length = ((uint64_t)length_high << 32) | length_low;
        uint32_t length_kb = (uint32_t)(length / 1024);
        
        // 计算总内存
        uint64_t end = base + length;
        if (end > mem_info.total_memory) {
            mem_info.total_memory = (uint32_t)(end / 1024);
        }
        
        // 计算可用内存
        if (type == ARDS_TYPE_AVAILABLE) {
            mem_info.available_memory += length_kb;
        }
    }
}

// 获取内存信息
memory_info_t *get_memory_info(void) {
    return &mem_info;
}

// 显示内存信息
void print_memory_info(void) {
    printf("Memory Information:\n");
    printf("  Total Memory: %u KB (%u MB)\n", 
           mem_info.total_memory, 
           mem_info.total_memory / 1024);
    printf("  Available Memory: %u KB (%u MB)\n", 
           mem_info.available_memory, 
           mem_info.available_memory / 1024);
    printf("  ARDS Count: %u\n", mem_info.ards_count);
}

