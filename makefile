AS = nasm
ASBINFLAGS = -f bin
ASELFFLAGS = -f elf32
DD = dd
DFLAGS = bs=512 count=1 conv=notrunc
QEMU = qemu-system-i386
QFLAGS = --drive format=raw,file=

BOCHS = bochsdbg -q -f bochsrc.bxrc

LD = i686-elf-ld
LDFLAGS=-m elf_i386 -T os.ld -nostdlib
CC = i686-elf-gcc
CFLAGS=-c -m32 -O0 -ffreestanding -nostdlib -fno-pie -fno-stack-protector
OBJCOPY = i686-elf-objcopy
DEFAULT_TARGET = $(TARGET)
.PHONY = clean

BIN = bin
BOOT = src/boot
INIT = src/init
TARGET = $(BIN)/kern.img
OBJS = $(BIN)/loader.bin $(BIN)/boot.bin $(BIN)/kern.bin 
V = @

qemu: $(TARGET)
	$(V)$(QEMU) $(QFLAGS)$<

bochs: $(TARGET)
	$(V)$(BOCHS)

$(TARGET): $(OBJS)
	$(DD) if=/dev/zero of=$(TARGET) bs=512 count=14400 2>/dev/null
	$(DD) if=$(BIN)/boot.bin of=$@ $(DFLAGS)
	$(DD) if=$(BIN)/loader.bin of=$@ $(DFLAGS) seek=1
	$(DD) if=$(BIN)/kern.bin of=$@ $(DFLAGS) seek=2

$(BIN)/%.bin: $(BOOT)/%.asm
	mkdir -p $(BIN)
	$(AS) $(ASBINFLAGS) -I src/boot/ $(BOOT)/$*.asm -o $(BIN)/$*.bin

$(BIN)/%.bin: $(INIT)/%.asm
	$(AS) $(ASELFFLAGS) $(INIT)/$*.asm -o $(BIN)/$*.bin

$(BIN)/kern.elf: src/kern.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/kern.bin: $(BIN)/init.bin $(BIN)/kern.elf
	$(LD) $(LDFLAGS) $^ -o $@
	$(OBJCOPY) -O binary $@

clean:
	rm -rf bin