org 0x7c00

global _start
_start:
    xchg bx, bx
; 清屏
    mov ax, 0x13
    int 0x10
; 文本模式
    mov ax, 0x03
    int 0x10
; 初始化寄存器
    xor ax, ax  ; 异或相同的值得到0,即 ax = 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; xchg bx, bx ; bochs 使用的断点
    mov si, bootMsg
    call print

    mov edi, 0x1000 ; 读取到的内存地址
    mov ecx, 2      ; 起始扇区
    mov bl, 4       ; 读取的扇区数

    call read_disk
    cmp word [0x1000], 0x55aa
    jnz error

    jmp 0:0x1002

    jmp $
; 直接与硬盘控制器进行交互，使用低级的命令和寄存器设置来完成读取操作。
; 每个步骤都涉及与硬盘控制器交互，设置扇区数、扇区地址、操作模式，
; 然后发出读取命令并等待硬盘准备好，最后通过数据寄存器读取硬盘数据。
read_disk:
; dx 被设置为硬盘控制器的 0x1f2 端口地址，这是硬盘命令寄存器中的“扇区计数”寄存器。
; bl 是存储要读取的扇区数。它的值被加载到 al 寄存器中，
; 然后通过 out dx, al 命令写入到硬盘控制器的 0x1f2 端口，告诉硬盘驱动器要读取几个扇区。
    ; 设置读写扇区数 4
    mov dx, 0x1f2   ; dx = 0x1f2 端口号
    mov al, bl    ; 读取的扇区数    al = 4
    out dx, al      ; 向端口号为0x1f2的端口写入al

; 接下来的部分设置了硬盘要读取的扇区的扇区号。每个硬盘扇区地址通常是32位（LBA模式），分为多个字节写入。
; dx 递增，指向硬盘控制器的 0x1f3 端口，
; 这是硬盘控制器的 "扇区号" 寄存器（低字节）。cl 的值被加载到 al 寄存器并写入到该寄存器。
    inc dx  ; 0x1f3
    mov al, cl      ; cl = 扇区号的低字节
    out dx, al
; dx 递增，指向硬盘控制器的 0x1f4 端口，这是硬盘控制器的 "扇区号" 寄存器（高字节）
; ecx 被右移8位，cl 寄存器的值作为扇区号的高字节写入。
    inc dx  ; 0x1f4
    shr ecx, 8
    mov al, cl  ; cl = 扇区号的高字节
    out dx, al
; 继续递增 dx，指向 0x1f5 寄存器，重复将 ecx 的高字节写入。
    inc dx  ; 0x1f5
    shr ecx, 8
    mov al, cl  ; 
    out dx, al
; 继续递增 dx，指向硬盘控制器的 0x1f6 端口，这个端口用于设置硬盘的操作模式。
; ecx 的值再次右移8位，cl 被限制为 4 位（即 and cl, 0b1111）。这表示要设置LBA（逻辑块寻址）模式的驱动器和分区信息。
; al 被设置为 0xe0（二进制：1110 0000），这个值表示在LBA模式下选择驱动器。
; 使用 or al, cl 合并 cl 和 0xe0，然后将最终值输出到 0x1f6 端口，完成LBA模式的设置。
    inc dx  ; 0x1f6
    shr ecx, 8
    and cl, 0b1111

    mov al, 0xe0    ; 1110 0000
    or al, cl
    out dx, al  ; 主盘 LBA 模式

; dx 被递增到 0x1f7 端口，这个端口用于向硬盘控制器发出命令。
; 0x20 表示硬盘的读取命令，写入到 0x1f7 端口，硬盘开始执行读取操作。
    inc dx  ; 0x1f7
    mov al, 0x20    ; 读硬盘
    out dx, al

    xor ecx, ecx
    mov cl, bl
.read:
    push cx
    call .wait
    call .reads
    pop cx
    loop .read
    ret
; dx 设置为 0x1f7，这是硬盘的状态寄存器。
; 使用 in al, dx 从硬盘控制器读取状态值。
; 使用 and al, 0x88 只检查状态的第4和第7位。这些位用于表示硬盘是否准备好读写。
; cmp al, 0x08 检查第3位是否为1，这表示硬盘准备好进行读操作。
; 如果硬盘没有准备好，代码会继续跳回 .check 循环等待。
.wait:
    mov dx, 0x1f7
.check:
    in al, dx
    jmp $+2
    jmp $+2
    jmp $+2
    and al, 0x88    ; 1000 1000
    cmp al, 0x08    ; 0000 1000
    jnz .check
    ret
; dx 设置为 0x1f0，这是硬盘的数据寄存器。
; cx 被设置为 256，表示将要读取的字节数（每次读取2字节）。
; 使用 in ax, dx 从硬盘控制器读取2字节数据。
; 数据被存储到 edi 指向的内存位置，edi 自动增加2，准备存储下一组数据。
; 使用 loop .readw 重复读取，直到读取完所有的数据。
.reads:
    mov dx, 0x1f0
    mov cx, 256 ; 读取的字节数
.readw:
    in ax, dx
    jmp $+2
    jmp $+2
    jmp $+2
    mov [edi], ax
    add edi, 2
    loop .readw 
    ret

print:
    mov ah, 0x0e
.next:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret
    
bootMsg: db "xcore os.", 10, 13, 0  ; \n\r

error:
    mov si, errorMsg
    call print
    hlt         ; 停止CPU
    jmp $

errorMsg: db "error", 10, 13, 0

times 510 - ($ - $$) db 0
dw 0xaa55
