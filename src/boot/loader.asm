%include "boot.inc"

[bits 16]
section loader vstart=LOADER_BASE_ADDR
jmp _start

LOADER_BASE_ADDR equ 0x900
KERNEL_BASE_ADDR equ 0x9000
LABEL_GDT:          Descriptor 0,       0,          0
LABEL_DESC_CODE:    Descriptor 0,       0xFFFFF,    DA_CR | DA_32
LABEL_DESC_DATA:    Descriptor 0,       0xFFFFF,    DA_DRW | DA_32
LABEL_DESC_VIDEO:   Descriptor 0xB8000, 0xFFFF,     DA_DRW

GDT_LEN equ $ - LABEL_GDT

; 选择子
SelectorCode  equ LABEL_DESC_CODE  - LABEL_GDT
SelectorData  equ LABEL_DESC_DATA  - LABEL_GDT
SelectorVideo equ LABEL_DESC_VIDEO - LABEL_GDT

; GDT指针
GdtPtr:
    dw GDT_LEN - 1
    dd 0 ; 基地址会在setup_gdt中设置

; ARDS缓冲区地址（传递给内核）
ARDS_BUFFER equ 0x5000
ARDS_COUNT_ADDR equ 0x4FFC

; 主程序
_start:
    ; 设置段寄存器
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x900
    
    mov si, msg_loading
    call print
    
    ; 检测内存
    call detect_memory
    
    mov si, msg_memory_detected
    call print
    
    ; 加载内核到内存
    mov ebx, 2          ; 从第3个扇区开始（MBR通常在第0扇区）
    mov cx, 32          ; 读取32个扇区（16KB）
    mov di, KERNEL_BASE_ADDR
    call read_disk

    call gdt_init
    call enter_protected_mode
    jmp $

; 设置GDT（放在代码后面避免覆盖）
gdt_init:
    ; 使用平坦内存模型：代码段和数据段基地址都为0
    mov eax, 0
    mov [LABEL_DESC_CODE + 2], ax
    shr eax, 16
    mov [LABEL_DESC_CODE + 4], al
    mov [LABEL_DESC_CODE + 7], ah
    
    mov eax, 0
    mov [LABEL_DESC_DATA + 2], ax
    shr eax, 16
    mov [LABEL_DESC_DATA + 4], al
    mov [LABEL_DESC_DATA + 7], ah
    
    ; GdtPtr需要包含GDT的线性地址 (段基址+偏移)
    mov eax, cs
    shl eax, 4
    add eax, LABEL_GDT
    mov dword [GdtPtr + 2], eax
    ret

; 进入保护模式
enter_protected_mode:
    mov si, msg_protected_mode
    call print
    cli
    lgdt [GdtPtr]
    
    ; 开启A20线
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; 设置CR0.PE
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; 远跳转刷新流水线并进入保护模式
    jmp dword SelectorCode:protected_mode_entry

; 打印字符串 (SI=字符串地址)
print:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

[bits 32]
protected_mode_entry:
    ; 设置保护模式下的段寄存器
    mov ax, SelectorData
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9000
    
    ; 将内存检测信息传递给内核
    ; ARDS_COUNT_ADDR (0x4FFC) 存储ARDS数量
    ; ARDS_BUFFER (0x5000) 存储ARDS数据
    
    ; 跳转到内核
    jmp dword SelectorCode:0x9000

[bits 16]
; 读取磁盘扇区
; EBX=起始LBA, CX=扇区数, DI=目标内存地址
read_disk:
    ; 设置扇区数
    mov dx, 0x1F2
    mov al, cl
    out dx, al
    
    ; 设置LBA地址
    mov dx, 0x1F3
    mov al, bl
    out dx, al
    
    mov dx, 0x1F4
    mov al, bh
    out dx, al
    
    mov dx, 0x1F5
    mov eax, ebx
    shr eax, 16
    mov al, al
    out dx, al
    
    mov dx, 0x1F6
    mov al, 0xE0 ; LBA模式
    out dx, al
    
    ; 发送读命令
    mov dx, 0x1F7
    mov al, 0x20
    out dx, al
    
.wait:
    in al, dx
    test al, 0x80 ; 检查忙位
    jnz .wait
    test al, 0x08 ; 检查就绪位
    jz .wait
    
    ; 计算读取字数 (扇区数 * 256)
    mov ax, cx
    shl ax, 8
    mov cx, ax
    
    ; 读取数据
    mov dx, 0x1F0
    mov bx, di
.read_loop:
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .read_loop
    ret


; 内存检测函数
detect_memory:
    xor ebx, ebx        ; ebx必须初始化为0
    mov edi, ARDS_BUFFER ; ARDS缓冲区地址
    mov edx, 0x534d4150  ; 签名 "SMAP"
    mov dword [ARDS_COUNT_ADDR], 0  ; 初始化计数器
    
.next_ards:
    mov eax, 0xE820     ; 功能号
    mov ecx, 20         ; ARDS结构大小（20字节）
    int 0x15            ; 调用BIOS中断
    
    jc .error           ; 如果CF置位，出错
    
    ; 检查签名
    cmp eax, 0x534d4150
    jne .error
    
    ; 增加计数器
    inc dword [ARDS_COUNT_ADDR]
    
    ; 移动到下一个ARDS
    add edi, 20
    
    ; 检查是否还有更多
    cmp ebx, 0
    jne .next_ards      ; 如果ebx不为0，继续
    
    ret

.error:
    mov si, msg_memory_error
    call print
    jmp $

; 数据区（放在代码后面）
msg_loading db "Loading kernel...", 13, 10, 0
msg_protected_mode db "Entering protected mode...", 13, 10, 0
msg_memory_detected db "Memory detected...", 13, 10, 0
msg_memory_error db "Memory detection error!", 13, 10, 0

