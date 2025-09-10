; boot.asm
org 0x7c00
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
; 重置光标
mov ah, 0x2
mov bx, 0
mov dx, 0
int 0x10
; 文本模式
mov ah, 0
mov al, 0x3
int 0x10

LOADER_BASE_ADDR equ 0x900  ; loader 程序存放到0x900内存
LOADER_BASE_SECTOR equ 0x1  ; loader 存放在硬盘的第二个sector

; 输出boot信息
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
    call print_string
    jmp $

print:
    mov ax, message
    mov bp, ax
    mov cx, 13
    mov ax, 0x1301
    mov bx, 0xc
    mov dl, 0
    int 0x10
    ret

; 更简单的字符串打印函数
print_string:
    mov ah, 0x0E
.print_loop:
    lodsb
    test al, al
    jz .print_done
    int 0x10
    jmp .print_loop
.print_done:
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
.read_loop:                 
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .read_loop
    ret
 
message: db "hello xcore", 13, 10, 0
error_msg: db "Load error!", 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xaa55