; init.asm - 32位保护模式内核入口
[bits 32]
section .text
global _start
extern kmain

_start:
    ; 32位保护模式入口点，由loader.asm的protected_mode_entry跳转到这里
    mov esp, 0x8000  ; 设置堆栈指针在0x8000，避免覆盖从0x9000开始的内核代码
    call kmain
.hlt:
    hlt
    jmp .hlt
