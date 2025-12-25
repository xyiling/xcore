/* os.ld */
ENTRY(_start)

SECTIONS
{
    . = 0x9000;     /* 内核加载地址 */
    
    .text : {
        *(.text)    /* 代码段 */
    }
    
    .rodata : {
        *(.rodata)  /* 只读数据 */
    }
    
    .data : {
        *(.data)    /* 初始化数据 */
    }
    
    .bss : {
        *(.bss)     /* BSS段 */
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.note)
        *(.eh_frame)
        *(.eh_*)
    }
}

