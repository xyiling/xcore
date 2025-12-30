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

// 内联端口I/O函数
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// 简化的扫描码到ASCII转换
char scancode_to_ascii(uint8_t scancode) {
    static const char scancode_table[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
    };
    
    if (scancode < sizeof(scancode_table)) {
        return scancode_table[scancode];
    }
    return 0;
}

// 键盘中断处理函数
void keyboard_interrupt_handler(void) {
    uint8_t scancode = inb(0x60);
    
    // 只处理按键按下事件（忽略释放事件）
    if (scancode < 128) {
        char ascii = scancode_to_ascii(scancode);
        if (ascii != 0) {
            keyboard_putchar(ascii);
        }
    }
    
    // 发送EOI
    extern void pic_send_eoi(uint8_t irq);
    pic_send_eoi(1);
}

// 从loader传递的内存信息（在loader.asm中定义）
uint32_t loader_ards_count __attribute__((section(".data"))) = 0;
uint32_t loader_ards_buffer[256 * 5] __attribute__((section(".data"))) = {0};


// 内核主函数
void kmain(void) {
    // 清屏
    clear_screen();
    
    // 显示启动信息
    puts("========================================\n");
    puts("  xcore Operating System v0.1.0\n");
    puts("========================================\n\n");
    
    puts("Kernel started successfully!\n");
    puts("Testing keyboard input...\n\n");
    
    // 初始化PIC
    init_pic();
    puts("PIC initialized.\n");
    
    // 初始化IDT
    init_idt();
    puts("IDT initialized.\n");
    
    // 注册键盘中断处理函数
    register_interrupt_handler(33, keyboard_interrupt_handler);
    puts("Keyboard handler registered.\n");
    
    // 启用中断
    enable_interrupts();
    puts("Interrupts enabled.\n\n");
    
    puts("Starting shell...\n\n");
    
    // 启动Shell
    shell_main();
    
    // 如果shell返回（不应该发生），进入无限循环
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
