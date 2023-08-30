CC=/usr/local/i386elfgcc/bin/i386-elf-gcc
OBJCP=/usr/local/i386elfgcc/bin/i386-elf-objcopy
LD=/usr/local/i386elfgcc/bin/i386-elf-ld
SRC=./src
BIN=./bin
KCSRC := $(shell find $(SRC)/kernel -name "*.c")
KCTAR := $(patsubst $(SRC)/%.c,$(BIN)/%.o,$(KCSRC))
KASMSRC := $(shell find $(SRC)/kernel -name "*.asm")
KASMTAR := $(patsubst $(SRC)/%.asm,$(BIN)/%.o,$(KASMSRC))
LDPRIORITY := $(BIN)/kernel/entry.o $(BIN)/kernel/main.o

all: build run

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

kernel: $(KASMTAR) $(KCTAR)
	$(LD) -o $(BIN)/kernel.elf -Ttext 0x8000 $(LDPRIORITY) --start-group $(filter-out $(LDPRIORITY),$(shell find $(BIN)/kernel -name "*.o" | xargs)) --end-group --oformat elf32-i386
	$(OBJCP) -O binary $(BIN)/kernel.elf $(BIN)/kernel.bin

$(BIN)/%.o: $(SRC)/%.c
	mkdir -p $(patsubst $(SRC)%,$(BIN)%,./$(shell dirname $<))
	$(CC) -c $< -o $(patsubst $(SRC)%.c,$(BIN)%.o,./$<) $(addprefix -I ,$(shell dirname $(shell echo $(KCSRC) | tr ' ' '\n' | sort -u | xargs)))

$(BIN)/%.o: $(SRC)/%.asm
	mkdir -p $(patsubst $(SRC)%,$(BIN)%,./$(shell dirname $<))
	fasm $< $(patsubst $(SRC)%.asm,$(BIN)%.o,./$<)

run:
	qemu-system-x86_64 -drive format=raw,file=$(BIN)/osimage.bin,index=0,if=floppy -m 128M