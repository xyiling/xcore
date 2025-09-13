.DEFAULT_GOAL := qemu
V := @
QEMU := qemu-system-x86_64
BOCHS := bochs
BFLAGS := -q -f bochsrc.txt
QFLAGS := -S -s
CC := gcc
CFLAGS := -stdnolib -g -Wall 
TERMINAL := gnome-terminal
GDB := gdb

C_SRC := $(wildcard src/*.c)
ASM_SRC := $(wildcard src/*.asm)

OBJ := $(C_SRC:src/%.c=bin/%.o) 
OBJ += $(ASM_SRC:src/%.asm=bin/%.o)

TARGET = bin/kernel.img

%.o: src/%.asm
	$(V)mkdir -p bin
	nasm -g -f bin -o bin/$@ $<

$(TARGET): boot.o loader.o
	yes | bximage -q -hd=16 -mode=create -sectsize=512 -imgmode=flat $@
	dd if=bin/boot.o of=$@ bs=512 count=1 conv=notrunc
	dd if=bin/loader.o of=$@ bs=512 count=4 seek=2 conv=notrunc

qemu: $(TARGET)
	$(V)$(QEMU) $(QFLAGS) $< &
	$(V)sleep 1
	$(V)$(TERMINAL) -- bash -c "gdb -x scripts/gdbinit"

run: $(TARGET)
	$(V)$(QEMU) $<

bochs: $(TARGET)
	$(BOCHS) $(BFLAGS)

.PHONY: clean
clean:
	rm -rf bin
