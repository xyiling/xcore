#include "include/type.h"
#include "include/stdio.h"
#include "include/string.h"
#include "kernel/memory.h"
#include "kernel/interrupt.h"
#include "kernel/pic.h"
#include "kernel/syscall.h"
#include "kernel/task.h"
#include "kernel/fs.h"
#include "kernel/shell.h"

// 从loader传递的内存信息（在loader.asm中定义）
uint32_t loader_ards_count __attribute__((section(".data"))) = 0;
uint32_t loader_ards_buffer[256 * 5] __attribute__((section(".data"))) = {0};

// 键盘扫描码到ASCII的简单映射
static char scan_code_to_ascii(uint8_t scan_code) {
    // 简化的扫描码映射（只处理部分按键）
    if (scan_code >= 0x02 && scan_code <= 0x0B) {
        return "1234567890"[scan_code - 0x02];
    } else if (scan_code >= 0x10 && scan_code <= 0x19) {
        return "qwertyuiop"[scan_code - 0x10];
    } else if (scan_code >= 0x1E && scan_code <= 0x26) {
        return "asdfghjkl"[scan_code - 0x1E];
    } else if (scan_code >= 0x2C && scan_code <= 0x32) {
        return "zxcvbnm"[scan_code - 0x2C];
    } else if (scan_code == 0x1C) {
        return '\n';
    } else if (scan_code == 0x0E) {
        return '\b';
    } else if (scan_code == 0x39) {
        return ' ';
    }
    return '\0';
}

// 键盘中断处理
static void keyboard_handler(void) {
    // 从键盘端口读取扫描码
    uint8_t scan_code;
    asm volatile("inb %1, %0" : "=a"(scan_code) : "Nd"((uint16_t)0x60));
    
    // 只处理按键按下（扫描码最高位为0）
    if (!(scan_code & 0x80)) {
        char c = scan_code_to_ascii(scan_code);
        if (c != '\0') {
            extern void keyboard_putchar(char c);
            keyboard_putchar(c);
        }
    }
    
    // 发送EOI
    extern void pic_send_eoi(uint8_t irq);
    pic_send_eoi(1);
}

// 定时器中断处理
static void timer_handler(void) {
    static uint32_t tick = 0;
    tick++;
    // 每100个tick可以触发一次任务调度
    if (tick % 100 == 0) {
        // schedule();
    }
}

// 初始化中断处理函数
static void init_interrupt_handlers(void) {
    // 注册键盘中断（IRQ1 -> 中断33）
    register_interrupt_handler(33, keyboard_handler);
    
    // 注册定时器中断（IRQ0 -> 中断32）
    register_interrupt_handler(32, timer_handler);
}

// 内核主函数
void kmain(void) {
    // 清屏
    clear_screen();
    
    // 显示启动信息
    puts("========================================\n");
    puts("  xcore Operating System v0.1.0\n");
    puts("========================================\n\n");
    
    // 初始化内存检测
    // 从loader传递的信息中读取ARDS数量
    // 注意：这里需要从0x4FFC读取ARDS数量
    uint32_t *ards_count_ptr = (uint32_t *)0x4FFC;
    loader_ards_count = *ards_count_ptr;
    
    // 复制ARDS数据
    if (loader_ards_count > 0 && loader_ards_count <= 256) {
        // ARDS结构是20字节，直接复制
        void *source = (void *)0x5000;
        void *dest = (void *)loader_ards_buffer;
        memcpy(dest, source, loader_ards_count * 20);
    }
    
    memory_detect();
    printf("Memory detection completed.\n");
    print_memory_info();
    printf("\n");
    
    // 初始化PIC
    init_pic();
    printf("PIC initialized.\n");
    
    // 初始化IDT
    init_idt();
    printf("IDT initialized.\n");
    
    // 初始化中断处理函数
    init_interrupt_handlers();
    printf("Interrupt handlers registered.\n");
    
    // 初始化系统调用
    init_syscall();
    printf("System calls initialized.\n");
    
    // 初始化任务系统
    init_task_system();
    printf("Task system initialized.\n");
    
    // 初始化文件系统
    init_filesystem();
    printf("Filesystem initialized.\n");
    
    printf("\n");
    printf("System initialization completed.\n");
    printf("Starting shell...\n\n");
    
    // 启用中断
    enable_interrupts();
    
    // 启动Shell
    shell_main();
    
    // 如果shell返回（不应该发生），进入无限循环
    for (;;) {
        asm volatile("hlt");
    }
}
