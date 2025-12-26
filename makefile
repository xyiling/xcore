AS = nasm
ASBINFLAGS = -f bin
ASELFFLAGS = -f elf32
DD = dd
DFLAGS = bs=512 count=1 conv=notrunc
QEMU = qemu-system-i386
QFLAGS = --drive format=raw,file=

BOCHS = bochsdbg -q -f bochsrc.bxrc

LD = i686-elf-ld
LDFLAGS=-m elf_i386 -g -T os.ld -nostdlib
CC = i686-elf-gcc
CFLAGS=-c -m32 -march=i386 -fno-builtin -fno-omit-frame-pointer -Wall -Wextra -Werror -O0 -g -ffreestanding -nostdlib -fno-pie -fno-stack-protector -std=c11 -I src/include
OBJCOPY = i686-elf-objcopy

BIN = bin
BOOT = src/boot
INIT = src/init
TARGET = $(BIN)/kern.img
OBJS = $(BIN)/loader.bin $(BIN)/boot.bin $(BIN)/kern.elf
V = @

.PHONY = all clean qemu debug

# 默认目标：只构建镜像，不启动模拟器
all: $(TARGET)

# 启动 QEMU
qemu: $(TARGET)
	$(V)$(QEMU) $(QFLAGS)$<

# 使用 Bochs 调试
debug: $(TARGET)
	$(V)$(BOCHS)

$(TARGET): $(OBJS)
	$(DD) if=/dev/zero of=$(TARGET) bs=512 count=14400 2>/dev/null
	$(DD) if=$(BIN)/boot.bin of=$@ $(DFLAGS)
	$(DD) if=$(BIN)/loader.bin of=$@ $(DFLAGS) seek=1
	# 写入内核：将整个 kern.elf 写入镜像（不只一扇区）
	$(DD) if=$(BIN)/kern.elf of=$@ bs=512 conv=notrunc seek=2

$(BIN)/%.bin: $(BOOT)/%.asm
	$(V)mkdir -p $(BIN)
	$(AS) $(ASBINFLAGS) -I src/boot/ $(BOOT)/$*.asm -o $(BIN)/$*.bin

$(BIN)/%.bin: $(INIT)/%.asm
	$(AS) $(ASELFFLAGS) $(INIT)/$*.asm -o $(BIN)/$*.bin

# 源文件列表
KERNEL_SRCS = src/kern.c \
              src/lib/string.c \
              src/lib/stdio.c \
              src/lib/stdlib.c \
              src/kernel/memory.c \
              src/kernel/interrupt.c \
              src/kernel/pic.c \
              src/kernel/syscall.c \
              src/kernel/task.c \
              src/kernel/fs.c \
              src/kernel/shell.c

# 汇编源文件
ASM_SRCS = src/kernel/interrupt.asm \
           src/kernel/syscall.asm

# 目标文件
KERNEL_OBJS = $(KERNEL_SRCS:src/%.c=$(BIN)/%.o)
ASM_OBJS = $(ASM_SRCS:src/%.asm=$(BIN)/asm/%.o)

$(BIN)/kern.elf: $(BIN)/init.bin $(KERNEL_OBJS) $(ASM_OBJS)
	$(LD) $(LDFLAGS) $^ -o $(BIN)/kern.elf.tmp
	$(OBJCOPY) -O binary $(BIN)/kern.elf.tmp $@

# C源文件编译规则
$(BIN)/%.o: src/%.c
	$(V)mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# 汇编源文件编译规则（放到 bin/asm/... 以避免与C对象冲突）
$(BIN)/asm/%.o: src/%.asm
	$(V)mkdir -p $(dir $@)
	$(AS) $(ASELFFLAGS) $< -o $@

clean:
	rm -rf bin

msg ?= "update"
submit:
	git add .
	git commit -m "$(msg)"
	git push origin main