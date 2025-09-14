void ioe_init(void); // 声明外部函数

// 简单的内核入口点
void kmain (void)
{
    const char* msg = "kernel c";
    unsigned short* video_memory = (unsigned short*)0xB8000;
    volatile int i = 0;
    
    for (i = 0; i < 80 * 25; i++) {
        video_memory[i] = (unsigned short) 0x0720;
    }
    video_memory[0] = (unsigned short)(0x0c00 | msg[0]);
    video_memory[1] = (unsigned short)(0x0700 | msg[1]);
    video_memory[2] = (unsigned short)(0x0700 | msg[2]);
    video_memory[3] = (unsigned short)(0x0700 | msg[3]);
    // video_memory[4] = (unsigned short)(0x0700 | msg[4]);
    // video_memory[5] = (unsigned short)(0x0700 | msg[5]);
    // video_memory[6] = (unsigned short)(0x0700 | msg[6]);
    // video_memory[7] = (unsigned short)(0x0700 | msg[7]);
    // video_memory[8] = (unsigned short)(0x0700 | msg[8]);
    // video_memory[9] = (unsigned short)(0x0700 | msg[9]);
    
    __asm__ __volatile__ (
        "hlt"
    );
    
    // 无限循环，防止返回
    // for (; ;) {
    //     volatile int j = 0;
    //     for (j = 0; j < 1000000; j++); //
    // }
    
}

void ioe_init(void)
{
    // 初始化输入输出设备的代码
    
}