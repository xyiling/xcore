
; GDT 描述符结构
%macro Descriptor 3
    dw %2 & 0xFFFF      ; 段界限低16位
    dw %1 & 0xFFFF      ; 段基址低16位
    db (%1 >> 16) & 0xFF ; 段基址中间8位
    dw ((%2 >> 8) & 0xF00) | (%3 & 0xF0FF) ; 属性 + 段界限高4位
    db (%1 >> 24) & 0xFF ; 段基址高8位
%endmacro

; 段属性
DA_32 equ 0x4000        ; 32位段
DA_CR equ 0x9A          ; 可执行、可读代码段
DA_DRW equ 0x92         ; 可读写数据段