CC=gcc
OBJCP=objcopy
LD=ld
CCOPTS=-m32 -ffreestanding -g
LDOPTS=-m elf_i386
SRC=./src
BIN=./bin
KCSRC := $(shell find $(SRC) -name "*.c")
KCTAR := $(patsubst $(SRC)/%.c,$(BIN)/%.o,$(KCSRC))
KASMSRC := $(filter-out $(SRC)/empty.asm $(wildcard $(SRC)/bootloader/*),$(shell find $(SRC) -name "*.asm"))
KASMTAR := $(patsubst $(SRC)/%.asm,$(BIN)/%.o,$(KASMSRC))
LDPRIORITY := $(BIN)/kernel/entry.o $(BIN)/kernel/main.o

all: run

clean:
	rm -rf $(BIN)
	mkdir $(BIN)

build: clean kernel boot
	cat $(BIN)/boot.bin > $(BIN)/osimage.bin
	cat $(BIN)/kernel.bin >> $(BIN)/osimage.bin
	fasm -d SIZE=$$((512*($$(stat -c "%s" $(BIN)/kernel.bin)/512+1)-$$(stat -c "%s" $(BIN)/kernel.bin))) $(SRC)/empty.asm $(BIN)/empty.bin
	cat $(BIN)/empty.bin >> $(BIN)/osimage.bin

boot: kernel
	fasm -d K_SIZE=$$(($$(stat -c "%s" $(BIN)/kernel.bin)/512+1)) $(SRC)/bootloader/boot.asm $(BIN)/boot.bin
	echo -e "Kernel binary is $$(($$(stat -c "%s" $(BIN)/kernel.bin)/512+1)) sectors long."

kernel: $(KASMTAR) $(KCTAR)
	$(LD) $(LDOPTS) -o $(BIN)/kernel.elf -Ttext 0x8000 $(LDPRIORITY) --start-group $(filter-out $(LDPRIORITY),$(KCTAR) $(KASMTAR)) --end-group --oformat elf32-i386
	$(OBJCP) -O binary $(BIN)/kernel.elf $(BIN)/kernel.bin
	$(OBJCP) --only-keep-debug $(BIN)/kernel.elf $(BIN)/kernel.sym

$(BIN)/%.o: $(SRC)/%.c
	mkdir -p $(patsubst $(SRC)%,$(BIN)%,./$(shell dirname $<))
	$(CC) $(CCOPTS) -c $< -o $(patsubst $(SRC)%.c,$(BIN)%.o,./$<) -I $(SRC)

$(BIN)/%.o: $(SRC)/%.asm
	mkdir -p $(patsubst $(SRC)%,$(BIN)%,./$(shell dirname $<))
	fasm $< $(patsubst $(SRC)%.asm,$(BIN)%.o,./$<)

run_current:
	qemu-system-x86_64 -drive format=raw,file=$(BIN)/osimage.bin,index=0,if=floppy -m 128M

run: build run_current

debug_current:
	qemu-system-x86_64 -drive format=raw,file=$(BIN)/osimage.bin,index=0,if=floppy -m 128M -s -S &
	gdb -s $(BIN)/kernel.sym -ex "target remote localhost:1234"

debug: build debug_current

