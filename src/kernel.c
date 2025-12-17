// 简单的内核入口点
void 
kmain(void)
{
    const char *msg = "kernel c";
    unsigned short *video_memory = (unsigned short *)0xB8000;
    volatile int i = 0;
    int attr = 0x0c >> 8; // 红色字符，黑色背景
    while (msg[i] != '\0') {
        video_memory[i] = (unsigned short)(attr | msg[i]);
        i++;
    }

    // 无限循环，防止返回
    for (;;) {}
}