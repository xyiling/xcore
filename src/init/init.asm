; kernel.asm
[bits 32]
section .magic
global magic
magic:
    dd 0xBEAF       ; 魔数标识

section .text
global _start
extern kmain

_start:
    ; 设置堆栈
    mov esp, 0x9000
    call kmain
    cli
    hlt
    jmp $