AS = nasm
ASBINFLAGS = -f bin
ASELFFLAGS = -f elf32
DD = dd
DFLAGS = bs=512 count=1 conv=notrunc
QEMU = qemu-system-i386
QFLAGS = --drive format=raw,file=

BOCHS = bochs -q -f bochsrc

LD = ld
LDFLAGS=-m elf_i386 -T os.ld -nostdlib --nmagic
CC = gcc
CFLAGS=-c -m32 -O0 -ffreestanding -nostdlib -fno-pie -fno-stack-protector
OBJCOPY = objcopy

DEFAULT_TARGET = $(TARGET)
.PHONY = clean

BIN = bin
BOOT = src/boot
INIT = src/init
TARGET = $(BIN)/kernel.img
OBJS = $(BIN)/loader.bin $(BIN)/boot.bin $(BIN)/kernel.bin 
V = @

qemu: $(TARGET)
	$(V)$(QEMU) $(QFLAGS)$<

bochs: $(TARGET)
	$(V)$(BOCHS)

$(TARGET): $(OBJS)
	$(DD) if=/dev/zero of=$(TARGET) bs=512 count=3 2>/dev/null
	$(DD) if=$(BIN)/boot.bin of=$@ $(DFLAGS)
	$(DD) if=$(BIN)/loader.bin of=$@ $(DFLAGS) seek=1
	$(DD) if=$(BIN)/kernel.bin of=$@ $(DFLAGS) seek=2

$(BIN)/%.bin: $(BOOT)/%.asm
	$(AS) $(ASBINFLAGS) $(BOOT)/$*.asm -o $(BIN)/$*.bin

$(BIN)/%.bin: $(INIT)/%.asm
	$(AS) $(ASELFFLAGS) $(INIT)/$*.asm -o $(BIN)/$*.bin

$(BIN)/kernel.elf: src/kernel.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/kernel.bin: $(BIN)/init.bin $(BIN)/kernel.elf
	$(LD) $(LDFLAGS) $^ -o $@
	$(OBJCOPY) -O binary $@

clean:
	rm -rf bin/*.*