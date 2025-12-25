#include "../include/stdlib.h"
#include "../include/type.h"
#include "../include/string.h"

// 简单的内存管理（固定大小分配）
#define HEAP_SIZE 1024 * 1024  // 1MB堆
static uint8_t heap[HEAP_SIZE];
static uint32_t heap_ptr = 0;

void *malloc(size_t size) {
    if (heap_ptr + size > HEAP_SIZE) {
        return NULL;
    }
    void *ptr = &heap[heap_ptr];
    heap_ptr += size;
    return ptr;
}

void free(void *ptr) {
    (void)ptr;
    // 简单的实现：不实际释放内存
}

void *calloc(size_t num, size_t size) {
    void *ptr = malloc(num * size);
    if (ptr) {
        memset(ptr, 0, num * size);
    }
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    (void)ptr;
    return malloc(size);
}

void halt(void) {
    asm volatile("cli");
    asm volatile("hlt");
    for (;;);
}

void reboot(void) {
    // 触发CPU重启
    asm volatile("outb %0, %1" : : "a"((uint8_t)0xFE), "Nd"((uint16_t)0x64));
    for (;;);
}

void shutdown(void) {
    // 触发ACPI关机
    asm volatile("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0xB004));
    for (;;);
}

