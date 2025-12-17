; kernel.asm
section .text
global _start
extern kmain
_start:
    ; 设置堆栈
    mov esp, 0x9000
    call kmain
.hlt:
    hlt