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

%.bin: %.S
	nasm -g -f bin -o $@ $<

kernel.img: boot.bin loader.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc

qemu: kernel.img
	$(V)$(QEMU) $(QFLAGS) $< &
	$(V)sleep 1
	$(V)$(TERMINAL) -- bash -c "gdb -x gdbinit"

run: kernel.img
	$(V)$(QEMU) $<

bochs: kernel.img
	$(BOCHS) $(BFLAGS)

.PHONY: clean
clean:
	rm *.bin *.img *.ini
