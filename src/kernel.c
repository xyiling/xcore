// 简单的内核入口点
void kmain (void)
{
    const char* msg = "kernel c";
    unsigned short* video_memory = (unsigned short*)0xB8000;
    volatile int i = 0;
    
    for (i = 0; i < 80 * 25; i++) {
        video_memory[i] = (unsigned short) 0x0720; // 黑底白字的空格
    }
    i = 1;
    video_memory[i] = (unsigned short)(0x0c00 | msg[i]); // 黑底白字

    while (msg [i] != '\0') {
        i++;
    }
    
    // 无限循环，防止返回
    // for (; ;) {
    //     volatile int j = 0;
    //     for (j = 0; j < 1000000; j++); //
    // }
    
}