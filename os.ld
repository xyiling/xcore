/* scripts/kernel.ld */
ENTRY(_start)

SECTIONS
{
    . = 0x9000;     /* 内核加载地址 */
    
    .magic : {
        *(.magic)   /* 魔数段 */
    }
    .rodata : {
        *(.rodata)
    }
    .text : {
        *(.text)    /* 代码段 */
    }
    
    .data : {
        *(.data)    /* 数据段 */
    }
    
    .bss : {
        *(.bss)     /* BSS段 */
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.note)
    }
}