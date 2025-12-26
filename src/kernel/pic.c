#include "../include/type.h"

// PIC端口定义
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// PIC初始化命令
#define ICW1_ICW4 0x01      // ICW4需要
#define ICW1_SINGLE 0x02    // 单级模式
#define ICW1_INTERVAL4 0x04 // 调用地址间隔4
#define ICW1_LEVEL 0x08     // 电平触发模式
#define ICW1_INIT 0x10      // 初始化

#define ICW4_8086 0x01       // 8086/88模式
#define ICW4_AUTO 0x02       // 自动EOI
#define ICW4_BUF_SLAVE 0x08  // 缓冲模式/从
#define ICW4_BUF_MASTER 0x0C // 缓冲模式/主
#define ICW4_SFNM 0x10       // 特殊全嵌套模式

// EOI命令
#define PIC_EOI 0x20

// 端口I/O函数原型（在文件后部定义）
static inline uint8_t inb(uint16_t port);
static inline void outb(uint16_t port, uint8_t val);

// 初始化PIC（可编程中断控制器）
void init_pic(void)
{
    // 保存中断掩码
    uint8_t a1, a2;
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    // 开始初始化序列
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // ICW2: 设置中断向量偏移
    outb(PIC1_DATA, 0x20); // 主PIC从0x20开始
    outb(PIC2_DATA, 0x28); // 从PIC从0x28开始

    // ICW3: 设置级联
    outb(PIC1_DATA, 0x04); // 主PIC的IRQ2连接到从PIC
    outb(PIC2_DATA, 0x02); // 从PIC连接到主PIC的IRQ2

    // ICW4: 设置模式
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // 恢复中断掩码
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

// 发送EOI（中断结束）信号
void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
    {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

// 端口I/O函数实现
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}
