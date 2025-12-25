#include "../include/type.h"
#include "../include/stdio.h"
#include "../include/string.h"

// 中断描述符表项结构
typedef struct {
    uint16_t offset_low;    // 偏移量低16位
    uint16_t selector;      // 段选择子
    uint8_t zero;           // 保留，必须为0
    uint8_t type_attr;      // 类型和属性
    uint16_t offset_high;   // 偏移量高16位
} __attribute__((packed)) idt_entry_t;

// IDT指针结构
typedef struct {
    uint16_t limit;         // IDT大小-1
    uint32_t base;          // IDT基地址
} __attribute__((packed)) idt_ptr_t;

// IDT表（256个中断）
#define IDT_SIZE 256
idt_entry_t idt[IDT_SIZE];
idt_ptr_t idt_ptr;

// 中断处理函数声明（在interrupt.asm中定义）
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

// IRQ处理函数声明
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

// 中断处理函数指针数组
typedef void (*interrupt_handler_t)(void);
interrupt_handler_t interrupt_handlers[IDT_SIZE];

// 设置IDT条目
static void set_idt_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = base & 0xFFFF;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

// 通用中断处理函数（由汇编代码调用）
void interrupt_handler(uint32_t int_no) {
    if (int_no < IDT_SIZE && interrupt_handlers[int_no]) {
        interrupt_handlers[int_no]();
    } else if (int_no < 32) {
        // 异常处理
        printf("Exception %u occurred!\n", int_no);
    }
    // IRQ需要发送EOI
    if (int_no >= 32 && int_no < 48) {
        extern void pic_send_eoi(uint8_t irq);
        pic_send_eoi((uint8_t)(int_no - 32));
    }
}

// 注册中断处理函数
void register_interrupt_handler(uint8_t int_no, interrupt_handler_t handler) {
    interrupt_handlers[int_no] = handler;
}

// 初始化IDT
void init_idt(void) {
    // 初始化IDT指针
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_SIZE - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    // 清零IDT
    memset(idt, 0, sizeof(idt));
    memset(interrupt_handlers, 0, sizeof(interrupt_handlers));
    
    // 设置异常处理（0-31）
    uint8_t code_selector = 0x08; // 代码段选择子
    uint8_t interrupt_gate = 0x8E; // 32位中断门
    
    set_idt_entry(0, (uint32_t)isr0, code_selector, interrupt_gate);
    set_idt_entry(1, (uint32_t)isr1, code_selector, interrupt_gate);
    set_idt_entry(2, (uint32_t)isr2, code_selector, interrupt_gate);
    set_idt_entry(3, (uint32_t)isr3, code_selector, interrupt_gate);
    set_idt_entry(4, (uint32_t)isr4, code_selector, interrupt_gate);
    set_idt_entry(5, (uint32_t)isr5, code_selector, interrupt_gate);
    set_idt_entry(6, (uint32_t)isr6, code_selector, interrupt_gate);
    set_idt_entry(7, (uint32_t)isr7, code_selector, interrupt_gate);
    set_idt_entry(8, (uint32_t)isr8, code_selector, interrupt_gate);
    set_idt_entry(9, (uint32_t)isr9, code_selector, interrupt_gate);
    set_idt_entry(10, (uint32_t)isr10, code_selector, interrupt_gate);
    set_idt_entry(11, (uint32_t)isr11, code_selector, interrupt_gate);
    set_idt_entry(12, (uint32_t)isr12, code_selector, interrupt_gate);
    set_idt_entry(13, (uint32_t)isr13, code_selector, interrupt_gate);
    set_idt_entry(14, (uint32_t)isr14, code_selector, interrupt_gate);
    set_idt_entry(15, (uint32_t)isr15, code_selector, interrupt_gate);
    set_idt_entry(16, (uint32_t)isr16, code_selector, interrupt_gate);
    set_idt_entry(17, (uint32_t)isr17, code_selector, interrupt_gate);
    set_idt_entry(18, (uint32_t)isr18, code_selector, interrupt_gate);
    set_idt_entry(19, (uint32_t)isr19, code_selector, interrupt_gate);
    set_idt_entry(20, (uint32_t)isr20, code_selector, interrupt_gate);
    set_idt_entry(21, (uint32_t)isr21, code_selector, interrupt_gate);
    set_idt_entry(22, (uint32_t)isr22, code_selector, interrupt_gate);
    set_idt_entry(23, (uint32_t)isr23, code_selector, interrupt_gate);
    set_idt_entry(24, (uint32_t)isr24, code_selector, interrupt_gate);
    set_idt_entry(25, (uint32_t)isr25, code_selector, interrupt_gate);
    set_idt_entry(26, (uint32_t)isr26, code_selector, interrupt_gate);
    set_idt_entry(27, (uint32_t)isr27, code_selector, interrupt_gate);
    set_idt_entry(28, (uint32_t)isr28, code_selector, interrupt_gate);
    set_idt_entry(29, (uint32_t)isr29, code_selector, interrupt_gate);
    set_idt_entry(30, (uint32_t)isr30, code_selector, interrupt_gate);
    set_idt_entry(31, (uint32_t)isr31, code_selector, interrupt_gate);
    
    // 设置IRQ处理（32-47）
    set_idt_entry(32, (uint32_t)irq0, code_selector, interrupt_gate);
    set_idt_entry(33, (uint32_t)irq1, code_selector, interrupt_gate);
    set_idt_entry(34, (uint32_t)irq2, code_selector, interrupt_gate);
    set_idt_entry(35, (uint32_t)irq3, code_selector, interrupt_gate);
    set_idt_entry(36, (uint32_t)irq4, code_selector, interrupt_gate);
    set_idt_entry(37, (uint32_t)irq5, code_selector, interrupt_gate);
    set_idt_entry(38, (uint32_t)irq6, code_selector, interrupt_gate);
    set_idt_entry(39, (uint32_t)irq7, code_selector, interrupt_gate);
    set_idt_entry(40, (uint32_t)irq8, code_selector, interrupt_gate);
    set_idt_entry(41, (uint32_t)irq9, code_selector, interrupt_gate);
    set_idt_entry(42, (uint32_t)irq10, code_selector, interrupt_gate);
    set_idt_entry(43, (uint32_t)irq11, code_selector, interrupt_gate);
    set_idt_entry(44, (uint32_t)irq12, code_selector, interrupt_gate);
    set_idt_entry(45, (uint32_t)irq13, code_selector, interrupt_gate);
    set_idt_entry(46, (uint32_t)irq14, code_selector, interrupt_gate);
    set_idt_entry(47, (uint32_t)irq15, code_selector, interrupt_gate);
    
    // 加载IDT
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

// 启用中断
void enable_interrupts(void) {
    asm volatile("sti");
}

// 禁用中断
void disable_interrupts(void) {
    asm volatile("cli");
}

