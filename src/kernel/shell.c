#include "../include/type.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "memory.h"
#include "task.h"
#include "fs.h"

// 命令结构
typedef struct
{
    const char *name;
    const char *description;
    int (*handler)(int argc, char **argv);
} command_t;

// 命令处理函数声明
static int cmd_help(int argc, char **argv);
static int cmd_info(int argc, char **argv);
static int cmd_meminfo(int argc, char **argv);
static int cmd_ls(int argc, char **argv);
static int cmd_echo(int argc, char **argv);
static int cmd_ps(int argc, char **argv);
static int cmd_reboot(int argc, char **argv);
static int cmd_shutdown(int argc, char **argv);
static int cmd_clear(int argc, char **argv);

// 命令表
static command_t commands[] = {
    {"help", "显示帮助信息", cmd_help},
    {"info", "显示系统信息", cmd_info},
    {"meminfo", "显示内存信息", cmd_meminfo},
    {"ls", "列出目录", cmd_ls},
    {"echo", "回显参数", cmd_echo},
    {"ps", "显示进程列表", cmd_ps},
    {"reboot", "重启系统", cmd_reboot},
    {"shutdown", "关机", cmd_shutdown},
    {"clear", "清屏", cmd_clear},
    {NULL, NULL, NULL}};

// 命令缓冲区
#define MAX_CMD_LEN 256
static char cmd_buffer[MAX_CMD_LEN];

// 命令处理函数实现
static int cmd_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Available commands:\n");
    for (int i = 0; commands[i].name; i++)
    {
        printf("  %s - %s\n", commands[i].name, commands[i].description);
    }
    return 0;
}

static int cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("=== System Information ===\n");
    printf("OS: xcore\n");
    printf("Version: 0.1.0\n");
    printf("Architecture: x86 (32-bit)\n");
    memory_info_t *mem = get_memory_info();
    printf("Total Memory: %u KB (%u MB)\n",
           mem->total_memory,
           mem->total_memory / 1024);
    printf("Available Memory: %u KB (%u MB)\n",
           mem->available_memory,
           mem->available_memory / 1024);
    return 0;
}

static int cmd_meminfo(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    print_memory_info();
    return 0;
}

static int cmd_ls(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Directory listing:\n");
    printf("  / (root)\n");
    return 0;
}

static int cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (i > 1) printf(" ");
        printf("%s", argv[i]);
    }
    printf("\n");
    return 0;
}

static int cmd_ps(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Process list:\n");
    printf("PID   Name       State\n");
    printf("1     kernel     Running\n");
    printf("2     shell      Running\n");
    return 0;
}

static int cmd_reboot(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Rebooting system...\n");
    // 触发CPU重启
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0xFE), "Nd"((uint16_t)0x64));
    return 0;
}

static int cmd_shutdown(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Shutting down system...\n");
    // 触发ACPI关机
    __asm__ __volatile__("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0xB004));
    return 0;
}

static int cmd_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    clear_screen();
    return 0;
}

// 执行命令
static int execute_command(char *line)
{
    if (!line || strlen(line) == 0)
    {
        return 0;
    }

    // 解析命令和参数
    char *argv[32];
    int argc = 0;
    char *token = line;

    // 跳过前导空格
    while (*token == ' ')
        token++;

    // 分割命令和参数
    while (*token && argc < 31)
    {
        argv[argc++] = token;
        while (*token && *token != ' ')
            token++;
        if (*token)
        {
            *token++ = '\0';
            while (*token == ' ')
                token++;
        }
    }
    argv[argc] = NULL;

    if (argc == 0)
    {
        return 0;
    }

    // 查找并执行命令
    for (int i = 0; commands[i].name; i++)
    {
        if (strcmp(argv[0], commands[i].name) == 0)
        {
            return commands[i].handler(argc, argv);
        }
    }

    printf("Command not found: %s\n", argv[0]);
    printf("Type 'help' for available commands.\n");
    return -1;
}

// 读取一行输入（简化实现，从键盘缓冲区读取）
static void read_line(char *buffer, uint32_t size)
{
    uint32_t pos = 0;
    buffer[0] = '\0';

    while (pos < size - 1)
    {
        // 简单的键盘输入处理（需要键盘中断支持）
        // 这里使用一个简化的实现
        char c = getchar();

        if (c == '\n' || c == '\r')
        {
            buffer[pos] = '\0';
            putchar('\n');
            return;
        }
        else if (c == '\b' || c == 127)
        {
            if (pos > 0)
            {
                pos--;
                buffer[pos] = '\0';
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        }
        else if (c >= 32 && c < 127)
        {
            buffer[pos++] = c;
            buffer[pos] = '\0';
            putchar(c);
        }
    }
    buffer[size - 1] = '\0';
}

// Shell主循环
void shell_main(void)
{
    printf("xcore shell v0.1.0\n");
    printf("Type 'help' for available commands.\n");

    while (1)
    {
        printf("xcore> ");

        read_line(cmd_buffer, MAX_CMD_LEN);

        if (strlen(cmd_buffer) > 0)
        {
            execute_command(cmd_buffer);
        }
    }
}
