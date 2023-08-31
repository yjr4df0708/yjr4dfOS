; parts copied and edited from https://github.com/mell-o-tron/MellOs/blob/main/Bootloader/boot.asm
org 0x7c00
use16

kernel=0x8000

	;store boot disk number
	mov [BOOT_DISK], dl

	;set segment registers and stack
	cli
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00
	sti

	call detect_memory

	mov dl, [BOOT_DISK]
	call load_kernel

	cli

	lgdt [gdt_descriptor]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp CODE_SEG:main32

use32
main32:
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	;set up the stack
	mov ebp, 0x90000
	mov esp, ebp
	
	;enable the A20 line
	in al, 0x92
	or al, 0x02
	out 0x92, al

	jmp kernel

cli
hlt

use16
include "memory.asm"
include "load_kernel.asm"
include "gdt.asm"

BOOT_DISK:
	rb 1

B_ERR:
	mov ah, 0x0e
	mov al, [ERRC]
	int 0x10
	mov ah, 0x0e
	mov al, [ERRC+1]
	int 0x10
	cli
	hlt

ERRC:
	rb 2

rb $$+510-$
db 0x55, 0xAA
