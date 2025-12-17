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
    ; 设置代码段和数据段的基地址为0
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
    
    mov eax, LABEL_GDT
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
    mov ax, SelectorVideo
    mov gs, ax
    ; line 11, col 0
    mov byte [gs:80*11 + 0], 'O'
    mov byte [gs:80*11 + 1], 0xc
    mov byte [gs:80*11 + 2], 'K'
    mov byte [gs:80*11 + 3], 0xc
    
    ; 跳转到内核
    jmp dword SelectorCode:KERNEL_BASE_ADDR

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
    mov al, ah
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


; 数据区（放在代码后面）
msg_loading db "Loading kernel...", 13, 10, 0
msg_protected_mode db "Entering protected mode...", 13, 10, 0

