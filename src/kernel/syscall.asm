; syscall.asm - 系统调用入口

[bits 32]
section .text

extern handle_syscall

; 系统调用入口（通过INT 0x80）
global syscall_entry
syscall_entry:
    pusha           ; 保存所有寄存器
    
    push edx        ; arg4
    push ecx        ; arg3
    push ebx        ; arg2
    push eax        ; arg1 (syscall_no)
    
    call handle_syscall
    add esp, 16     ; 清理参数
    
    mov [.ret_val], eax
    popa
    mov eax, [.ret_val]
    iret

.ret_val: dd 0

