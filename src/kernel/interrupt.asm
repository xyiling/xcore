; interrupt.asm - 中断处理函数汇编部分

[bits 32]
section .text

; 外部函数声明
extern interrupt_handler

; 宏：定义ISR（异常处理）
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push 0          ; 错误代码占位符
    push %1         ; 中断号
    jmp isr_common_stub
%endmacro

; 宏：定义带错误代码的ISR
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push %1         ; 中断号（错误代码已经由CPU压入）
    jmp isr_common_stub
%endmacro

; 定义所有ISR
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; 通用ISR处理
isr_common_stub:
    pusha           ; 保存所有通用寄存器
    
    mov ax, ds
    push eax        ; 保存数据段选择子
    
    mov ax, 0x10    ; 加载内核数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 传递中断号（在栈中，esp+36的位置）
    mov eax, [esp + 36]
    push eax
    call interrupt_handler
    add esp, 4      ; 清理参数
    
    pop eax         ; 恢复数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa            ; 恢复所有通用寄存器
    add esp, 8      ; 清理错误代码和中断号
    iret            ; 返回

; 宏：定义IRQ处理
%macro IRQ 2
global irq%1
irq%1:
    cli
    push 0          ; 错误代码占位符
    push %2         ; IRQ号（32+%1）
    jmp irq_common_stub
%endmacro

; 定义所有IRQ
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; 通用IRQ处理
irq_common_stub:
    pusha           ; 保存所有通用寄存器
    
    mov ax, ds
    push eax        ; 保存数据段选择子
    
    mov ax, 0x10    ; 加载内核数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 传递中断号（在栈中，esp+36的位置）
    mov eax, [esp + 36]
    push eax
    call interrupt_handler
    add esp, 4      ; 清理参数
    
    pop eax         ; 恢复数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa            ; 恢复所有通用寄存器
    add esp, 8      ; 清理错误代码和IRQ号
    iret            ; 返回

