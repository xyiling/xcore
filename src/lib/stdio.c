#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/stdarg.h"

static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t text_color = 0x0C; // 红色字符，黑色背景

// VGA光标控制端口
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5

// 内联端口I/O函数
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// 更新硬件光标位置
static void update_hardware_cursor(void) {
    uint16_t position = cursor_row * SCREEN_WIDTH + cursor_col;
    
    // 设置光标位置低8位
    outb(VGA_CRTC_INDEX, 0x0F);
    outb(VGA_CRTC_DATA, position & 0xFF);
    
    // 设置光标位置高8位
    outb(VGA_CRTC_INDEX, 0x0E);
    outb(VGA_CRTC_DATA, (position >> 8) & 0xFF);
}

void putchar(char c)
{
    if (c == '\n')
    {
        cursor_row++;
        cursor_col = 0;
    }
    else if (c == '\r')
    {
        cursor_col = 0;
    }
    else if (c == '\t')
    {
        cursor_col = (cursor_col + 4) & ~3;
    }
    else if (c == '\b')
    {
        if (cursor_col > 0)
        {
            cursor_col--;
        }
    }
    else
    {
        uint16_t *video = (uint16_t *)(VIDEO_MEMORY + (cursor_row * SCREEN_WIDTH + cursor_col) * 2);
        *video = (text_color << 8) | c;
        cursor_col++;
    }

    if (cursor_col >= SCREEN_WIDTH)
    {
        cursor_col = 0;
        cursor_row++;
    }

    if (cursor_row >= SCREEN_HEIGHT)
    {
        cursor_row = SCREEN_HEIGHT - 1;
        // 简单的滚动：向上移动一行
        uint16_t *video = (uint16_t *)VIDEO_MEMORY;
        memmove(video, video + SCREEN_WIDTH, (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2);
        memset(video + (SCREEN_HEIGHT - 1) * SCREEN_WIDTH, 0, SCREEN_WIDTH * 2);
    }

    // 更新硬件光标位置
    update_hardware_cursor();
}

void puts(const char *str)
{
    while (*str)
    {
        putchar(*str++);
    }
}

void clear_screen(void)
{
    uint16_t *video = (uint16_t *)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        video[i] = (text_color << 8) | ' ';
    }
    cursor_row = 0;
    cursor_col = 0;
    update_hardware_cursor();
}

void set_cursor(int row, int col)
{
    cursor_row = row;
    cursor_col = col;
    update_hardware_cursor();
}

int get_cursor_row(void)
{
    return cursor_row;
}

int get_cursor_col(void)
{
    return cursor_col;
}

static void print_number(uint32_t num, int base)
{
    char buf[32];
    int i = 0;

    if (num == 0)
    {
        putchar('0');
        return;
    }

    while (num > 0)
    {
        int digit = num % base;
        buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        num /= base;
    }

    while (i > 0)
    {
        putchar(buf[--i]);
    }
}

void printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            switch (*format)
            {
            case 'd':
            case 'i':
            {
                int32_t num = va_arg(args, int32_t);
                if (num < 0)
                {
                    putchar('-');
                    num = -num;
                }
                print_number((uint32_t)num, 10);
                break;
            }
            case 'u':
            {
                uint32_t num = va_arg(args, uint32_t);
                print_number(num, 10);
                break;
            }
            case 'x':
            case 'X':
            {
                uint32_t num = va_arg(args, uint32_t);
                print_number(num, 16);
                break;
            }
            case 'c':
            {
                char c = (char)va_arg(args, int);
                putchar(c);
                break;
            }
            case 's':
            {
                char *str = va_arg(args, char *);
                puts(str);
                break;
            }
            case '%':
            {
                putchar('%');
                break;
            }
            default:
                putchar('%');
                putchar(*format);
                break;
            }
        }
        else
        {
            putchar(*format);
        }
        format++;
    }

    va_end(args);
}

// 键盘缓冲区
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint32_t keyboard_read_pos = 0;
static uint32_t keyboard_write_pos = 0;
static volatile uint32_t keyboard_count = 0;

// 简单的getchar实现（需要键盘中断支持）
char getchar(void)
{
    // 等待键盘输入
    while (keyboard_count == 0)
    {
        __asm__ __volatile__("hlt");
    }

    // 从缓冲区读取
    char c = keyboard_buffer[keyboard_read_pos];
    keyboard_read_pos = (keyboard_read_pos + 1) % KEYBOARD_BUFFER_SIZE;
    keyboard_count--;

    return c;
}

// 将字符放入键盘缓冲区（由键盘中断处理函数调用）
void keyboard_putchar(char c)
{
    if (keyboard_count < KEYBOARD_BUFFER_SIZE)
    {
        keyboard_buffer[keyboard_write_pos] = c;
        keyboard_write_pos = (keyboard_write_pos + 1) % KEYBOARD_BUFFER_SIZE;
        keyboard_count++;
    }
}
