; boot.asm
section mbr vstart=0x7c00

jmp _start
LOADER_BASE_ADDR equ 0x900  ; loader 程序存放到0x900内存
LOADER_BASE_SECTOR equ 0x1  ; loader 存放在硬盘的第二个sector

_start:
    ; 初始化段寄存器
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov sp, 0x7c00

    ; 清空屏幕
    mov ah, 0x6
    mov bl, 0x7
    mov cx, 0
    mov dx, 0x184f
    int 0x10

    ; 文本模式
    mov ah, 0
    mov al, 0x3
    int 0x10

    ; 输出boot信息
    mov si, boot_msg
    call print

    ; 加载loader到内存
    mov ebx, LOADER_BASE_SECTOR
    mov cx, 1
    mov di, LOADER_BASE_ADDR
    call rd_disk

    ; 跳转到loader
    jmp 0:LOADER_BASE_ADDR

load_fail:
    mov si, error_msg
    call print
    jmp $

; 更简单的字符串打印函数
print:
    mov ah, 0x0E ; ah=Teletype Output
.loop:
    lodsb
    test al, al ; 检查是否为字符串结束符
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

; ------------------------------------------------------------------------
; 读取磁盘:rd_disk
; 参数:
; ebx 扇区逻辑号
; cx 读入的扇区数
; di 读取后的写入内存地址
; ------------------------------------------------------------------------  
rd_disk:
    ; 设置读取的扇区数
    mov al, cl
    mov dx, 0x1f2
    out dx, al
    
    ; 设置LBA地址（低24位）
    mov al, bl
    mov dx, 0x1f3
    out dx, al
    
    mov al, bh
    mov dx, 0x1f4
    out dx, al
    
    shr ebx, 16
    mov al, bl
    mov dx, 0x1f5
    out dx, al
    
    ; 设置高4位和device
    mov al, 0xE0 ; LBA模式，主硬盘
    mov dx, 0x1f6
    out dx, al
        
    ; 发送读命令
    mov al, 0x20
    mov dx, 0x1f7
    out dx, al
 
.ready: ; 检查磁盘状态
    nop
    in al, dx
    and al, 0x88
    cmp al, 0x08
    jnz .ready
    
    ; 计算要读取的字数（扇区数 * 256）
    mov ax, cx
    mov dx, 256
    mul dx
    mov cx, ax
    
    ; 读取数据
    mov dx, 0x1f0
    mov bx, di
.loop:                 
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .loop
    ret
 
boot_msg: db "hello xcore", 13, 10, 0
error_msg: db "Load error!", 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xaa55